#include "ChessGame.h"
#include<conio.h>
extern bool exitgame;
extern int playMode;
bool ispaused = false;

//为了便于调用各个类的功能，在ChessGame中添加3个数据成员，并构造函数中初始化这三个数据成员
ChessGame::ChessGame(Man* man, AI* ai, Chess* chess)
{
	this->man = man;
	this->ai = ai;
	this->chess = chess;

	man->init(chess);
	ai->init(chess);
}
//对局：开始五子棋
void ChessGame::play()
{
	chess->init();

	while (1) {
		if (exitgame) {
			return;
			break;
		}
		//人走
		man->go();
		if (chess->checkOver()) {
			chess->init();
			continue;
		}

		if (playMode == 1) {
			//人机对战，接下来由ai走
			ai->go();
			if (chess->checkOver()) {
				chess->init();
				continue;
			}
		}
		else if (playMode == 2) {
			//双人对战，依然人走
			man->go();
			if (chess->checkOver()) {
				chess->init();
				continue;
			}
		}
	}
}

