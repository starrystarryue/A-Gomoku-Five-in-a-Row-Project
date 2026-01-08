#include "Chess.h"
#include<graphics.h>
#include<easyx.h>
#include<cmath>
#include <conio.h>
#include <mmsystem.h>
#include<iostream>
#pragma comment(lib, "winmm.lib")

bool valid = true;//是否禁手
extern int playMode;//指示双人or人机
extern chess_kind_t chessflag;

//操作按钮
void drawButton(float x, float y, int w, int h) {
	// 设置按钮的外观
	setfillcolor(WHITE); // 设置按钮的填充颜色（蓝色）
	setlinecolor(BLACK);  // 设置按钮边框颜色（深蓝色）

	// 绘制圆角矩形按钮
	fillroundrect(x, y, x + w, y + h, 20, 20); // 设置圆角半径为20
	roundrect(x, y, x + w, y + h, 20, 20);    // 绘制边框
}

//透明化棋子的背景
void putimagePNG(int x, int y, IMAGE* picture) //x为载入图片的X坐标，y为Y坐标
{
	// 变量初始化
	DWORD* dst = GetImageBuffer();    // GetImageBuffer()函数，用于获取绘图设备的显存指针，EASYX自带
	DWORD* draw = GetImageBuffer();
	DWORD* src = GetImageBuffer(picture); //获取picture的显存指针
	int picture_width = picture->getwidth(); //获取picture的宽度，EASYX自带
	int picture_height = picture->getheight(); //获取picture的高度，EASYX自带
	int graphWidth = getwidth();       //获取绘图区的宽度，EASYX自带
	int graphHeight = getheight();     //获取绘图区的高度，EASYX自带
	int dstX = 0;    //在显存里像素的角标

	// 实现透明贴图 公式： Cp=αp*FP+(1-αp)*BP ， 贝叶斯定理来进行点颜色的概率计算
	for (int iy = 0; iy < picture_height; iy++)
	{
		for (int ix = 0; ix < picture_width; ix++)
		{
			int srcX = ix + iy * picture_width; //在显存里像素的角标
			int sa = ((src[srcX] & 0xff000000) >> 24); //0xAArrggbb;AA是透明度
			int sr = ((src[srcX] & 0xff0000) >> 16); //获取RGB里的R
			int sg = ((src[srcX] & 0xff00) >> 8);   //G
			int sb = src[srcX] & 0xff;              //B
			if (ix >= 0 && ix <= graphWidth && iy >= 0 && iy <= graphHeight && dstX <= graphWidth * graphHeight)
			{
				dstX = (ix + x) + (iy + y) * graphWidth; //在显存里像素的角标
				int dr = ((dst[dstX] & 0xff0000) >> 16);
				int dg = ((dst[dstX] & 0xff00) >> 8);
				int db = dst[dstX] & 0xff;
				draw[dstX] = ((sr * sa / 255 + dr * (255 - sa) / 255) << 16)  //公式： Cp=αp*FP+(1-αp)*BP  ； αp=sa/255 , FP=sr , BP=dr
					| ((sg * sa / 255 + dg * (255 - sa) / 255) << 8)         //αp=sa/255 , FP=sg , BP=dg
					| (sb * sa / 255 + db * (255 - sa) / 255);              //αp=sa/255 , FP=sb , BP=db
			}
		}
	}
}

Chess::Chess(int gradeSize, int marginX, int marginY, float chessSize)
{
	this ->gradeSize = gradeSize;
	this->margin_x = marginX;
	this->margin_y = marginY;
	this->chessSize = chessSize;
	playerFlag = CHESS_BLACK;

	for (int i = 0; i < gradeSize; i++) {
		vector<int> row;
		for (int j = 0; j < gradeSize; j++) {
			row.push_back(0);
		}
		chessMap.push_back(row);
	}
}

