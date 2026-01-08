#pragma once
#include<graphics.h>
#include<vector>
#include<fstream>
#include<iostream>

#include<cstdlib>
#include<cstring>
#include<sstream>
using namespace std;

struct ChessPos {
	int row;
	int col;
	ChessPos(int r = 0, int c = 0) :row(r), col(c) {}
	//row被赋值为r，col被赋值为c
};

typedef enum {//白棋为-1，黑棋为1
	CHESS_WHITE = -1,
	CHESS_BLACK=1
}chess_kind_t;

class Chess
{
public:
	Chess(int gradeSize, int margin_x, int margin_y, float chessSize);
	void init();
	//棋盘的初始化如加载棋盘图片资源、初始化棋盘相关数据

	bool clickBoard(int x, int y, ChessPos* pos);
	// 判断(x,y)位置否是有效点击并把有效点击的位置的行列保存在参数pos中

	void chessDown(ChessPos* pos, chess_kind_t kind);//落子
	int getGradeSize();//获取棋盘大小

	int getChessData(ChessPos* pos);
	int getChessData(int row, int col);// 看指定位置是黑棋/白棋/空白

	bool checkOver();//是否对局结束

	vector<vector<int>> chessMap;
	//存储当期棋局的棋子分布数据 0-空白 1-黑子 -1-白子

	void saveGame(const std::string& filename); // 存档
	void loadGame(const std::string& filename); // 读档

	//禁手
	bool isLiveThreeHorizontal(int row, int col);
	bool isLiveThreeVertical(int row, int col);
	bool isLiveThreeDiagonal1(int row, int col);
	bool isLiveThreeDiagonal2(int row, int col);
	bool isDoubleThree(int row, int col);

	bool isLongLine(int row, int col);

	bool isLiveFourVertical(int row, int col);
	bool isLiveFourDiagonal1(int row, int col);
	bool isLiveFourDiagonal2(int row, int col);
	bool isLiveFourHorizontal(int row, int col);
	bool isDoubleFour(int row, int col);

private:
	IMAGE chessBlackImg;//黑旗棋子
	IMAGE chessWhiteImg;//白棋棋子

	int gradeSize;//棋盘大小（13,15,17,19）
	int margin_x;//棋盘左侧边界
	int margin_y;//棋盘顶部边界
	float chessSize;//棋子的大小

	//表示现在该谁下棋
	bool playerFlag;//true 该黑子走  false 该白字走

	void updateGameMap(ChessPos* pos);//更新棋盘状态

	bool checkWin();//如果胜负已分，返回true
	ChessPos lastPos;//最近落子位置
};