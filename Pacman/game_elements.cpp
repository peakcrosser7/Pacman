#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <cstdio>
#include "game_elements.h"
#include "GL/glut.h"


extern const int BLOCKWIDTH;
extern const int BLOCKCNT;
extern bool gameActive;
extern bool gameOver;


Pacman::Pacman(const Coord& c)
{
	dir = RIGTH;
	pos = { c.x * BLOCKWIDTH + BLOCKWIDTH / 2,c.y * BLOCKWIDTH + BLOCKWIDTH / 2 };
	lifeCnt = 3;
	ghostFlag = true;
	ghostTime = time(nullptr);
}


//检测吃豆人与墙的碰撞
bool Pacman::checkLeftCollision(const GameMap& gm) const
{
	int x = static_cast<int>(pos.x - R - SPEED) / BLOCKWIDTH;
	int  y1 = static_cast<int>(pos.y - R) / BLOCKWIDTH;
	int y2 = static_cast<int>(pos.y + R) / BLOCKWIDTH;
	if(gm.BLOCKS[x][y1]&&gm.BLOCKS[x][y2])
		return false;	//未碰撞
	return true;	//碰撞
}


bool Pacman::checkRightCollision(const GameMap& gm) const
{
	int x = static_cast<int>(pos.x + R + SPEED) / BLOCKWIDTH;
	int y1 = static_cast<int>(pos.y - R) / BLOCKWIDTH;
	int y2 = static_cast<int>(pos.y + R) / BLOCKWIDTH;
	if (gm.BLOCKS[x][y1] && gm.BLOCKS[x][y2])
		return false;
	return true;
}

bool Pacman::checkUpCollision(const GameMap& gm) const
{
	int x1 = static_cast<int>(pos.x - R) / BLOCKWIDTH;
	int x2 = static_cast<int>(pos.x + R) / BLOCKWIDTH;
	int y = static_cast<int>(pos.y - R - SPEED) / BLOCKWIDTH;
	if (gm.BLOCKS[x1][y]&&gm.BLOCKS[x2][y])
		return false;
	return true;
}

bool Pacman::checkDownCollision(const GameMap& gm) const
{
	int x1 = static_cast<int>(pos.x - R) / BLOCKWIDTH;
	int x2 = static_cast<int>(pos.x + R) / BLOCKWIDTH;
	int y = static_cast<int>(pos.y + R + SPEED) / BLOCKWIDTH;
	if (gm.BLOCKS[x1][y] && gm.BLOCKS[x2][y])
		return false;
	return true;
}

//绘制吃豆人
void Pacman::draw()
{
	double x, y;
	double perRad = M_PI / 180;
	if(ghostFlag)	//正常颜色
		glColor3f(CLR[0].r, CLR[0].g, CLR[0].b);
	else	//无敌时的颜色
		glColor3f(CLR[1].r, CLR[1].g, CLR[1].b);
	
	//绘制图形(扇形)
	glBegin(GL_LINES);	
	for (double i = mouthAngle; i < 360-mouthAngle; i++)
	{
		x = pos.x;
		y = pos.y;
		glVertex2d(x, y);
		x += R * cos((i + 90. * dir) * perRad);
		y += R * sin((i + 90. * dir) * perRad);
		glVertex2d(x, y);
	}
	glEnd();

	//更新张嘴角度
	if (mouthFlag)
	{
		mouthAngle += MOUTHSPEED;
		if (mouthAngle >= mouthAngleMax)
			mouthFlag = false;
	}
	else
	{
		mouthAngle -= MOUTHSPEED;
		if (mouthAngle <= mouthAngleMin)
			mouthFlag = true;
	}
}


//吃豆人移动
void Pacman::move(const GameMap& gm)
{
	if (upFlag && !checkUpCollision(gm))
	{
		pos.y -= SPEED;
		dir = UP;
	}

	if (downFlag && !checkDownCollision(gm))
	{
		pos.y += SPEED;
		dir = DOWN;
	}
	if (leftFlag && !checkLeftCollision(gm))
	{
		pos.x -= SPEED;
		dir = LEFT;
	}
	if (rightFlag && !checkRightCollision(gm))
	{
		pos.x += SPEED;
		dir = RIGTH;
	}
}