//初始化游戏内页面
void Chess::init()
{	
	//创建游戏窗口
	initgraph(750, 550,EW_SHOWCONSOLE);
	loadimage(0, "resource/bk.jpg");
	IMAGE board;
	loadimage(&board,"resource/棋盘 500.jpg");
	putimage(25, 25, &board);
	setbkmode(TRANSPARENT); settextstyle(30, 0, "微软雅黑 BOLD"); settextcolor(BROWN);

	drawButton(557.5, 40, 160, 50); drawButton(557.5, 115, 160, 50); 
	drawButton(557.5, 190, 160, 50); drawButton(557.5, 265, 160, 50); drawButton(557.5, 340, 160, 50);
	outtextxy(617,50,"暂停"); outtextxy(617, 125, "继续");
	outtextxy(617, 200, "存档"); outtextxy(595, 275, "读档继续"); outtextxy(595, 350, "直接退出");
	drawButton(557.5, 430, 160, 80);
	mciSendString("play resource/start.wav", 0, 0, 0); 
	mciSendString("play resource/music.mp3", 0, 0, 0);

	//加载黑白棋子的图片
	loadimage(&chessBlackImg, "resource/黑棋.png", chessSize, chessSize, true);
	loadimage(&chessWhiteImg, "resource/白棋.png", chessSize, chessSize, true);

	//棋盘清零
	for (int i = 0; i <gradeSize; i++) {
		for (int j = 0; j < gradeSize; j++) {
			chessMap[i][j] = 0;
		}
	}
	playerFlag = true;//黑旗先走
}

//点击棋盘
bool Chess::clickBoard(int x, int y, ChessPos* pos)
{
	if (x > 525 || y > 525)return false;

	int col = (x - margin_x) / chessSize;
	int row = (y - margin_y) / chessSize;
	int leftTopPosX = margin_x + chessSize * col;
	int leftTopPosY = margin_y + chessSize * row;
	int delta = chessSize * 0.4;//点击位置距离棋盘交点处的距离

	int len;
	bool ret = false;//表示是否为有效点击

	do {
		//左上角判断
		len = sqrt((x - leftTopPosX) * (x - leftTopPosX) + (y - leftTopPosY) * (y - leftTopPosY));
		if (len <= delta) {
			pos->row = row;
			pos->col = col;
			if (chessMap[pos->row][pos->col] == 0) {
				ret = true;
			}	
			break;
		}

		//右上角判断
		int x2 = leftTopPosX + chessSize;
		int y2 = leftTopPosY;
		len = sqrt((x - x2) * (x - x2) + (y - y2) * (y - y2));
		if (len <= delta) {
			pos->row = row;
			pos->col = col+1;
			if (chessMap[pos->row][pos->col] == 0) {
				ret = true;
			}
			break;
		}

		//左下角判断
		int x3 = leftTopPosX;
		int y3 = leftTopPosY + chessSize;
		len = sqrt((x - x3) * (x - x3) + (y - y3)* (y - y3));
		if (len <= delta) {
			pos->row = row+1;
			pos->col = col;
			if (chessMap[pos->row][pos->col] == 0) {
				ret = true;
			}
			break;
		}
		
		//右下角判断
		int x4 = leftTopPosX+chessSize;
		int y4 = leftTopPosY + chessSize;
		len = sqrt((x - x4) * (x - x4) + (y - y4) * (y - y4));
		if (len <= delta) {
			pos->row = row + 1;
			pos->col = col+1;
			if (chessMap[pos->row][pos->col] == 0) {
				ret = true;
			}
			break;
		}

	} while (0);
	
	return ret;
}

//落子
void Chess::chessDown(ChessPos* pos, chess_kind_t kind)
{
	if (chessflag == CHESS_BLACK) {
		if (isDoubleThree(pos->row, pos->col)) {// 三三禁手
			cout << "三三禁手，不能落子" << endl;
			mciSendString("play resource/无效点击.mp3", 0, 0, 0);
			valid = false;
			return; 
		}
		if (isLongLine(pos->row, pos->col)) {//长连禁手
			cout << "长连禁手，不能落子" << endl;
			mciSendString("play resource/无效点击.mp3", 0, 0, 0);
			valid = false;
			return;
		}
		if (isDoubleFour(pos->row, pos->col)) {//四四禁手
			cout << "四四禁手，不能落子" << endl;
			mciSendString("play resource/无效点击.mp3", 0, 0, 0);
			valid = false;
			return;
		}
	}

	valid = true;
	mciSendString("play resource/down7.WAV", 0, 0, 0);

	int x = margin_x + chessSize * pos->col - 0.5 * chessSize;
	int y = margin_y + chessSize * pos->row - 0.5 * chessSize;

	if (kind == CHESS_WHITE) {
		putimagePNG(x, y, &chessWhiteImg);
	}
	else {
		putimagePNG(x, y, &chessBlackImg);
	}
	updateGameMap(pos);
}

