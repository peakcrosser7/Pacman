#ifndef GAME_FUNC_H
#define GAME_FUNC_H

void displayFunc();		//显示函数
void timerFunc(int value);		//周期调用函数
void reshapeFunc(int width, int height);		//窗口调整函数
void keyboardFunc(unsigned char key, int x, int y);		//按键按下函数
void keyboardUpFunc(unsigned char key, int x, int y);	//按键抬起函数	
void specialKeyFunc(int key, int x, int y);		//特殊按键按下函数
void specialKeyUpFunc(int key, int x, int y);	//特殊按键抬起函数
void mouseFunc(int button, int state, int x, int y);
void elementsReset(bool re);		//元素重置


#endif // !GAME_FUNC_H


