#include "Config.h"

// AColor
//**************************************************************************************************************
AColor::AColor()
: R(0), G(0), B(0), Pen(0), Brush(0)
{
}
//**************************************************************************************************************
AColor::AColor(unsigned char r, unsigned char g, unsigned char b)
: R(r), G(g), B(b), Pen(0), Brush(0)
{
	Pen = CreatePen(PS_SOLID, 0, RGB(r, g, b));
	Brush = CreateSolidBrush(RGB(r, g, b));
}
//**************************************************************************************************************
int AColor::Get_RGB() const
{
	return RGB(R, G, B);
}
//**************************************************************************************************************
void AColor::Select(HDC hdc) const
{
	SelectObject(hdc, Pen);
	SelectObject(hdc, Brush);
}
//**************************************************************************************************************





// AsConfig
bool AsConfig::Level_Has_Floor = false;
int AsConfig::Current_Timer_Tick = 0;

const AColor AsConfig::BG_Color(15, 63, 31);
const AColor AsConfig::Red_Color(255, 85, 85);
const AColor AsConfig::Blue_Color(85, 255, 255);
const AColor AsConfig::White_Color(255, 255, 255);

HWND AsConfig::Hwnd;

const double AsConfig::Moving_Step_Size = 1.0 / AsConfig::Global_Scale;
//**************************************************************************************************************
int AsConfig::Rand(int range)
{// Вычисляет псевдослучайное число в диапазоне [0, .. range - 1]

	return rand() * range / RAND_MAX; // (RAND_MAX + 1) чтобы не выйти за пределы массива
}
//**************************************************************************************************************
