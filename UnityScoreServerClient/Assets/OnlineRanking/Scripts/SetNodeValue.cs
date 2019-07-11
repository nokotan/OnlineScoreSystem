using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class SetNodeValue : MonoBehaviour {
    /// <summary>
    /// ランキングの順位を書いている Text コンポーネントへの参照
    /// </summary>
    [Header("Rank"), SerializeField]
    private Text RankingText;
    /// <summary>
    /// ランキングの順位を書くときのフォーマット
    /// </summary>
    [SerializeField]
    private string RankingTextFormat = "{0}位";

    /// <summary>
    /// プレイヤーの名前を書いている Text コンポーネントへの参照
    /// </summary>
    [Header("PlayerName"), SerializeField]
    private Text PlayerNameText;

    /// <summary>
    /// スコアを書いている Text コンポーネントへの参照
    /// </summary>
    [Header("Score"), SerializeField]
    private Text ScoreText;
    /// <summary>
    /// スコアを書くときのフォーマット
    /// </summary>
    [SerializeField]
    private string ScoreTextFormat = "{0:000000}";

    /// <summary>
    /// ノードにプレイヤーのランキング情報を書き込みます。
    /// </summary>
    /// <param name="Data">書き込むプレイヤーの情報</param>
    public void SetPlayerData(int Rank, RankingPlayerData Data) {
        RankingText.text = string.Format(RankingTextFormat, Rank);
        PlayerNameText.text = Data.PlayerName;
        ScoreText.text = string.Format(ScoreTextFormat, Data.Score);
    }
}