int Chess::getGradeSize()
{
	return gradeSize;
}
int Chess::getChessData(ChessPos* pos)
{
	return chessMap[pos->row][pos->col];
}
int Chess::getChessData(int row, int col)
{
	return chessMap[row][col];
}

//检查胜利条件
bool Chess::checkWin()
{
	// 水平方向
	int row = lastPos.row;
	int col = lastPos.col;

	for (int i = 0; i < 5; i++)
	{
		// 往左5个，往右匹配4个子，20种情况
		if (col - i >= 0 &&
			col - i + 4 < gradeSize &&
			chessMap[row][col - i] == chessMap[row][col - i + 1] &&
			chessMap[row][col - i] == chessMap[row][col - i + 2] &&
			chessMap[row][col - i] == chessMap[row][col - i + 3] &&
			chessMap[row][col - i] == chessMap[row][col - i + 4])
			return true;
	}

	// 竖直方向(上下延伸4个)
	for (int i = 0; i < 5; i++)
	{
		if (row - i >= 0 &&
			row - i + 4 < gradeSize &&
			chessMap[row - i][col] == chessMap[row - i + 1][col] &&
			chessMap[row - i][col] == chessMap[row - i + 2][col] &&
			chessMap[row - i][col] == chessMap[row - i + 3][col] &&
			chessMap[row - i][col] == chessMap[row - i + 4][col])
			return true;
	}

	// “/"方向
	for (int i = 0; i < 5; i++)
	{
		if (row + i < gradeSize &&
			row + i - 4 >= 0 &&
			col - i >= 0 &&
			col - i + 4 < gradeSize &&
			// 第[row+i]行，第[col-i]的棋子，与右上方连续4个棋子都相同
			chessMap[row + i][col - i] == chessMap[row + i - 1][col - i + 1] &&
			chessMap[row + i][col - i] == chessMap[row + i - 2][col - i + 2] &&
			chessMap[row + i][col - i] == chessMap[row + i - 3][col - i + 3] &&
			chessMap[row + i][col - i] == chessMap[row + i - 4][col - i + 4])
			return true;
	}

	// “\“ 方向
	for (int i = 0; i < 5; i++)
	{
		// 第[row+i]行，第[col-i]的棋子，与右下方连续4个棋子都相同
		if (row - i >= 0 &&
			row - i + 4 < gradeSize &&
			col - i >= 0 &&
			col - i + 4 < gradeSize &&
			chessMap[row - i][col - i] == chessMap[row - i + 1][col - i + 1] &&
			chessMap[row - i][col - i] == chessMap[row - i + 2][col - i + 2] &&
			chessMap[row - i][col - i] == chessMap[row - i + 3][col - i + 3] &&
			chessMap[row - i][col - i] == chessMap[row - i + 4][col - i + 4])
			return true;
	}
	return false;
}

//检查对局结束并显示结束信息
bool Chess::checkOver()
{
	if (checkWin()) {
		Sleep(150);
		//棋盘显示提示 对局结束
		setbkmode(TRANSPARENT);settextstyle(80, 0, "仿宋 BOLD");
		settextcolor(YELLOW);outtextxy(85, 200, "对局结束");

		if (playMode == 1)//人机对战，人走黑棋
		{
			
			if (playerFlag == false) {  //黑棋赢（玩家赢）,此时标记已经反转，轮到白棋落子
				mciSendString("play resource/胜利.mp3", 0, 0, 0);
				setbkmode(TRANSPARENT);
				settextstyle(35, 0,"仿宋 BOLD");
				settextcolor(RED);
				outtextxy(570, 450, "恭喜胜利");
				//loadimage(0, "resource/胜利 850 600.png");
			}
			else {
				mciSendString("play resource/失败.mp3", 0, 0, 0);
				setbkmode(TRANSPARENT);
				settextstyle(35, 0, "仿宋 BOLD");
				settextcolor(RED);
				outtextxy(570, 450, "遗憾落败");
				//loadimage(0, "resource/失败 850.png");
			}
		}
		else if (playMode == 2) {//双人模式，判断黑白方玩家
			setbkmode(TRANSPARENT);
			settextstyle(35, 0, "仿宋 BOLD");
			settextcolor(RED);
			mciSendString("play resource/胜利.mp3", 0, 0, 0);
			if (chessflag == CHESS_BLACK) {
				outtextxy(570, 450, "白棋胜利");
			}
			else if (chessflag == CHESS_WHITE) {
				outtextxy(570, 450, "黑棋胜利");
			}
		}

		_getch(); 
		return true;
	}
	return false;
}

