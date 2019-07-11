#include "DxLib.h"

// 簡易スコアサーバクライアント --------------------------------------------
//
//   　簡易スコアサーバにスコアとプレイヤー名を送信するプログラムです。
//   日本語入力やマウス入力など、まだ使い方を教えていない関数を数多く
//   使っているので、適宜 DX ライブラリのリファレンスを参照してください。
//   (DXライブラリ 関数一覧 http://dxlib.o.oo7.jp/dxfunc.html)
//
// ver 1.0 (2017/ 6/21) ----------------------------------------------------



// 設定 --------------------------------------------------------------------

// 簡易スコアサーバの配置場所
const char ScoreServerUrl[] = "http://qpic.jp/contents/43/ScoreServer.php?Format=CSV";

// -------------------------------------------------------------------------



// 文字列入力フィールドの実装 ----------------------------------------------

// 文字列入力フィールド
struct TextField {
	int X, Y, Width, Height;

	int InputHandle;
	int BackColor, ForeColor;
};

// 1フレーム前にマウスの左ボタンが押されていたかどうか
bool MouseLeftPressedInPreviousFlame = false;

// 文字列入力フィールドが編集中かどうか
bool TextFieldIsEditing(TextField Field) {
	if (CheckKeyInput(Field.InputHandle) != 0) {
		return false;
	} else {
		return true;
	}
}

// 文字列入力フィールドの入力された文字列を取得します
void TextFieldGetText(TextField Field, char Buffer[]) {
	GetKeyInputString(Buffer, Field.InputHandle);
}

// 文字列入力フィールドを更新します
void UpdateTextField(TextField Field) {
	if (GetMouseInput() & MOUSE_INPUT_LEFT) {
		if (MouseLeftPressedInPreviousFlame == false) {
			int MouseX, MouseY;
			GetMousePoint(&MouseX, &MouseY);

			// 文字列入力フィールドの中で左クリックされたら入力できる状態に
			if (Field.X <= MouseX && MouseX < Field.X + Field.Width && Field.Y <= MouseY && MouseY < Field.Y + Field.Height) {
				SetActiveKeyInput(Field.InputHandle);
			} else {
				SetActiveKeyInput(-1);
			}
		}

		MouseLeftPressedInPreviousFlame = true;
	} else {
		MouseLeftPressedInPreviousFlame = false;
	}
}

// 文字列入力フィールドを描画します
void DrawTextField(TextField Field) {
	DrawBox(Field.X, Field.Y, Field.X + Field.Width, Field.Y + Field.Height, Field.BackColor, TRUE);

	if (TextFieldIsEditing(Field)) {
		DrawKeyInputString(Field.X, Field.Y, Field.InputHandle);
	} else {
		char TextBuffer[256];
		GetKeyInputString(TextBuffer, Field.InputHandle);
		DrawString(Field.X, Field.Y, TextBuffer, Field.ForeColor);
	}
}

// -------------------------------------------------------------------------



// スコア送信部分 ----------------------------------------------------------

// スコアを送信します。
// 戻り値: 0 = 成功, -1 = 失敗
int SendScore(char PlayerName[], int Score) {
	IPDATA IP = { 127, 0, 0, 1 };

	if (GetHostIPbyName("qpic.jp", &IP) == -1) {
		return -1;
	}

	int NetHandle = ConnectNetWork(IP, 80);

	if (NetHandle == -1) {
		return -1;
	}

	// Http リクエストの作成
	char SendBuffer[1024 * 8];
	char Data[128];
	
	sprintf_s(Data, "PlayerName=%s&Score=%d", PlayerName, Score);

	sprintf_s(SendBuffer,
		"POST %s HTTP/1.1\r\n"
		"Host: qpic.jp:80\r\n"
		"Content-Length: %d\r\n"
		"Content-Type: application/x-www-form-urlencoded\r\n"
		"\r\n"
		"%s",
		ScoreServerUrl, strlen(Data), Data);

	// Http リクエストを送信
	NetWorkSend(NetHandle, SendBuffer, strlen(SendBuffer));

	// 強引だがこの関数を呼ぶと強制的にデータを送信する。
	WaitTimer(0);

	int TotalReceivedLength = 0, Length = 0;

	// 送信したリクエストに対する Http レスポンスの取得
	while ((Length = GetNetWorkDataLength(NetHandle)) > 0) {
		if (NetWorkRecv(NetHandle, &SendBuffer[TotalReceivedLength], Length) == -1) {
			CloseNetWork(NetHandle);
			return -1;
		} else {
			TotalReceivedLength += Length;
		}
	}

	SendBuffer[TotalReceivedLength] = '\0';

	CloseNetWork(NetHandle);

	return 0;
}

