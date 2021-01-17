#ifndef MAIN_H
#define MAIN_H

enum Direction { RIGTH, DOWN, LEFT, UP };

//颜色结构体
struct Color_f
{
	float r;
	float g;
	float b;
};

//坐标结构体
struct Coord
{
	double x;
	double y;
	bool operator<(const Coord& c)const;
};


const int WINWIDTH = 1000;		//窗口宽度
const int WINHEIGIHT = 600;		//窗口高度
const int BLOCKCNT = 15;		//地图块每行个数
const int BLOCKWIDTH = WINHEIGIHT/BLOCKCNT;		//地图块宽度
const int WINPOSX = 200;		//窗口起始位置
const int WINPOSY = 100;
const int fpsScale = 15;		//每秒调用函数值


#endif // !MAIN_H