//交换
void Chess::updateGameMap(ChessPos* pos)
{
	chessMap[pos->row][pos->col] = playerFlag ? CHESS_BLACK : CHESS_WHITE;
	playerFlag = !playerFlag;//黑白方交换进行落子
	lastPos = *pos;
}

void Chess::saveGame(const std::string& filename) {
	std::ofstream outFile(filename);
	if (outFile.is_open()) {
		for (int i = 0; i < gradeSize; i++) {
			for (int j = 0; j < gradeSize; j++) {
				outFile << chessMap[i][j];
				if (j < gradeSize - 1) outFile << " "; 
			}
			outFile << std::endl;
		}
		outFile.close();
		std::cout << "Game saved successfully!存档成功！" << std::endl;
	}
	else {
		std::cerr << "Failed to save game!读档失败" << std::endl;
	}
}

void Chess::loadGame(const std::string& filename) {
	std::ifstream inFile(filename);
	if (inFile.is_open()) {
		std::string line;
		for (int i = 0; i < gradeSize; i++) {
			std::getline(inFile, line);
			std::stringstream ss(line);
			std::string cell;
			for (int j = 0; j < gradeSize; j++) {
				std::getline(ss, cell, ' ');
				chessMap[i][j] = std::stoi(cell); // 将字符串转换为整数
			}
		}
		inFile.close();
		std::cout << "Game loaded successfully!读档成功！" << std::endl;


		// 更新 UI 显示当前棋局
		for (int i = 0; i < 15; i++) {
			for (int j = 0; j < 15; j++) {
				if (chessMap[i][j] == CHESS_BLACK) {
					putimagePNG(margin_x + j * chessSize - 0.5 * chessSize, 
						margin_y + i * chessSize - 0.5 * chessSize, &chessBlackImg); // 绘制黑子
				}
				else if (chessMap[i][j] == CHESS_WHITE) {
					putimagePNG(margin_x + j * chessSize - 0.5 * chessSize, 
						margin_y + i * chessSize - 0.5 * chessSize, &chessWhiteImg); // 绘制白子
				}
			}
		}

	}
	else {
		std::cerr << "Failed to load game!" << std::endl;
	}
}

