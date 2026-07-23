#pragma once

#include <Windows.h>

#define _USE_MATH_DEFINES
#include <math.h>

//**************************************************************************************************************
class AColor
{
public:
	AColor();
	AColor(unsigned char r, unsigned char g, unsigned char b);

	int Get_RGB() const;
	void Select(HDC hdc) const;

	unsigned char R, G, B;

	HPEN Pen;
	HBRUSH Brush;
};
//**************************************************************************************************************
class AsConfig
{
public:
	static int Rand(int range);

	static bool Level_Has_Floor;
	static int Current_Timer_Tick;
	static const AColor BG_Color, Red_Color, Blue_Color, White_Color;
	static HWND Hwnd;

	static const double Moving_Step_Size;

	static const int Global_Scale = 3;
	static const int Border_X_Offset = 6;
	static const int Border_Y_Offset = 4;
	static const int Level_Width = 12;  // Ширина уровня в ячейках
	static const int Level_Height = 14;  // Высота уровня в ячейках
	static const int Level_X_Offset = 8;
	static const int Level_Y_Offset = 6;
	static const int Cell_Width = 16;
	static const int Cell_Height = 8;
	static const int Brick_Width = 15;
	static const int Brick_Height = 7;
	static const int Max_X_Pos = Level_X_Offset + Cell_Width * Level_Width;
	static const int Max_Y_Pos = 199;
	static const int Platform_Y_Pos = 185;
	static const int FPS = 20;
	static const int Max_Active_Bricks_Count = 10;
	static const int Hits_Per_Letter = 10;  // Вероятность выбить букву = 1.0 / Hits_Per_Letter
	static const int Max_Falling_Letters_Count = 10;
};
//**************************************************************************************************************