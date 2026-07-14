#include "Falling_Letter.h"

// AFalling_Letter
//**************************************************************************************************************
AFalling_Letter::AFalling_Letter(EBrick_Type brick_type, ELetter_Type letter_type, int x, int y)
	:  Brick_Type(brick_type), Letter_Type(letter_type), Falling_Letter_State(EFLS_Normal), X(x), Y(y), Rotation_Step(2), Next_Rotation_Tick(AsConfig::Current_Timer_Tick + Ticks_Per_Step) // И хотя константу менять нельзя, в С++ есть особеность позволяющая задавать значения таким константам в момент создания объекта,
	// и единствиное место, где можно присвоить константе значение - это только список инициализации в конструкторе такого класса
{
	Letter_Cell.left = X;
	Letter_Cell.top = Y;
	Letter_Cell.right = Letter_Cell.left + AsConfig::Brick_Width * AsConfig::Global_Scale;
	Letter_Cell.bottom = Letter_Cell.top + AsConfig::Brick_Height * AsConfig::Global_Scale;

	Prev_Letter_Cell = Letter_Cell;
}
//**************************************************************************************************************
void AFalling_Letter::Act()
{
	if (Falling_Letter_State != EFLS_Normal)
		return;

	if (Letter_Cell.top >= AsConfig::Max_Y_Pos * AsConfig::Global_Scale) // for invalidate rect if near bottom border
	{
		Vanish();
		return;
	}

	Prev_Letter_Cell = Letter_Cell;

	Y += AsConfig::Global_Scale;
	Letter_Cell.top += AsConfig::Global_Scale;
	Letter_Cell.bottom += AsConfig::Global_Scale;

	if (AsConfig::Current_Timer_Tick >= Next_Rotation_Tick)
	{
		++Rotation_Step;
		Next_Rotation_Tick += Ticks_Per_Step;
	}

	InvalidateRect(AsConfig::Hwnd, &Prev_Letter_Cell, FALSE);
	InvalidateRect(AsConfig::Hwnd, &Letter_Cell, FALSE);
}
//**************************************************************************************************************
void AFalling_Letter::Draw(HDC hdc, RECT &paint_area)
{
	RECT intersection_rect;

	// 1. Очищаем фон
	if (IntersectRect(&intersection_rect, &paint_area, &Prev_Letter_Cell) )
	{
		SelectObject(hdc, AsConfig::BG_Pen);
		SelectObject(hdc, AsConfig::BG_Brush);

		Rectangle(hdc, Prev_Letter_Cell.left, Prev_Letter_Cell.top, Prev_Letter_Cell.right, Prev_Letter_Cell.bottom);
	}

	if (Falling_Letter_State == EFLS_Vanishing)
	{
		Falling_Letter_State = EFLS_Finished;
		return;
	}

	if (IntersectRect(&intersection_rect, &paint_area, &Letter_Cell) )
		Draw_Brick_Letter(hdc);
}
//**************************************************************************************************************
bool AFalling_Letter::Is_Finished()
{
	if (Falling_Letter_State == EFLS_Finished)
		return true;
	else
		return false;
}
//**************************************************************************************************************
void AFalling_Letter::Get_Letter_Cell(RECT &rect)
{
	rect = Letter_Cell;
}
//**************************************************************************************************************
void AFalling_Letter::Vanish()
{
	Falling_Letter_State = EFLS_Vanishing;
	InvalidateRect(AsConfig::Hwnd, &Prev_Letter_Cell, FALSE);
	InvalidateRect(AsConfig::Hwnd, &Letter_Cell, FALSE);
}
//**************************************************************************************************************
void AFalling_Letter::Set_Brick_Letter_Colors(bool is_switch_color, HPEN &front_pen, HBRUSH &front_brush, HPEN &back_pen, HBRUSH &back_brush)
{
	if (is_switch_color)
	{
		front_pen = AsConfig::Brick_Red_Pen;
		front_brush = AsConfig::Brick_Red_Brush;

		back_pen = AsConfig::Brick_Blue_Pen;
		back_brush = AsConfig::Brick_Blue_Brush;
	}
	else
	{
		front_pen = AsConfig::Brick_Blue_Pen;
		front_brush = AsConfig::Brick_Blue_Brush;

		back_pen = AsConfig::Brick_Red_Pen;
		back_brush = AsConfig::Brick_Red_Brush;
	}
}
//**************************************************************************************************************
void AFalling_Letter::Draw_Brick_Letter(HDC hdc)
{// Вывод падающей буквы

	bool switch_color;
	double offset;
	double rotation_angle;  // Преобразование шага в угол поворота
	int brick_half_height = AsConfig::Brick_Height * AsConfig::Global_Scale / 2;
	int back_part_offset;
	HPEN front_pen, back_pen;
	HBRUSH front_brush, back_brush;
	XFORM xform, old_xform;

	if (! (Brick_Type == EBT_Blue || Brick_Type == EBT_Red))
		return;  // Падающие буквы могут быть только от кирпичей такого типа

	//Корректируем шаг вращения и угол поворота
	Rotation_Step = Rotation_Step % 16;

	if (Rotation_Step < 8)
		rotation_angle = 2.0 * M_PI / 16.0 * (double)Rotation_Step;
	else
		rotation_angle = 2.0 * M_PI / 16.0 * (double)(8 - Rotation_Step);

	if (Rotation_Step > 4 && Rotation_Step <= 12)
	{
		if (Brick_Type == EBT_Blue)
			switch_color = true;
		else
			switch_color = false;
	}
	else
	{
		if (Brick_Type == EBT_Red)
			switch_color = true;
		else
			switch_color = false;
	}

	Set_Brick_Letter_Colors(switch_color, front_pen, front_brush, back_pen, back_brush);


	if (Rotation_Step == 4 || Rotation_Step == 12)
	{
		//Выводим фон
		SelectObject(hdc, back_pen);
		SelectObject(hdc, back_brush);

		Rectangle(hdc, X, Y + brick_half_height - AsConfig::Global_Scale, X + AsConfig::Brick_Width * AsConfig::Global_Scale, Y + brick_half_height);

		//Выводим передний план
		SelectObject(hdc, front_pen);
		SelectObject(hdc, front_brush);

		Rectangle(hdc, X, Y + brick_half_height, X + AsConfig::Brick_Width * AsConfig::Global_Scale, Y + brick_half_height + AsConfig::Global_Scale - 1);
	}
	else
	{
		//Настраиваем матрицу переворота буквы
		xform.eM11 = 1.0f;
		xform.eM12 = 0.0f;
		xform.eM21 = 0.0f;
		xform.eM22 = (float)cos(rotation_angle);
		xform.eDx = (float)X;
		xform.eDy = (float)Y + (float)(brick_half_height);
		GetWorldTransform(hdc, &old_xform);
		SetWorldTransform(hdc, &xform);

		//Выводим фон
		SelectObject(hdc, back_pen);
		SelectObject(hdc, back_brush);

		offset = 3.0 * (1.0 - fabs(xform.eM22)) * (double)AsConfig::Global_Scale;
		back_part_offset = (int)round(offset);
		Rectangle(hdc, 0, -brick_half_height - back_part_offset, AsConfig::Brick_Width * AsConfig::Global_Scale, brick_half_height - back_part_offset);

		//Выводим передний план
		SelectObject(hdc, front_pen);
		SelectObject(hdc, front_brush);

		Rectangle(hdc, 0, -brick_half_height, AsConfig::Brick_Width * AsConfig::Global_Scale, brick_half_height);

		if (Rotation_Step > 4 && Rotation_Step <= 12)
		{
			if (Letter_Type == ELT_O)
			{
				SelectObject(hdc, AsConfig::Letter_Pen);
				Ellipse(hdc, 0 + 5 * AsConfig::Global_Scale, (-5 * AsConfig::Global_Scale) / 2, 0 + 10 * AsConfig::Global_Scale, 5 * AsConfig::Global_Scale / 2);
			}
		}

		SetWorldTransform(hdc, &old_xform);
	}
}
//**************************************************************************************************************