#include <string>
#include <windows.h>
#include "game_func.h"
#include "main.h"
#include "game_elements.h"
#include "GL/glut.h"



GameMap gMap;	//地图
Food food;		//食物
Pacman pacman({ 7,10 });
Ghost ghosts[4] = {		//幽灵
	Ghost({ 13,1 }, { 1,0,0 }),		//red
	Ghost({4,6},{0,1,1}),			//blue
	Ghost({10,8},{1.0,0.6,0}),		//orange
	Ghost({2,13},{1.,0,0.6})		//pink
};
GameConsole gameC;		//游戏控制台
time_t timer;		//计时器


//显示函数
void displayFunc()
{
	glClear(GL_COLOR_BUFFER_BIT);	//清除缓存
	gameC.drawFPS();	//绘制FPS
	//游戏进行中
	if (gameC.gameActive)
	{
		glEnable(GL_POINT_SMOOTH);		//图像抗齿距
		gMap.draw();		//绘制地图
		food.draw();		//绘制食物
		pacman.draw();		//绘制吃豆人
		for (int i = 0; i < Ghost::cnt; i++)		//绘制幽灵
			ghosts[i].draw();

		gameC.drawScoreBoard(pacman);	//绘制计分板

		//游戏未结束
		if(gameC.gameOver)	//游戏结束
			gameC.drawGameOver();	//绘制游戏结束
	}
	else	//游戏未进行
		gameC.drawWelcomeScreen();		//绘制开始窗口
	glutSwapBuffers();
}


//周期调用函数
void timerFunc(int value)
{
	if (gameC.gameActive)
	{
		if (!gameC.gameOver)
		{
			if (food.foodCnt() == 0)	//食物吃光
			{
				Sleep(1500);
				elementsReset(false);	//重置参数
				gameC.upgrade(ghosts);	//升级
			}
			pacman.eatFood(food, gameC);	//吃豆人吃食物
			pacman.move(gMap);		//吃豆人移动
			pacman.meetGhost(ghosts);		//吃豆人与幽灵碰撞
			for (int i = 0; i < Ghost::cnt; i++)		//幽灵移动
				ghosts[i].autoMove(gMap);

			//游戏结束设置
			if (gameC.checkGameOver(pacman))
			{
				gameC.gameOver = true;
				timer = time(nullptr);	//更新及时器
			}
		}
		else if (time(nullptr) - timer >= 2)		//2s后重置游戏信息
		{
			gameC.saveHiScore();	//存储最高分
			//重置参数
			gameC.reset();
			elementsReset(true);
		}
	}
	glutTimerFunc(fpsScale, timerFunc, 1);
	glutPostRedisplay();
}


//重置元素参数
void elementsReset(bool re)
{	//ghSpeed表示是否重置幽灵的速度
	food.Food::Food();
	pacman.reset({ 7,10 }, re);
	const Coord tmpC[] = { {13, 1}, { 4,6 }, { 10,8 }, { 2,13 } };
	const Color_f tmpclr[] = { { 1,0,0 } ,{ 0,1,1 } ,{ 1.0,0.6,0 },{ 1.,0,0.6 } };
	ghostReset(ghosts, tmpC, tmpclr, re);
	gMap.setColor(re);	//更改地图墙壁颜色
}


//窗口调整函数
void reshapeFunc(int width, int height)
{
	glMatrixMode(GL_PROJECTION);	//设置投影方式
	glLoadIdentity();	//恢复初始坐标系
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);		//设置视图窗口: x坐标, y坐标, 宽度, 高度
	glOrtho(0, WINWIDTH, WINHEIGIHT, 0, -1, 1.);	//设置视景体(左,右,下,上,近,远)
	glMatrixMode(GL_MODELVIEW);		//设置投影方式: 模型视图
	glLoadIdentity();
}

//按键按下
void keyboardFunc(unsigned char key, int x, int y)
{
	switch (key)
	{
	case ' ':	//空格
		gameC.playFlag = true;
		break;
	case 27:	//Esc
		gameC.GameExit();	//退出游戏
		break;
	}
}

//抬起按键
void keyboardUpFunc(unsigned char key, int x, int y)
{
	switch (key)
	{
	case ' ':
		gameC.resetPlayLine();	//重绘play横线
	}
}

//特殊按键按下
void specialKeyFunc(int key, int x, int y)
{
	switch (key)
	{
	//上下左右
	case GLUT_KEY_DOWN:
		pacman.downFlag = true;
		break;
	case GLUT_KEY_UP:
		pacman.upFlag = true;
		break;
	case GLUT_KEY_LEFT:
		pacman.leftFlag = true;
		break;
	case GLUT_KEY_RIGHT:
		pacman.rightFlag = true;
	}
}

//特殊按键抬起
void specialKeyUpFunc(int key, int x, int y)
{
	switch (key)
	{
	//上下左右
	case GLUT_KEY_DOWN:
		pacman.downFlag = false;
		break;
	case GLUT_KEY_UP:
		pacman.upFlag = false;
		break;
	case GLUT_KEY_LEFT:
		pacman.leftFlag = false;
		break;
	case GLUT_KEY_RIGHT:
		pacman.rightFlag = false;
	}
}


void mouseFunc(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON &&
		x >= WINWIDTH / 2 - 40. && x <= WINWIDTH / 2 + 40 &&
		y >= 365 && y <= 400)
	{
		if (state == GLUT_DOWN)
			gameC.playFlag = true;
		else
			gameC.resetPlayLine();
	}
}


