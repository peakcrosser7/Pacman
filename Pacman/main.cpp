#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")		//取消开启控制台
#include "GL/glut.h"
#include "main.h"
#include "game_elements.h"
#include "game_func.h"


extern GameConsole gameC;

int main(int argc, char** argv)
{
	glutInit(&argc, argv);		//glut初始化
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(WINWIDTH, WINHEIGIHT);	//窗口大小
	glutInitWindowPosition(WINPOSX, WINPOSY);	//窗口位置
	glutCreateWindow("Pacman V1.0");	//窗口标题
	glutDisplayFunc(displayFunc);		//显示函数
	glutTimerFunc(fpsScale, timerFunc, 1);		//定时调用显示函数
	glutReshapeFunc(reshapeFunc);		//窗口大小变化调用函数

	glutKeyboardFunc(keyboardFunc);		//按键按下响应
	glutKeyboardUpFunc(keyboardUpFunc);		//按键抬起
	glutSpecialFunc(specialKeyFunc);	//特殊按键按下响应
	glutSpecialUpFunc(specialKeyUpFunc);	//特殊按键抬起
	glutMouseFunc(mouseFunc);
	glutMainLoop();		//主循环

	return 0;
}



bool Coord::operator<(const Coord& c) const
{
	if (x == c.x)
		return y < c.y;
	return x < c.x;
}