//吃豆人进食
void Pacman::eatFood(Food& f, GameConsole & gc)
{
	for (auto it = f.cor.begin(); it != f.cor.end();)	//遍历食物
	{
		//若满足距离条件
		if (abs((it->x + 0.5) * BLOCKWIDTH - pos.x) <= R &&
			abs((it->y + 0.5) * BLOCKWIDTH - pos.y) <= R)
		{
			it = f.cor.erase(it);		//去除食物
			gc.scores += gc.scoreInc;	//增加积分
		}
		else
			++it;
	}
}


//检测与幽灵的碰撞
void Pacman::meetGhost(const Ghost * ghp)
{
	for (int i = 0; i < Ghost::cnt; i++)	//遍历所有幽灵
	{
		//幽灵中心坐标
		Coord ghPos = { ghp[i].corMulWid.x + BLOCKWIDTH / 2,
		ghp[i].corMulWid.y + BLOCKWIDTH - ghp[i].R / 2 };

		if (time(nullptr) - ghostTime <= 1.5)
			ghostFlag = false;		//距上次碰到幽灵时间在1.5s内则设置为无敌状态
		else	//非无敌状态
		{
			ghostFlag = true;
			if (abs(pos.x - ghPos.x) <= 1.5 * R && abs(pos.y - ghPos.y) <= R && lifeCnt > 0)	//若距离满足
			{
				--lifeCnt;
				ghostTime = time(nullptr);		//更新碰撞时间
			}
		}
	}
}


//吃豆人重置参数
void Pacman::reset(const Coord& c, bool lifeRe)
{
	dir = RIGTH;
	pos = { c.x * BLOCKWIDTH + BLOCKWIDTH / 2,c.y * BLOCKWIDTH + BLOCKWIDTH / 2 };
	if (lifeRe)
		lifeCnt = 3;
}


//绘制地图
void GameMap::draw() const
{
	//绘制墙外围
	glColor3f(CLR[clrIdx].r, CLR[clrIdx].g, CLR[clrIdx].b);		//设置墙的颜色
	for (int i = 0; i < BLOCKCNT; i++)
		for (int j = 0; j < BLOCKCNT; j++)
			if (!BLOCKS[i][j])
				glRecti(i * BLOCKWIDTH, j * BLOCKWIDTH,
					i * BLOCKWIDTH + BLOCKWIDTH, j * BLOCKWIDTH + BLOCKWIDTH);
	//绘制墙内部
	glColor3f(1,1,1);
	for (int i = 0; i < BLOCKCNT; i++)
		for (int j = 0; j < BLOCKCNT; j++)
			if (!BLOCKS[i][j])
				glRecti(i * BLOCKWIDTH + 5, j * BLOCKWIDTH + 5,
					i * BLOCKWIDTH + BLOCKWIDTH - 5, j * BLOCKWIDTH + BLOCKWIDTH - 5);
}


//设置墙壁颜色
void GameMap::setColor(bool re)
{
	if (!re)	//下一种颜色
		clrIdx = (clrIdx + 1) % 3;	//更改地图墙壁颜色
	else	//重置
		clrIdx = 0;
}


