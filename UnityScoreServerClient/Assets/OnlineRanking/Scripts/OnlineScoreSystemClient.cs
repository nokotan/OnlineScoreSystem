using System.Collections;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Net;
using System.Text;
using UnityEngine;

[System.Serializable]
public class RankingPlayerData {
    /// <summary>
    /// プレイヤーの名前
    /// </summary>
    public string PlayerName;

    /// <summary>
    /// スコア
    /// </summary>
    public int Score;
}

[System.Serializable] 
public class RankingData {
    /// <summary>
    /// ランキングの配列
    /// </summary>
    public RankingPlayerData[] Ranking;
}

public class OnlineScoreSystemClient : MonoBehaviour {

    /// <summary>
    /// 簡易スコアサーバの配置アドレス
    /// </summary>
    [Header("Network")]
    public string URL = "http://qpic.jp/contents/43/ScoreServer.php?Format=Json";

    /// <summary>
    /// ランキングを格納すべき場所
    /// </summary>
    [Header("RankingView")]
    public GameObject RankingViewContents;
    /// <summary>
    /// ランキングノードのプレハブ
    /// </summary>
    public GameObject NodePrefab;

    void Start() {
        UpdateRanking();
    }

    /// <summary>
    /// スコアを送信します。
    /// </summary>
    /// <param name="Data">送信するデータ</param>
    public void SendScore(RankingPlayerData Data) {
        WebClient HttpClient = new WebClient();
        NameValueCollection PostValues = new NameValueCollection();

        PostValues.Add("PlayerName", Data.PlayerName);
        PostValues.Add("Score", Data.Score.ToString());

        try {
            var Response = HttpClient.UploadValues(URL, PostValues);
            var ResponseStringOffset = 3;
            var ResponseString = Encoding.UTF8.GetString(Response, ResponseStringOffset, Response.Length - ResponseStringOffset);

            Debug.Log(ResponseString);
        } catch (WebException e) {
            Debug.Log("スコア送信に失敗しました。ネットワークへの接続状況を確認してください。\n Details : " + e.Message);
        }
    }

    /// <summary>
    /// ランキングを読み込みます。
    /// </summary>
    public RankingData LoadRanking() {
        WebClient HttpClient = new WebClient();
        NameValueCollection PostValues = new NameValueCollection();

        RankingData Ranking = null;

        try {
            var Response = HttpClient.UploadValues(URL, PostValues);
            var ResponseStringOffset = 3;
            var ResponseString = Encoding.UTF8.GetString(Response, ResponseStringOffset, Response.Length - ResponseStringOffset);
            
            Debug.Log(ResponseString);
            Ranking = JsonUtility.FromJson<RankingData>(ResponseString);
        } catch (WebException e) {
            Debug.Log("スコア受信に失敗しました。ネットワークへの接続状況を確認してください。\n Details : " + e.Message);
        }

        return Ranking;
    }

    /// <summary>
    /// ランキングを更新します。
    /// </summary>
    public void UpdateRanking() {
        // 以前のランキングをリセット
        for (int i = 0; i < RankingViewContents.transform.childCount; i++) {
            GameObject.Destroy(RankingViewContents.transform.GetChild(i).gameObject);
        }

        // 新しいランキングを作成
        var NewRanking = LoadRanking();

        if (NewRanking != null) {
            for (int i = 0; i < NewRanking.Ranking.Length; i++) {
                var NewNode = GameObject.Instantiate(NodePrefab, RankingViewContents.transform);
                var NodeValueSetter = NewNode.GetComponent<SetNodeValue>();
                NodeValueSetter.SetPlayerData(i + 1, NewRanking.Ranking[i]);
            }
        }
    }
}
