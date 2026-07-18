#pragma once

#include "Active_Brick.h"

//**************************************************************************************************************
enum ELetter_Type
{
	ELT_C, //Cancels All options. Cancels effects from plates C, L, E, F, S and M. Doesn't affect the multiple balls.
	ELT_I, //Inverts the direction of all balls
	ELT_S, //Slows down all balls
	ELT_G, //Sticks the ball on the bat (Glue)
	ELT_W, //Makes platform wider
	ELT_H, //Catch this plate to earn a new life(heart)
	ELT_M, //The Monsters may get frezzed for one minute

	ELT_T, //Makes Three balls from one
	ELT_L, //Laser gun
	ELT_F, //Adds a Floor to the levels

	ELT_Plus, //Opens a short path to the next level

	ELT_Max
};
//**************************************************************************************************************
enum EFalling_Letter_State
{
	EFLS_Normal,
	EFLS_Vanishing,
	EFLS_Finished
};
//**************************************************************************************************************
class AFalling_Letter: public AGraphics_Object
{
public:
	AFalling_Letter(EBrick_Type brick_type, ELetter_Type letter_type, int x, int y);

	virtual void Act();
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual bool Is_Finished();

	void Get_Letter_Cell(RECT &rect);
	void Vanish();
	void Test_Draw_All_Steps(HDC hdc);

	static void Init();
	static ELetter_Type Get_Random_Letter_Type();

	const EBrick_Type Brick_Type;
	const ELetter_Type Letter_Type;

private:
	void Set_Brick_Letter_Colors(bool is_switch_color, HPEN &front_pen, HBRUSH &front_brush, HPEN &back_pen, HBRUSH &back_brush);
	void Draw_Brick_Letter(HDC hdc);
	void Draw_Line(HDC hdc, int x_1, int y_1, int x_2, int y_2);
	void Draw_Line_To(HDC hdc, int x, int y);

	EFalling_Letter_State Falling_Letter_State;
	int X, Y;
	int Rotation_Step;
	int Next_Rotation_Tick;
	RECT Letter_Cell, Prev_Letter_Cell;

	static const int Ticks_Per_Step = 4;
	static const int Max_Rotation_Step = 16;
	static const int Brick_Half_Height = AsConfig::Brick_Height * AsConfig::Global_Scale / 2;

	static int All_Letters_Popularity;
	static int Letters_Popularity[ELT_Max]; // "Вес" каждой буквы
};
//**************************************************************************************************************