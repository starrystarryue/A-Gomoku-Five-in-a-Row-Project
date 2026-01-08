#pragma once
#include "Chess.h"
#include<vector>

class AI
{
public:
	void init(Chess* chess);
	void go();

private:
	Chess* chess;
	vector<vector<int>> scoreMap;

private:
	void calculateScore();//评分函数
	ChessPos think();//评分确定后，“思考”落子位置
};