//禁手判断
// 判断水平方向是否为活三
bool Chess::isLiveThreeHorizontal(int row, int col) {
	int count = 0;
	int emptyEnds = 0;
	// 向左检查
	for (int i = col - 1; i >= 0; --i) {
		if (chessMap[row][i] == CHESS_BLACK) {
			++count;
		}
		else if (chessMap[row][i] == 0) {
			emptyEnds++;
			break;
		}
		else {
			break;
		}
	}
	// 向右检查
	for (int i = col + 1; i < gradeSize; ++i) {
		if (chessMap[row][i] == CHESS_BLACK) {
			++count;
		}
		else if (chessMap[row][i] == 0) {
			emptyEnds++;
			break;
		}
		else {
			break;
		}
	}
	return count == 2 && emptyEnds == 2;
}
// 判断垂直方向是否为活三
bool Chess::isLiveThreeVertical(int row, int col) {
	int count = 0;
	int emptyEnds = 0;
	// 向上检查
	for (int i = row - 1; i >= 0; --i) {
		if (chessMap[i][col] == CHESS_BLACK) {
			++count;
		}
		else if (chessMap[i][col] == 0) {
			emptyEnds++;
			break;
		}
		else {
			break;
		}
	}
	// 向下检查
	for (int i = row + 1; i < gradeSize; ++i) {
		if (chessMap[i][col] == CHESS_BLACK) {
			++count;
		}
		else if (chessMap[i][col] == 0) {
			emptyEnds++;
			break;
		}
		else {
			break;
		}
	}
	return count == 2 && emptyEnds == 2;
}
// 判断斜向（从左上角到右下角）是否为活三
bool Chess::isLiveThreeDiagonal1(int row, int col) {
	int count = 0;
	int emptyEnds = 0;
	// 向左上检查
	for (int i = 1; row - i >= 0 && col - i >= 0; ++i) {
		if (chessMap[row - i][col - i] == CHESS_BLACK) {
			++count;
		}
		else if (chessMap[row - i][col - i] == 0) {
			emptyEnds++;
			break;
		}
		else {
			break;
		}
	}
	// 向右下检查
	for (int i = 1; row + i < gradeSize && col + i < gradeSize; ++i) {
		if (chessMap[row + i][col + i] == CHESS_BLACK) {
			++count;
		}
		else if (chessMap[row + i][col + i] == 0) {
			emptyEnds++;
			break;
		}
		else {
			break;
		}
	}
	return count == 2 && emptyEnds == 2;
}
// 判断斜向（从右上角到左下角）是否为活三
bool Chess::isLiveThreeDiagonal2(int row, int col) {
	int count = 0;
	int emptyEnds = 0;
	// 向左下检查
	for (int i = 1; row + i < gradeSize && col - i >= 0; ++i) {
		if (chessMap[row + i][col - i] == CHESS_BLACK) {
			++count;
		}
		else if (chessMap[row + i][col - i] == 0) {
			emptyEnds++;
			break;
		}
		else {
			break;
		}
	}
	// 向右上检查
	for (int i = 1; row - i >= 0 && col + i < gradeSize; ++i) {
		if (chessMap[row - i][col + i] == CHESS_BLACK) {
			++count;
		}
		else if (chessMap[row - i][col + i] == 0) {
			emptyEnds++;
			break;
		}
		else {
			break;
		}
	}
	return count == 2 && emptyEnds == 2;
}
// 判断是否为三三禁手
bool Chess::isDoubleThree(int row, int col) {
	int liveThreeCount = 0;
	if (isLiveThreeHorizontal(row, col)) ++liveThreeCount;
	if (isLiveThreeVertical(row, col)) ++liveThreeCount;
	if (isLiveThreeDiagonal1(row, col)) ++liveThreeCount;
	if (isLiveThreeDiagonal2(row, col)) ++liveThreeCount;
	return liveThreeCount >= 2;
}

// 判断是否为长连禁手
bool Chess::isLongLine(int row, int col) {
	int count = 1;
	// 水平方向检查
	for (int i = col + 1; i < gradeSize; ++i) {
		if (chessMap[row][i] == CHESS_BLACK) {
			++count;
		}
		else {
			break;
		}
	}
	for (int i = col - 1; i >= 0; --i) {
		if (chessMap[row][i] == CHESS_BLACK) {
			++count;
		}
		else {
			break;
		}
	}
	if (count >= 6) return true;
	count = 1;
	// 垂直方向检查
	for (int i = row + 1; i < gradeSize; ++i) {
		if (chessMap[i][col] == CHESS_BLACK) {
			++count;
		}
		else {
			break;
		}
	}
	for (int i = row - 1; i >= 0; --i) {
		if (chessMap[i][col] == CHESS_BLACK) {
			++count;
		}
		else {
			break;
		}
	}
	if (count >= 6) return true;
	count = 1;
	// 斜向（从左上角到右下角）检查
	for (int i = 1; row + i < gradeSize && col + i < gradeSize; ++i) {
		if (chessMap[row + i][col + i] == CHESS_BLACK) {
			++count;
		}
		else {
			break;
		}
	}
	for (int i = 1; row - i >= 0 && col - i >= 0; ++i) {
		if (chessMap[row - i][col - i] == CHESS_BLACK) {
			++count;
		}
		else {
			break;
		}
	}
	if (count >= 6) return true;
	count = 1;
	// 斜向（从右上角到左下角）检查
	for (int i = 1; row + i < gradeSize && col - i >= 0; ++i) {
		if (chessMap[row + i][col - i] == CHESS_BLACK) {
			++count;
		}
		else {
			break;
		}
	}
	for (int i = 1; row - i >= 0 && col + i < gradeSize; ++i) {
		if (chessMap[row - i][col + i] == CHESS_BLACK) {
			++count;
		}
		else {
			break;
		}
	}
	return count >= 6;
}

