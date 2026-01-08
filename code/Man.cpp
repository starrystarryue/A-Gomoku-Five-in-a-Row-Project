#include "Man.h"
#include"Chess.h"

#include<iostream>
#include<fstream>
#include<vector>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

extern bool valid;
extern bool ispaused;
extern int playMode;
chess_kind_t chessflag = CHESS_BLACK;
bool exitgame = false;

void Man::init(Chess* chess)
{
	this->chess = chess;
}

void Man::go()
{
	MOUSEMSG msg;
	ChessPos pos;

	while (1) {
		msg = GetMouseMsg();
		if (msg.uMsg == WM_LBUTTONDOWN ) {
			int mx = msg.x, my = msg.y;

			//点击棋盘落子
			if (chess->clickBoard(mx, my, &pos) ) {
				if (ispaused) {
					mciSendString("play resource/无效点击.mp3", 0, 0, 0);
					cout << "游戏已暂停！请点击“继续”按钮继续游戏。" << endl;
				}
				break;
			}
			//直接退出
			if (mx >= 557 && mx <= 718 && my <= 390 && my >= 340) {
				mciSendString("play resource/select.mp3", 0, 0, 0);
				exitgame = true;
				return;
			}
			//暂停
			if (mx >= 557 && mx <= 718 && my >= 40 && my <= 90) {
				mciSendString("play resource/select.mp3", 0, 0, 0);
				cout << "游戏暂停！" << endl;
				ispaused = true;
			}
			//继续
			if (mx >= 557 && mx <= 718 && my >= 115 && my <= 165) {
				ispaused = false;
				mciSendString("play resource/select.mp3", 0, 0, 0);
				cout << "欢迎回到游戏，现在请继续对局吧！" << endl;
			}
			//存档
			if (mx >= 557 && mx <= 718 && my >= 190 && my <= 240) {
				mciSendString("play resource/select.mp3", 0, 0, 0);
				if (playMode == 1) {
					chess->saveGame("resource/game_save.txt");
				}
				else if (playMode == 2) {
					chess->saveGame("resource/game_save_pvp.txt");
				}
			}
			//读档
			if (mx >= 557 && mx <= 718 && my >= 265 && my <= 315) {
				mciSendString("play resource/select.mp3", 0, 0, 0);
				if (playMode == 1) {
					chess->loadGame("resource/game_save.txt");
				}
				else if (playMode == 2) {
					chess->loadGame("resource/game_save_pvp.txt");
				}
			}
		}
	}
	//未“暂停”，正常落子
	if (!ispaused) {
		if(playMode==1)chess->chessDown(&pos, CHESS_BLACK);
		else if (playMode == 2) {
			chess->chessDown(&pos, chessflag);
			if (valid) {
				chessflag = (chessflag == CHESS_BLACK ? CHESS_WHITE : CHESS_BLACK);
			}
		}
	}
}