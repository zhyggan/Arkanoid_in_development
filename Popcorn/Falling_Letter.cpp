#include "Falling_Letter.h"

// AFalling_Letter
int AFalling_Letter::All_Letters_Popularity;
int AFalling_Letter::Letters_Popularity[ELT_Max] = { 7, 7, 7, 7, 7, 7, 7,  3, 3, 3,  1 };
//**************************************************************************************************************
AFalling_Letter::AFalling_Letter(EBrick_Type brick_type, ELetter_Type letter_type, int x, int y)
	:  Brick_Type(brick_type), Letter_Type(letter_type), Falling_Letter_State(EFLS_Normal), X(x), Y(y), Rotation_Step(0), Next_Rotation_Tick(AsConfig::Current_Timer_Tick + Ticks_Per_Step) // И хотя константу менять нельзя, в С++ есть особеность позволяющая задавать значения таким константам в момент создания объекта,
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
	//AColor *color;

	// 1. Очищаем фон
	if (IntersectRect(&intersection_rect, &paint_area, &Prev_Letter_Cell) )
	{
		AsConfig::BG_Color.Select(hdc);
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
void AFalling_Letter::Test_Draw_All_Steps(HDC hdc)
{
	int i;
	int x_step = AsConfig::Cell_Width * AsConfig::Global_Scale;

	Rotation_Step = 0;

	for (i = 0; i < Max_Rotation_Step; i++)
	{
		Draw_Brick_Letter(hdc);

		++Rotation_Step;
		X += x_step;
		Letter_Cell.left += x_step;
		Letter_Cell.right += x_step;
	}
}
//**************************************************************************************************************
void AFalling_Letter::Init()
{
	int i;

	All_Letters_Popularity = 0;

	for (i = 0; i < ELT_Max; i++)
		All_Letters_Popularity += Letters_Popularity[i];
}
//**************************************************************************************************************
ELetter_Type AFalling_Letter::Get_Random_Letter_Type()
{
	int i;
	int letters_popularity;

	letters_popularity = AsConfig::Rand(All_Letters_Popularity);

	for (i = 0; i < ELT_Max; i++)
	{
		if (letters_popularity < Letters_Popularity[i])
			return (ELetter_Type)i;

		letters_popularity -= Letters_Popularity[i];
	}

	return ELT_C;
}
//**************************************************************************************************************
void AFalling_Letter::Set_Brick_Letter_Colors(bool is_switch_color, const AColor **front_color, const AColor **back_color) 
{
	if (is_switch_color)
	{
		*front_color = &AsConfig::Red_Color;
		*back_color = &AsConfig::Blue_Color;
	}
	else
	{
		*front_color = &AsConfig::Blue_Color;
		*back_color = &AsConfig::Red_Color;
	}
}
//**************************************************************************************************************
void AFalling_Letter::Draw_Brick_Letter(HDC hdc)
{// Вывод падающей буквы

	bool switch_color;
	double offset;
	double rotation_angle;  // Преобразование шага в угол поворота
	int back_part_offset;
	const AColor *front_color, *back_color;
	XFORM xform, old_xform;

	if (! (Brick_Type == EBT_Blue || Brick_Type == EBT_Red))
		return;  // Падающие буквы могут быть только от кирпичей такого типа

	// Корректируем шаг вращения и угол поворота
	Rotation_Step = Rotation_Step % Max_Rotation_Step;

	if (Rotation_Step < 8)
		rotation_angle = 2.0 * M_PI / (double)Max_Rotation_Step * (double)Rotation_Step;
	else
		rotation_angle = 2.0 * M_PI / (double)Max_Rotation_Step * (double)(8 - Rotation_Step);

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

	Set_Brick_Letter_Colors(switch_color, &front_color, &back_color);


	if (Rotation_Step == 4 || Rotation_Step == 12)
	{
		// Выводим фон
		back_color->Select(hdc);
		Rectangle(hdc, X, Y + Brick_Half_Height - AsConfig::Global_Scale, X + AsConfig::Brick_Width * AsConfig::Global_Scale, Y + Brick_Half_Height);

		front_color->Select(hdc);
		Rectangle(hdc, X, Y + Brick_Half_Height, X + AsConfig::Brick_Width * AsConfig::Global_Scale, Y + Brick_Half_Height + AsConfig::Global_Scale - 1);
	}
	else
	{
		// Настраиваем матрицу переворота буквы
		xform.eM11 = 1.0f;
		xform.eM12 = 0.0f;
		xform.eM21 = 0.0f;
		xform.eM22 = (float)cos(rotation_angle);
		xform.eDx = (float)X;
		xform.eDy = (float)Y + (float)(Brick_Half_Height);
		GetWorldTransform(hdc, &old_xform);
		SetWorldTransform(hdc, &xform);

		// Выводим фон
		back_color->Select(hdc);
		offset = 3.0 * (1.0 - fabs(xform.eM22)) * (double)AsConfig::Global_Scale;
		back_part_offset = (int)round(offset);
		Rectangle(hdc, 0, -Brick_Half_Height - back_part_offset, AsConfig::Brick_Width * AsConfig::Global_Scale - 1, Brick_Half_Height - back_part_offset);

		// Выводим передний план
		front_color->Select(hdc);
		Rectangle(hdc, 0, -Brick_Half_Height, AsConfig::Brick_Width * AsConfig::Global_Scale - 1, Brick_Half_Height);

		if (Rotation_Step > 4 && Rotation_Step <= 12)
		{
			xform.eM22 = (float)fabs(cos(rotation_angle) );
			SetWorldTransform(hdc, &xform);

			AsConfig::White_Color.Select_Pen(hdc);

			switch (Letter_Type)
			{
			//case ELT_O:
			//	Ellipse(hdc, 0 + 5 * AsConfig::Global_Scale, 1 * AsConfig::Global_Scale - Brick_Half_Height, 0 + 10 * AsConfig::Global_Scale, 6 * AsConfig::Global_Scale - Brick_Half_Height - 1);
			//	break;
			
			case ELT_I:
				Draw_Line(hdc, 6, 1, 8, 1);
				Draw_Line(hdc, 7, 1, 7, 6);
				Draw_Line(hdc, 6, 6, 8, 6);
				break;

			case ELT_T:
				Draw_Line(hdc, 5, 1, 9, 1);
				Draw_Line(hdc, 7, 1, 7, 6);
				break;

			case ELT_S:
				Draw_Line(hdc, 9, 2, 9, 1);
				Draw_Line_To(hdc, 5, 1);
				Draw_Line_To(hdc, 5, 4);
				Draw_Line_To(hdc, 9, 4);
				Draw_Line_To(hdc, 9, 6);
				Draw_Line_To(hdc, 5, 6);
				Draw_Line_To(hdc, 5, 5);
				break;

			case ELT_G:
				Draw_Line(hdc, 9, 1, 5, 1);
				Draw_Line_To(hdc, 5, 6);
				Draw_Line_To(hdc, 9, 6);
				Draw_Line_To(hdc, 9, 4);
				Draw_Line_To(hdc, 7, 4);
				break;

			case ELT_L:
				Draw_Line(hdc, 5, 1, 5, 6);
				Draw_Line_To(hdc, 9, 6);
				break;

			case ELT_W:
				Draw_Line(hdc, 4, 1, 5, 6);
				Draw_Line_To(hdc, 7, 2);
				Draw_Line_To(hdc, 9, 6);
				Draw_Line_To(hdc, 10, 1);
				break;

			case ELT_H:
				Draw_Line(hdc, 5, 1, 5, 6);
				Draw_Line(hdc, 5, 4, 9, 4);
				Draw_Line(hdc, 9, 1, 9, 6);
				break;

			case ELT_M:
				Draw_Line(hdc, 5, 6, 5, 1);
				Draw_Line_To(hdc, 7, 5);
				Draw_Line_To(hdc, 9, 1);
				Draw_Line_To(hdc, 9, 6);
				break;

			case ELT_C:
				Draw_Line(hdc, 9, 2, 9, 1);
				Draw_Line_To(hdc, 5, 1);
				Draw_Line_To(hdc, 5, 6);
				Draw_Line_To(hdc, 9, 6);
				Draw_Line_To(hdc, 9, 5);
				break;
			
			case ELT_Plus:
				Draw_Line(hdc, 7, 1, 7, 6);
				Draw_Line(hdc, 5, 4, 9, 4);
				break;
			}

		}

		SetWorldTransform(hdc, &old_xform);
	}
}
//**************************************************************************************************************
void AFalling_Letter::Draw_Line(HDC hdc, int x_1, int y_1, int x_2, int y_2)
{
	int first_y, second_y;

	first_y = y_1 * AsConfig::Global_Scale - Brick_Half_Height;

	if (y_1 == 6)
		--first_y;

	second_y = y_2 * AsConfig::Global_Scale - Brick_Half_Height;

	if (y_2 == 6)
		--second_y;

	// Offset for central horizontal line
	if (y_1 == 4 && y_2 == 4)
	{
		first_y -= 2;
		second_y -= 2;
	}

	MoveToEx(hdc, x_1 * AsConfig::Global_Scale + 1, first_y, 0);
	LineTo(hdc, x_2 * AsConfig::Global_Scale + 1, second_y);
}
//**************************************************************************************************************
void AFalling_Letter::Draw_Line_To(HDC hdc, int x, int y)
{
	int end_y = y * AsConfig::Global_Scale - Brick_Half_Height;

	if (y == 6)
		--end_y;

	// Offset for central horizontal line
	if (y == 4)
		end_y -= 2;

	LineTo(hdc, x * AsConfig::Global_Scale + 1, end_y);
}
//**************************************************************************************************************