Food::Food()
{
	cor = {		//食物位置
		{1,1},{2,1},{3,1},{4,1},{5,1},			//1
		{9,1},{10,1},{11,1},{12,1},{13,1},
		{1,2},{3,2},{5,2},{6,2},{8,2},			//2
		{9,2},{11,2},{13,2},
		{1,3},{3,3},{6,3},{8,3},{11,3},			//3
		{13,3},
		{1,4},{3,4},{4,4},{6,4},{8,4},			//4
		{10,4},{11,4},{13,4},
		{1,5},{4,5},{5,5},{6,5},{7,5},			//5
		{8,5},{9,5},{10,5},{13,5},
		{1,6},{2,6},{3,6},{4,6},{7,6},			//6
		{10,6},{11,6},{12,6},{13,6},
		{1,7},{4,7},{6,7},{7,7},{8,7},			//7
		{10,7},{13,7},
		{1,8},{3,8},{4,8},{10,8},{11,8},		//8
		{13,8},
		{1,9},{2,9},{3,9},{7,9},{11,9},			//9
		{12,9},{13,9},
		{1,10},{3,10},{5,10},{6,10},{8,10},		//10
		{9,10},{11,10},{13,10},
		{1,11},{3,11},{4,11},{5,11},{7,11},		//11
		{9,11},{10,11},{11,11},{13,11},
		{1,12},{4,12},{7,12},{10,12},{13,12},	//12
		{1,13},{2,13},{3,13},{4,13},{5,13},		//13
		{6,13},{7,13},{8,13},{9,13},{10,13},
		{11,13},{12,13},{13,13}
	};
}


//绘制食物
void Food::draw() const
{
	glPointSize(pointSize);		//食物大小
	glBegin(GL_POINTS);
	glColor3f(CLR.r, CLR.g, CLR.b);
	double HalfBlock = BLOCKWIDTH / 2;
	for (const Coord &f : cor)
		glVertex2d(f.x * BLOCKWIDTH + HalfBlock, f.y * BLOCKWIDTH + HalfBlock);
	glEnd();
}


int Ghost::cnt = 0;		//初始化幽灵数目
const double Ghost::initSpeed = 0.2 * fpsScale;
const double Ghost::speedInc = 0.05 * fpsScale;

Ghost::Ghost(const Coord& c, const Color_f& rgb)
{
	corMulWid = {c.x*BLOCKWIDTH, c.y*BLOCKWIDTH};		//幽灵坐标
	srand(time(nullptr));
	dir = static_cast<Direction>(rand() % 4);		//运动方向
	clr = rgb;		//颜色
	speed = initSpeed;	//速度
	++cnt;		//数目+1
}


//绘制幽灵
void Ghost::draw() const
{
	double x, y;
	Coord pos= { corMulWid.x + BLOCKWIDTH / 2,corMulWid.y + BLOCKWIDTH - R / 3 };
	double perRad = M_PI / 180;
	glColor3f(clr.r, clr.g, clr.b);
	//上身
	glBegin(GL_LINES);
	for (double i = 180; i < 360; i++)
	{
		x = pos.x;
		y = pos.y;
		glVertex2d(x, y);
		x += R * cos(i * perRad);
		y += R * sin(i * perRad);
		glVertex2d(x, y);
	}
	glEnd();
	//腿
	glLineWidth(2);
	glBegin(GL_LINES);
	for (int i = 0; i < 4; i++)
	{
		double orix = pos.x - R + i * R / 2;
		for (int j = 1; j <= R / 2; j++)
		{
			glVertex2d(orix, pos.y);
			x = pos.x - R + i * R / 2 + j;
			y = R /4 * sin(2. * M_PI / R *j) + pos.y;
			glVertex2d(x, y);
		}
	}
	glEnd();
	//眼睛
	glPointSize(10);
	glColor3f(1., 1., 1.);
	glBegin(GL_POINTS);
	glVertex2d(pos.x - R / 3, pos.y - R / 3);
	glVertex2d(pos.x + R / 3, pos.y - R / 3);
	glEnd();
	glPointSize(4);
	glColor3f(0, 0, 0);
	glBegin(GL_POINTS);
	glVertex2d(pos.x - R / 3, pos.y - R / 3);
	glVertex2d(pos.x + R / 3, pos.y - R / 3);
	glEnd();
}


//幽灵自动移动
void Ghost::autoMove(const GameMap& gm)
{
	//检测是否撞墙, 撞墙则更改移动方向
	while (checkCollision(dir, gm))
		dir = static_cast<Direction>(rand() % 4);
		
	switch (dir)
	{
	case RIGTH:
		corMulWid.x += speed;
		return;
	case DOWN:
		corMulWid.y += speed;
		return;
	case LEFT:
		corMulWid.x -= speed;
		return;
	case UP:
		corMulWid.y -= speed;
		return;
	}
}


