#include "DxLib.h"

// �ȈՃX�R�A�T�[�o�N���C�A���g --------------------------------------------
//
//   �@�ȈՃX�R�A�T�[�o�ɃX�R�A�ƃv���C���[���𑗐M����v���O�����ł��B
//   ���{����͂�}�E�X���͂ȂǁA�܂��g�����������Ă��Ȃ��֐��𐔑���
//   �g���Ă���̂ŁA�K�X DX ���C�u�����̃��t�@�����X���Q�Ƃ��Ă��������B
//   (DX���C�u���� �֐��ꗗ http://dxlib.o.oo7.jp/dxfunc.html)
//
// ver 1.0 (2017/ 6/21) ----------------------------------------------------



// �ݒ� --------------------------------------------------------------------

// �ȈՃX�R�A�T�[�o�̔z�u�ꏊ
const char ScoreServerUrl[] = "http://qpic.jp/contents/43/ScoreServer.php?Format=CSV";

// -------------------------------------------------------------------------



// ��������̓t�B�[���h�̎��� ----------------------------------------------

// ��������̓t�B�[���h
struct TextField {
	int X, Y, Width, Height;

	int InputHandle;
	int BackColor, ForeColor;
};

// 1�t���[���O�Ƀ}�E�X�̍��{�^����������Ă������ǂ���
bool MouseLeftPressedInPreviousFlame = false;

// ��������̓t�B�[���h���ҏW�����ǂ���
bool TextFieldIsEditing(TextField Field) {
	if (CheckKeyInput(Field.InputHandle) != 0) {
		return false;
	} else {
		return true;
	}
}

// ��������̓t�B�[���h�̓��͂��ꂽ��������擾���܂�
void TextFieldGetText(TextField Field, char Buffer[]) {
	GetKeyInputString(Buffer, Field.InputHandle);
}

// ��������̓t�B�[���h���X�V���܂�
void UpdateTextField(TextField Field) {
	if (GetMouseInput() & MOUSE_INPUT_LEFT) {
		if (MouseLeftPressedInPreviousFlame == false) {
			int MouseX, MouseY;
			GetMousePoint(&MouseX, &MouseY);

			// ��������̓t�B�[���h�̒��ō��N���b�N���ꂽ����͂ł����Ԃ�
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

// ��������̓t�B�[���h��`�悵�܂�
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



// �X�R�A���M���� ----------------------------------------------------------

// �X�R�A�𑗐M���܂��B
// �߂�l: 0 = ����, -1 = ���s
int SendScore(char PlayerName[], int Score) {
	IPDATA IP = { 127, 0, 0, 1 };

	if (GetHostIPbyName("qpic.jp", &IP) == -1) {
		return -1;
	}

	int NetHandle = ConnectNetWork(IP, 80);

	if (NetHandle == -1) {
		return -1;
	}

	// Http ���N�G�X�g�̍쐬
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

	// Http ���N�G�X�g�𑗐M
	NetWorkSend(NetHandle, SendBuffer, strlen(SendBuffer));

	// �����������̊֐����ĂԂƋ����I�Ƀf�[�^�𑗐M����B
	WaitTimer(0);

	int TotalReceivedLength = 0, Length = 0;

	// ���M�������N�G�X�g�ɑ΂��� Http ���X�|���X�̎擾
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



// ���C���֐� --------------------------------------------------------------

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
	// ���{����͊֐� (MakeKeyInput �Ȃ�)
	// http://dxlib.o.oo7.jp/function/dxfunc_input.html#R5N13
	NameField.InputHandle = MakeKeyInput(12, FALSE, FALSE, FALSE);
	NameField.ForeColor = GetColor(255, 255, 255);
	NameField.BackColor = GetColor(0, 0, 0);

	bool EnterPressedInPreviousFlame = false;
	bool OPressedInPreviousFlame = false;

	while (ProcessMessage() != -1) {
		ClearDrawScreen();

		// [Ctrl]+[O] �������ꂽ��u���E�U���J��
		if (CheckHitKey(KEY_INPUT_O)) {
			if (OPressedInPreviousFlame == false && (CheckHitKey(KEY_INPUT_LCONTROL) || CheckHitKey(KEY_INPUT_RCONTROL))) {
				// ShellExecute �֐��̎g����
				// http://ohwhsmm7.blog28.fc2.com/blog-entry-105.html
				ShellExecute(NULL, "Open", ScoreServerUrl, NULL, NULL, SW_SHOWDEFAULT);
			}

			OPressedInPreviousFlame = true;
		} else {
			OPressedInPreviousFlame = false;
		}

		// [Enter] �������ꂽ��f�[�^���M
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

		// [Escape] �������ꂽ�烍�O���N���A
		if (CheckHitKey(KEY_INPUT_ESCAPE)) {
			clsDx();
		}

		// ���O���̓t�B�[���h���X�V�E�`��
		UpdateTextField(NameField);
		DrawTextField(NameField);
		DrawString(NameField.X - 150, NameField.Y, "�v���C���[�̖��O", GetColor(255, 255, 255));

		// ��������Ȃ�
		DrawString(3, 3, "�X�R�A�����L���O�N���C�A���g", GetColor(255, 255, 255));
		DrawString(3, 23, "[�����������N���b�N]:���O����, [Enter]:�X�R�A���M, [Escape]:���O�N���A", GetColor(255, 255, 255));
		DrawString(3, 43, "[Ctrl]+[O]:�u���E�U�ŊJ��", GetColor(255, 255, 255));

		ScreenFlip();
	}

	DxLib_End();
	return 0;
}

// -------------------------------------------------------------------------