// -------------------------------------------------------------------------



// メイン関数 --------------------------------------------------------------

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	
	ChangeWindowMode(TRUE);
	SetMainWindowText("Ranking Client");
	SetGraphMode(640, 480, 32);
	SetBackgroundColor(128, 128, 255);

	if (DxLib_Init() == -1) {
		return -1;
	}

	SetDrawScreen(DX_SCREEN_BACK);
	SetUseDXNetWorkProtocol(FALSE);

	TextField NameField;

	NameField.X = 350;
	NameField.Y = 400;
	NameField.Width = 240;
	NameField.Height = 20;
	// 日本語入力関数 (MakeKeyInput など)
	// http://dxlib.o.oo7.jp/function/dxfunc_input.html#R5N13
	NameField.InputHandle = MakeKeyInput(12, FALSE, FALSE, FALSE);
	NameField.ForeColor = GetColor(255, 255, 255);
	NameField.BackColor = GetColor(0, 0, 0);

	bool EnterPressedInPreviousFlame = false;
	bool OPressedInPreviousFlame = false;

	while (ProcessMessage() != -1) {
		ClearDrawScreen();

		// [Ctrl]+[O] が押されたらブラウザを開く
		if (CheckHitKey(KEY_INPUT_O)) {
			if (OPressedInPreviousFlame == false && (CheckHitKey(KEY_INPUT_LCONTROL) || CheckHitKey(KEY_INPUT_RCONTROL))) {
				// ShellExecute 関数の使い方
				// http://ohwhsmm7.blog28.fc2.com/blog-entry-105.html
				ShellExecute(NULL, "Open", ScoreServerUrl, NULL, NULL, SW_SHOWDEFAULT);
			}

			OPressedInPreviousFlame = true;
		} else {
			OPressedInPreviousFlame = false;
		}

		// [Enter] が押されたらデータ送信
		if (CheckHitKey(KEY_INPUT_RETURN)) {
			if (EnterPressedInPreviousFlame == false && TextFieldIsEditing(NameField) == false) {
				char PlayerName[64];
				int Score = 100;

				TextFieldGetText(NameField, PlayerName);

				if (SendScore(PlayerName, Score) != -1) {
					printfDx("Score Sended. PlayerName = %s, Score = %d\n", PlayerName, Score);
				} else {
					printfDx("Network Error. \n");
				}
			}

			EnterPressedInPreviousFlame = true;
		} else {
			EnterPressedInPreviousFlame = false;
		}

		// [Escape] が押されたらログをクリア
		if (CheckHitKey(KEY_INPUT_ESCAPE)) {
			clsDx();
		}

		// 名前入力フィールドを更新・描画
		UpdateTextField(NameField);
		DrawTextField(NameField);
		DrawString(NameField.X - 150, NameField.Y, "プレイヤーの名前", GetColor(255, 255, 255));

		// 操作説明など
		DrawString(3, 3, "スコアランキングクライアント", GetColor(255, 255, 255));
		DrawString(3, 23, "[黒い部分をクリック]:名前入力, [Enter]:スコア送信, [Escape]:ログクリア", GetColor(255, 255, 255));
		DrawString(3, 43, "[Ctrl]+[O]:ブラウザで開く", GetColor(255, 255, 255));

		ScreenFlip();
	}

	DxLib_End();
	return 0;
}

// -------------------------------------------------------------------------