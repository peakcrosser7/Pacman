#ifndef MAIN_H
#include "main.h"
#endif // !MAIN_H

#ifndef GAME_ELEMENTS_H
#define GAME_ELEMENTS_H
#include <list>
#include <string>
#include <ctime>


//地图
class GameMap
{
private:
	//颜色
	const Color_f CLR[3] = {
		{ 0.f, 0.2f, 0.6f },
		{ 0.2f, 0.6f, 0.1f },
		{ 0.5f, 0.f, 0.7f }
	};
	int clrIdx = 0;		//当前地图颜色序号

public:	
	const bool BLOCKS[BLOCKCNT][BLOCKCNT] = {	//地图块: 0为墙, 1为道路
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 },
		{ 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0 },
		{ 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0 },
		{ 0, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0 },
		{ 0, 1, 1, 0, 0, 1, 0, 0, 0, 0,	1, 1, 0, 1, 0 },
		{ 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0 },
		{ 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0 },
		{ 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0 },
		{ 0, 1, 1, 0, 0, 1, 0, 0, 0, 0,	1, 1, 0, 1, 0 },
		{ 0, 1, 0, 0, 1, 1, 1, 1, 1, 0,	0, 1, 1, 1, 0 },
		{ 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0 },
		{ 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0 },
		{ 0, 1, 1, 1, 1, 1, 1, 1, 1, 1,	1, 1, 1, 1, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
	};
	void draw() const;		//绘制地图
	void setColor(bool re);
};

//前向声明
class Food;
class Ghost;
class GameConsole;
class TcpCli;

//吃豆人
class Pacman
{
private:
	const double SPEED = 0.15 * fpsScale;	//速度
	const double R = 0.4 * BLOCKWIDTH;		//图形半径
	const Color_f CLR[2] = { { 1.f, 1.f, 0.f }, {1.f, 0.3f, 0.f} };
	Direction dir;	//方向
	Coord pos;	//图形中心点
	const double MOUTHSPEED = 0.3 * fpsScale;	//张嘴速度
	const double mouthAngleMax = 75;	//张嘴最大角
	const double mouthAngleMin = 0;		//张嘴最小角
	bool mouthFlag = true;		//张嘴标识:张嘴为true, 闭嘴为false
	double mouthAngle = 30;		//初始张嘴角度
	int lifeCnt;
	bool ghostFlag;		//幽灵标识: 碰到ghost减生命值时为true, false即为无敌时
	time_t ghostTime;	//上次碰到幽灵的时间
	//与墙的碰撞检测
	bool checkLeftCollision(const GameMap& gm) const;
	bool checkRightCollision(const GameMap& gm) const;
	bool checkUpCollision(const GameMap& gm) const;
	bool checkDownCollision(const GameMap& gm) const;

public:
	//按键标识
	bool upFlag = false;
	bool downFlag = false;
	bool leftFlag = false;
	bool rightFlag = false;
	Pacman(const Coord& c);
	void draw();	//绘制
	void move(const GameMap& gm);	//移动
	void eatFood(Food& f, GameConsole & gc);	//进食
	void meetGhost(const Ghost * ghp);		//与幽灵碰撞检测
	int getLifeCnt() const { return lifeCnt; }	//获取生命值
	void reset(const Coord& c, bool lifeRe);		//重置参数
};


//食物
class Food
{
private:
	const Color_f CLR = { 1.,0.5,0. };	//颜色
	double pointSize = 5.0;		//食物大小
	std::list<Coord> cor;	//位置
	
public:
	Food();
	void draw() const;	//绘制
	unsigned foodCnt() const { return cor.size(); }		//剩余食物数量

	friend class Pacman;
};


//游戏控制台
class GameConsole
{
private:
	const double playPos = 370;		//play按钮位置
	double playLneLen;		//play横线长度
	int hiScore = 0;	//最高分
	int level;		//游戏等级

	int FPS = 0, frame = 0, timeBase = 0;	//FPS参数

	void updateHiScore();	//更新最高分
	void playAn();		//绘制play横线动画
	void printStr(const std::string msg, void* font) const;		//绘制文字
	void drawPacmanLife(const Pacman& pac);		//绘制吃豆人生命值

public:
	bool gameActive;	//游戏活动标识
	bool gameOver;		//游戏结束标识
	bool playFlag;		//play按钮标识: true表示
	int scores;			//分数
	int scoreInc;		//分数增量	
	GameConsole();
	void drawWelcomeScreen();		//绘制欢迎界面
	void resetPlayLine() { playFlag = false; playLneLen = 0; }	//重置play横线
	void drawScoreBoard(const Pacman& pac);		//绘制计分板
	void drawGameOver()const;		//游戏结束
	bool checkGameOver(const Pacman& pac);		//检测游戏是否结束
	void reset();		//重置参数
	void readHiScore();		//读取最高分
	void saveHiScore()const;		//保存最高分
	void GameExit();	//退出游戏
	void upgrade(Ghost * ghp);		//升级
	void drawFPS();		//绘制FPS
};


//幽灵
class Ghost
{
private:
	static const double initSpeed;		//初始速度
	static const double speedInc;		//速度增量
	const double R = 0.46 * BLOCKWIDTH;		//图形半径
	Color_f clr;	//颜色
	Coord corMulWid;	//坐标位置 = 坐标位置*BLOCKWIDTH
	Direction dir;	//移动方向
	double speed;	//速度
	bool checkCollision(Direction d, const GameMap& gm) const;	//检测与墙的碰撞

public:
	static int cnt;		//幽灵数目
	Ghost(const Coord& c, const Color_f& rgb);
	void draw() const;		//绘制
	void autoMove(const GameMap& gm);	//自动移动

	friend class Pacman;
	friend void GameConsole::upgrade(Ghost * ghp);		//升级
	friend void ghostReset(Ghost* ghp, const Coord * cp, const Color_f * clrp, bool speedRe);		//重置幽灵参数
};


#endif // !GAME_ELEMENTS_H