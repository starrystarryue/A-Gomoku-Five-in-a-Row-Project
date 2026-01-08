#include<iostream>
#include "Man.h"
#include"Chess.h"
#include"ChessGame.h"
#include<easyx.h>
#include<conio.h>
#include<graphics.h>
#include<fstream>
#pragma once

extern bool exitgame;//在游戏页面内，点击直接退出时
int playMode = 0;//1表示人机对战，2表示双人对战

void drawButton(int x, int y, int w, int h) {
    setfillcolor(WHITE); setlinecolor(BLACK);  
    fillroundrect(x, y, x + w, y + h, 20, 20); roundrect(x, y, x + w, y + h, 20, 20);   
}
void writeinstruction(int x, int y)
{
	setfillcolor(WHITE); setlinecolor(BLACK); fillcircle(40, 40, 22); circle(40, 40, 22);

	setbkmode(TRANSPARENT);settextstyle(30, 0, "微软雅黑 BOLD");settextcolor(BROWN);outtextxy(x, y, "?");
}
void writeexit(int x, int y)
{
	setbkmode(TRANSPARENT); settextstyle(35, 0, "微软雅黑");settextcolor(BROWN); outtextxy(x, y, "退出游戏");
}
void writeloadfile(int x, int y)
{
	setbkmode(TRANSPARENT); settextstyle(35, 0, "微软雅黑");settextcolor(BROWN); outtextxy(x, y, "双人模式");
}
void writenewgame(int x, int y)
{
	setbkmode(TRANSPARENT); settextstyle(35, 0, "微软雅黑");settextcolor(BROWN); outtextxy(x, y, "人机对战");
}
void writetitle (int x, int y)
{
	setbkmode(TRANSPARENT); settextstyle(88, 0, "微软雅黑 BOLD");settextcolor(BROWN); outtextxy(x, y, "五子棋对战");
}
void drawMenu()
{
	initgraph(750,550);
	loadimage(0,"resource/bk.jpg");

	writeinstruction(35, 25);
	writetitle(200, 100);
	drawButton(230, 250, 280, 50);writenewgame(315, 257);
	drawButton(230, 320, 280, 50);writeloadfile(315, 327);
	drawButton(230, 390, 280, 50);writeexit(315, 397);
}
void drawInstructionPage() {
	// 在当前页面上绘制说明书的内容
	setfillcolor(WHITE);
	setlinecolor(BLACK);

	// 绘制说明书的背景
	fillrectangle(50, 50, 700, 500); 
	settextcolor(BROWN);
	settextstyle(30, 0, _T("微软雅黑"));
	outtextxy(100, 100, "游戏说明：");
	outtextxy(100, 150, "欢迎来到人机对战之五子棋对局！");
	outtextxy(100, 200, "本游戏支持双人对战与人机对战模式");
	outtextxy(100, 250, "游戏支持存档、读档功能");
	outtextxy(100, 300, "注意：双人模式与人机模式分别各自存档读档");
	outtextxy(100, 350, "祝您游戏愉快！");

	setfillcolor(WHITE); setlinecolor(BLACK);fillcircle(660, 90, 22); circle(660, 90, 22); 
	setbkmode(TRANSPARENT); settextstyle(50, 0, "微软雅黑 BOLD");settextcolor(BROWN); outtextxy(647, 66, "X");
}


int main(void)
{
	drawMenu();
	bool gotogame = false;//进入游戏界面
	bool gotomenu = false;//回到菜单页面
	while (true) {
		MOUSEMSG mouse;
		mouse = GetMouseMsg();
		int x0 = mouse.x, y0 = mouse.y;
		
		if (mouse.uMsg == WM_LBUTTONDOWN) {//点击按钮
			//打开说明书
			if (x0 >= 18 && x0 <= 62 && y0 >= 18 && y0 <= 62) {
				mciSendString("play resource/select.mp3", 0, 0, 0);
				drawInstructionPage();
			}
			//从说明书返回主菜单	
			if (x0 >= 638 && x0 <= 682 && y0 >= 68 && y0 <= 112) {
				mciSendString("play resource/select.mp3", 0, 0, 0);
				gotomenu = true;
				closegraph();
				drawMenu();
				continue;
			}
			//进入人机游戏
			if (x0 >= 230 && x0 <= 510 && y0 >= 250 && y0 <= 300) {
				mciSendString("play resource/select.mp3", 0, 0, 0);
				playMode = 1;
				gotogame = true;
				break;
			}
			//退出游戏
			if (x0 >= 230 && x0 <= 510 && y0 >= 390 && y0 <= 440) break;
			//进入双人游戏
			if (x0 >= 230 && x0 <= 510 && y0 >= 320 && y0 <= 370) {
				mciSendString("play resource/select.mp3", 0, 0, 0);
				playMode = 2;
				gotogame = true;
				break;
			}
		}
	}
	
	if (gotogame)
	{
		if (playMode == 1) {
			Man man;
			Chess chess(15, 30 + 25, 27 + 25, 31.71);

			AI ai;
			ChessGame game(&man, &ai, &chess);

			game.play();
			return 0;
		}
		else if (playMode == 2) {
			Man man;
			Chess chess(15, 30 + 25, 27 + 25, 31.71);
			AI ai;
			ChessGame game(&man, &ai, &chess);

			game.play();
			return 0;
		}
	}

	closegraph();
	return 0;
}