//幽灵与墙碰撞检测
bool Ghost::checkCollision(Direction d, const GameMap& gm) const
{
	int x, y;
	switch (d)
	{
	case RIGTH:
		x = (corMulWid.x + BLOCKWIDTH) / BLOCKWIDTH;
		if (gm.BLOCKS[x][int((corMulWid.y + R / 2) / BLOCKWIDTH)])
			return false;
		break;
	case DOWN:
		y = (corMulWid.y + BLOCKWIDTH) / BLOCKWIDTH;
		if (gm.BLOCKS[int((corMulWid.x + R / 2) / BLOCKWIDTH)][y])
			return false;
		break;
	case LEFT:
		x = (corMulWid.x-speed) / BLOCKWIDTH;
		if (gm.BLOCKS[x][int((corMulWid.y + R / 2) / BLOCKWIDTH)])
			return false;
		break;
	case UP:
		y = (corMulWid.y-speed) / BLOCKWIDTH;
		if (gm.BLOCKS[int((corMulWid.x + R / 2) / BLOCKWIDTH)][y])
			return false;	//未碰撞
		break;
	}
	return true;	//碰撞
}


GameConsole::GameConsole()
{
	gameActive = false;
	gameOver = false;
	playFlag = false;
	playLneLen = 0;
	scores = 0;
	scoreInc = 10;
	level = 1;
}


//绘制文字
void GameConsole::printStr(const std::string msg, void* font) const
{
	for (unsigned i = 0; i < msg.size(); i++)
		glutBitmapCharacter(font, msg[i]);
}


//绘制欢迎界面
void GameConsole::drawWelcomeScreen()
{
	glClearColor(0, 0, 0, 1);
	glColor3f(1, 1, 1);
	glRasterPos2d(WINWIDTH / 2 - 50., 200);
	printStr("PACMAN", GLUT_BITMAP_TIMES_ROMAN_24);
	if (playFlag)	//若按下play则变红
		glColor3f(1, 0, 0);
	glRasterPos2d(WINWIDTH / 2 - 30., playPos + 5);
	printStr("PLAY", GLUT_BITMAP_TIMES_ROMAN_24);
	playAn();
}


//play横线动画
void GameConsole::playAn()
{
	//绘制横线
	if (playFlag)
	{
		if (playLneLen < 50)
			playLneLen += 0.2 * fpsScale;
	
		glColor3f(1, 1, 1);
		glBegin(GL_LINES);
		glVertex2d(WINWIDTH / 2 - 100., playPos);
		glVertex2d(WINWIDTH / 2 - 100. + playLneLen, playPos);
		glVertex2d(WINWIDTH / 2 + 100., playPos);
		glVertex2d(WINWIDTH / 2 + 100. - playLneLen, playPos);
		glEnd();
	}
	if (playLneLen >= 50)	//到达最大值开始游戏
		gameActive = true;
}


//绘制计分板
void GameConsole::drawScoreBoard(const Pacman& pac)
{
	updateHiScore();	//更新最高分

	//绘制最高分/等级/分数
	glColor3f(1, 0, 0);
	glRasterPos2d((WINHEIGIHT + WINWIDTH) / 2 - 45., 85);
	printStr("HI-SCORE", GLUT_BITMAP_TIMES_ROMAN_24);

	glRasterPos2d((WINHEIGIHT + WINWIDTH) / 2 - 15., 160);
	printStr("Level " + std::to_string(level), GLUT_BITMAP_HELVETICA_18);

	glColor3f(1, 1, 1);
	glRasterPos2d((WINHEIGIHT + WINWIDTH) / 2 - 5., 120);
	printStr(std::to_string(hiScore), GLUT_BITMAP_TIMES_ROMAN_24);
	glRasterPos2d((WINHEIGIHT + WINWIDTH) / 2 - 5., 200);
	printStr(std::to_string(scores), GLUT_BITMAP_TIMES_ROMAN_24);

	drawPacmanLife(pac);	//绘制吃豆人生命值
}