// 判断水平方向是否为活四
bool Chess::isLiveFourHorizontal(int row, int col) {
    int count = 0;
    int emptyEnds = 0;
    // 向左检查
    for (int i = col - 1; i >= 0; --i) {
        if (chessMap[row][i] == CHESS_BLACK) {
            ++count;
        } else if (chessMap[row][i] == 0) {
			++emptyEnds;
            break;
        } else {
            break;
        }
    }
    // 向右检查
    for (int i = col + 1; i < gradeSize; ++i) {
        if (chessMap[row][i] == CHESS_BLACK) {
            ++count;
        } else if (chessMap[row][i] == 0) {
			++emptyEnds;
            break;
        } else {
            break;
        }
    }
    return count == 3 && emptyEnds == 2;
}
// 判断垂直方向是否为活四
bool Chess::isLiveFourVertical(int row, int col) {
    int count = 0;
    int emptyEnds = 0;
    // 向上检查
    for (int i = row - 1; i >= 0; --i) {
        if (chessMap[i][col] == CHESS_BLACK) {
            ++count;
        } else if (chessMap[i][col] == 0) {
			++emptyEnds;
            break;
        } else {
            break;
        }
    }
    // 向下检查
    for (int i = row + 1; i < gradeSize; ++i) {
        if (chessMap[i][col] == CHESS_BLACK) {
            ++count;
        } else if (chessMap[i][col] == 0) {
			++emptyEnds;
            break;
        } else {
            break;
        }
    }
    return count == 3 && emptyEnds == 2;
}
// 判断斜向（从左上角到右下角）是否为活四
bool Chess::isLiveFourDiagonal1(int row, int col) {
    int count = 0;
    int emptyEnds = 0;
    // 向左上检查
    for (int i = 1; row - i >= 0 && col - i >= 0; ++i) {
        if (chessMap[row - i][col - i] == CHESS_BLACK) {
            ++count;
        } else if (chessMap[row - i][col - i] == 0) {
			++emptyEnds;
            break;
        } else {
            break;
        }
    }
    // 向右下检查
    for (int i = 1; row + i < gradeSize && col + i < gradeSize; ++i) {
        if (chessMap[row + i][col + i] == CHESS_BLACK) {
            ++count;
        } else if (chessMap[row + i][col + i] == 0) {
			++emptyEnds;
            break;
        } else {
            break;
        }
    }
    return count == 3 && emptyEnds == 2;
}
// 判断斜向（从右上角到左下角）是否为活四
bool Chess::isLiveFourDiagonal2(int row, int col) {
    int count = 0;
    int emptyEnds = 0;
    // 向左下检查
    for (int i = 1; row + i < gradeSize && col - i >= 0; ++i) {
        if (chessMap[row + i][col - i] == CHESS_BLACK) {
            ++count;
        } else if (chessMap[row + i][col - i] == 0) {
			++emptyEnds;
            break;
        } else {
            break;
        }
    }
    // 向右上检查
    for (int i = 1; row - i >= 0 && col + i < gradeSize; ++i) {
        if (chessMap[row - i][col + i] == CHESS_BLACK) {
            ++count;
        } else if (chessMap[row - i][col + i] == 0) {
			++emptyEnds;
            break;
        } else {
            break;
        }
    }
    return count == 3 && emptyEnds == 2;
}
// 判断是否为四四禁手
bool Chess::isDoubleFour(int row, int col) {
    int liveFourCount = 0;
    if (isLiveFourHorizontal(row, col)) ++liveFourCount;
    if (isLiveFourVertical(row, col)) ++liveFourCount;
    if (isLiveFourDiagonal1(row, col)) ++liveFourCount;
    if (isLiveFourDiagonal2(row, col)) ++liveFourCount;
    return (liveFourCount >= 2);
}