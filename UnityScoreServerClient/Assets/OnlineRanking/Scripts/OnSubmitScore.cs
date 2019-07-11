using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class OnSubmitScore : MonoBehaviour {
    /// <summary>
    /// プレイヤー名を入力するフィールドへの参照
    /// </summary>
    [Header("SendData")]
    public UnityEngine.UI.InputField PlayerNameField;

    /// <summary>
    /// 送信するスコア
    /// </summary>
    public int Score = 100;

    public void OnSubmit() {
        var PlayerData = new RankingPlayerData();

        PlayerData.PlayerName = PlayerNameField.text;
        PlayerData.Score = 100;

        var ScoreSystem = GameObject.FindGameObjectWithTag("ScoreSystem");
        var ScoreSystemClient = ScoreSystem.GetComponent<OnlineScoreSystemClient>();
        ScoreSystemClient.SendScore(PlayerData);
        ScoreSystemClient.UpdateRanking();
    }
}