//更新最高分
void GameConsole::updateHiScore()
{
	if (scores > hiScore)
		hiScore = scores;
}


//绘制吃豆人生命值
void GameConsole::drawPacmanLife(const Pacman& pac)
{

	const Color_f pacClr = { 1,1,0 };
	Coord pos = { (WINHEIGIHT + WINWIDTH) / 2 - 100., WINHEIGIHT - 100. };
	double perRad = M_PI / 180;
	double x, y;
	int R = 20;

	glColor3f(pacClr.r, pacClr.g, pacClr.b);
	glBegin(GL_LINES);
	for (int i = 0; i < pac.getLifeCnt(); i++)
	{
		pos.x += 50;
		for (double j = 30; j < 330; j++)
		{
			x = pos.x;
			y = pos.y;
			glVertex2d(x, y);
			x += R * cos(j * perRad);
			y += R * sin(j * perRad);
			glVertex2d(x, y);
		}
	}
	glEnd();
}


//读取最高分
void GameConsole::readHiScore()
{
	std::ifstream fin;
	fin.open("HiScore.dat");
	if (fin.is_open())
	{
		fin >> hiScore;
		fin.close();
	}
	else
		hiScore = 0;
}


//绘制游戏结束
void GameConsole::drawGameOver() const
{
	glColor3f(1, 1, 0);
	glRasterPos2d((WINHEIGIHT + WINWIDTH) / 2 - 60., WINHEIGIHT - 100.);
	printStr("GAME OVER!", GLUT_BITMAP_TIMES_ROMAN_24);

}


//检测游戏结束
bool GameConsole::checkGameOver(const Pacman& pac)
{
	if (pac.getLifeCnt() == 0)
		return true;	//游戏结束
	return false;		//游戏未结束
}


//重置参数
void GameConsole::reset()
{
	gameActive = false;
	gameOver = false;
	playFlag = false;
	playLneLen = 0;
	scores = 0;
	scoreInc = 10;
	level = 1;
}


//保存最高分
void GameConsole::saveHiScore() const
{
	std::ofstream fout;
	fout.open("hiscore.dat");
	fout << hiScore;
	fout.close();
}


//退出游戏
void GameConsole::GameExit()
{
	if (!gameOver)
		gameOver = true;	//游戏进行时仅游戏结束回到欢迎界面
	else if (!gameActive)
		exit(0);		// 在欢迎界面则退出游戏
}


//游戏升级
void GameConsole::upgrade(Ghost * ghp)
{
	++level;
	scoreInc = 10 + (level - 1) * 5;	//更改得分增量
	if (level < 6)		//更改幽灵速度
		for (int i = 0; i < Ghost::cnt; ++i)
			ghp[i].speed += Ghost::speedInc;
}


//绘制FPS
void GameConsole::drawFPS()
{
	//计算
	frame++;
	int curTime = glutGet(GLUT_ELAPSED_TIME);
	if (curTime - timeBase > 1000)
	{
		FPS = frame * 1000/ (curTime - timeBase);
		timeBase = curTime;
		frame = 0;
	}
	//绘制
	std::string msg = "FPS:" + std::to_string(FPS);
	glColor3f(1, 1, 1);
	glRasterPos2d(WINWIDTH - 70., WINHEIGIHT-10.);
	printStr(msg, GLUT_BITMAP_HELVETICA_12);
}


//重置幽灵参数
void ghostReset(Ghost* ghp, const Coord * cp, const Color_f * clrp, bool speedRe)
{
	for (int i = 0; i < Ghost::cnt; i++)
	{
		ghp[i].corMulWid = { cp[i].x * BLOCKWIDTH, cp[i].y * BLOCKWIDTH };
		ghp[i].clr = clrp[i];
		if (speedRe)		//speedCHg为true则重置幽灵速度
			ghp[i].speed = Ghost::initSpeed;
	}
}


