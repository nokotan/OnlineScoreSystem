<?php 
/*----------------------------------------------------

            簡易ランキングサーバプログラム
        
  ==================================================

    スコア登録、スコア表示などの最低限の機能だけをつ
  けてあります。

  主な仕様
  ・スコアデータはテキストファイルに保存
  ・クライアント側からのHttpリクエストに応じてスコア
    を登録

  ==================================================

    不明な点などはのこたんまたは以下の目安箱まで。
    https://goo.gl/forms/MMV6lUOwot3kdDZE2

 ver 0.1 (2017/ 6/21) --------------------------------*/
 ?>

<?php // モデル // ?>
<?php
    // ランキングデータにアクセスする機能を実装
    class RankingData {
        function __construct($FileName) {
            $this->FilePath = $FileName;

            if (!file_exists($FileName)) {
                touch($FileName);
                $this->ClearRanking();
            }
        }

        // ランキングを配列として取得します。
        function LoadRanking() {
            $Data = json_decode(file_get_contents($this->FilePath), true);
            return $Data['Ranking'];
        }

        // ランキングを保存します。
        private function SaveRanking($Data) {
            $Serialized = json_encode($Data);

            $FileHandle = fopen($this->FilePath, 'w');
            fputs($FileHandle, $Serialized);
            fclose($FileHandle);
        }

        // ランキングをクリアします。
        function ClearRanking() {
            $this->SaveRanking(array('Ranking' => array()));
        }

        // ランキングにデータを登録します。
        function RegisterRanking($NewData) {
            // 現在のランキングを取得
            $Data = $this->LoadRanking();
            // ランキングの末尾にデータを追加
            array_push($Data, $NewData);

            // ランキングをソート
            usort($Data, function($a, $b) {
                return  $b['Score'] - $a['Score'] != 0 ?
                        $b['Score'] - $a['Score'] :
                        ($b['TimeStamp'] > $a['TimeStamp'] ? 1 : -1);
            });

            // ランキングを保存
            $this->SaveRanking(array('Ranking' => $Data));
        }
    }
?>

<?php // コントローラ // ?>
<?php
	// 入力によってランキングを操作する機能を実装
    class RankingController {
        private $Ranking;
		private $FilePath = './Ranking.json';

        function __construct() {
            $this->Ranking = new RankingData($this->FilePath);
        }

		// ページが読み込まれたらするべき処理
        function OnLoad() {
            $this->ClearRanking();
            $this->UpdateRanking();
			$this->CheckRequestedFormat();
        }

		// 引数によってランキングを更新します。
        function UpdateRanking() {
			// 取得するメソッド。 Get または Post
			$Arguments = $_POST;

            if (isset($Arguments['Score']) && isset($Arguments['PlayerName'])) {
                $Data = array(
                    'Score' => (int)$Arguments['Score'],
                    'TimeStamp' => date('Y-m-d H-i-s'),
                    'PlayerName' => mb_convert_encoding($Arguments['PlayerName'], 'UTF-8', 'auto'),
                );

                $this->Ranking->RegisterRanking($Data);
            }
        }

		// ランキングを初期化します。
        function ClearRanking() {
            if (isset($_GET['Reset'])) {
                $this->Ranking->ClearRanking();
            }
        }

		// ランキングを取得します。
        function GetRanking() {
            return $this->Ranking->LoadRanking();
        }

		// 要求されたデータ形式によってファイルを出力します。
		function CheckRequestedFormat() {
			if (isset($_GET['Format'])) {
				$FormatHandlers = array(
					'Json' => function() { print(file_get_contents($this->FilePath)); },
					'CSV' => function() {
						foreach ($this->GetRanking() as $PlayerData) {
							foreach ($PlayerData as $Element) {
								print($Element.',');
							}
							print("\r\n");
						}
					}
				);

				if (array_key_exists($_GET['Format'], $FormatHandlers)) {
					$FormatHandlers[$_GET['Format']]();
					die(200);
				}
			}
		}
    }
?>

<?php // メイン処理 // ?>
<?php
    // 時刻設定、ロケール設定
	date_default_timezone_set('Asia/Tokyo');
	setlocale(LC_ALL, 'ja_JP');

    $Controller = new RankingController();
    $Controller->OnLoad();

    $RankingCounter = 0;
?>

<?php // ビュー // ?>
<!DOCTYPE HTML>

<html>
    <head>
        <meta http-equiv="content-type" charset="utf-8" />
        <meta name="viewport" content="width=device-width" />

        <title>簡易スコアランキング</title>
    </head>

    <body>
        <h1>簡易スコアランキング</h1>

        <table>

            <?php foreach ($Controller->GetRanking() as $Item) { ?>
            <?php     $RankingCounter++; ?>

            <tr>
				<td><?php print($RankingCounter); ?>位</td>
                <td><?php print(htmlspecialchars($Item['PlayerName'], ENT_QUOTES)); ?></td>
                <td><?php print($Item['Score']); ?></td>
            </tr>

            <?php } ?>

        </table>
    </body>
</html>
