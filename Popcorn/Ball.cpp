#include "Ball.h"

// AHit_Checker
//**************************************************************************************************************
bool AHit_Checker::Hit_Ball_On_Line(double y, double next_x_pos, double left_x, double right_x, double radius, double &x)
{// Проверяет пересечение горизонтального отрезка (проходящего от left_x до right_x через y) с окружностью радиусом radius

	double min_x, max_x;
	// x*x + y*y = R*R

	//y = next_y_pos - brick_y_pos; 
	// мы делаем этот расчет для того чтобы получить значение, которое потом сравниваем с радиусом, и если это значение меньше чем радиус значит произошло столкновение
	// потом проверяем чтобы по х лежал в кирпиче и только потом алгоритм отскока 

	if (y > radius)
		return false;

	x = sqrt(radius * radius - y * y);

	max_x = next_x_pos + x;
	min_x = next_x_pos - x;

	if (max_x >= left_x && max_x <= right_x  ||  min_x >= left_x && min_x <= right_x)
		return true;
	else
		return false;
}
//**************************************************************************************************************




// ABall
const double ABall::Start_Ball_Y_Pos = 181.0;
const double ABall::Radius = 2.0 - 0.5 / AsConfig::Global_Scale;
int ABall::Hit_Checkers_Count = 0;
AHit_Checker *ABall::Hit_Checkers[] = {};
//**************************************************************************************************************
ABall::ABall()
	: Ball_State(EBS_Normal), Ball_Pen(0), Ball_Brush(0), Center_X_Pos(0.0), Center_Y_Pos(Start_Ball_Y_Pos), Ball_Speed(0.0), 
	  Rest_Distance(0.0), Ball_Direction(0.0), Testing_Is_Active(false), Test_Itertation(0), Ball_Rect{}, Prev_Ball_Rect{}
{
	//Set_State(EBS_Normal, 0);
}
//**************************************************************************************************************
void ABall::Init()
{
	AsConfig::Create_Pen_Brush(255, 255, 255, Ball_Pen, Ball_Brush);
}
//**************************************************************************************************************
void ABall::Draw(HDC hdc, RECT &paint_area)
{
	RECT intersection_rect;

	//1. Очищаем фон
	if (IntersectRect(&intersection_rect, &paint_area, &Prev_Ball_Rect) )
	{
		SelectObject(hdc, AsConfig::BG_Pen);
		SelectObject(hdc, AsConfig::BG_Brush);

		Ellipse(hdc, Prev_Ball_Rect.left, Prev_Ball_Rect.top, Prev_Ball_Rect.right - 1, Prev_Ball_Rect.bottom - 1);
	}

	if (Ball_State == EBS_Lost)
		return;

	//2. Рисуем шарик
	if (IntersectRect(&intersection_rect, &paint_area, &Ball_Rect) )
	{
		SelectObject(hdc, Ball_Pen);
		SelectObject(hdc, Ball_Brush);

		Ellipse(hdc, Ball_Rect.left, Ball_Rect.top, Ball_Rect.right - 1, Ball_Rect.bottom - 1);
	}
}
//**************************************************************************************************************
void ABall::Move()
{
	int i;
	bool got_hit;
	double next_x_pos, next_y_pos;

	if (Ball_State != EBS_Normal)
		return;

	Prev_Ball_Rect = Ball_Rect;
	Rest_Distance += Ball_Speed;

	while (Rest_Distance >= AsConfig::Moving_Step_Size)
	{
		got_hit = false;

		next_x_pos = Center_X_Pos + (AsConfig::Moving_Step_Size * cos(Ball_Direction));
		next_y_pos = Center_Y_Pos - (AsConfig::Moving_Step_Size * sin(Ball_Direction)); // Инвертируем  (ставим знак минус после Ball_Y_Pos) синус чтобы изменить тригонометрию с компьтерной на человеческую

		// Корректируем позицию при отражении:
	 	for (i = 0; i < Hit_Checkers_Count; i++)
			got_hit |= Hit_Checkers[i]->Сheck_Hit(next_x_pos, next_y_pos, this);

		// ^^^^^
		//// Корректируем позицию при отражении:
		//got_hit |= border_hit_checker->Сheck_Hit(next_x_pos, next_y_pos, this); // от рамки 
		//got_hit |= level_hit_checker->Сheck_Hit(next_x_pos, next_y_pos, this);  // от кирпичей
		//got_hit |= platform_hit_checker->Сheck_Hit(next_x_pos, next_y_pos, this);  // от платформы


		if (! got_hit)
		{
			// Мячик продолжит движение, если не взаимодействует с другими объектами
			Rest_Distance -= AsConfig::Moving_Step_Size;

			Center_X_Pos = next_x_pos;
			Center_Y_Pos = next_y_pos;

			if (Testing_Is_Active)
				Rest_Test_Distance -= AsConfig::Moving_Step_Size;
		}
	}

	Redraw_Ball();
}
//**************************************************************************************************************
void ABall::Set_For_Test()
{
	Testing_Is_Active = true;
	Rest_Test_Distance = 30.0;

	//Set_State(EBS_Normal, 80, 189 - Test_Itertation);
	//Ball_Direction = M_PI_4 / 4.0;
	//Ball_Speed = 1.0;

	//Set_State(EBS_Normal, 80 + Test_Itertation, 194);
	//Ball_Direction = M_PI_4;

	//Set_State(EBS_Normal, 100 - Test_Itertation, 194);
	//Ball_Direction = M_PI - M_PI_4;

	Set_State(EBS_Normal, 100 + Test_Itertation, 170);
	Ball_Direction = 5.0;
	Ball_Speed = 1.0;
	
	++Test_Itertation;
}
//**************************************************************************************************************
bool ABall::Is_Test_Finished()
{
	if (Testing_Is_Active)
	{
		if (Rest_Test_Distance <= 0.0)
		{
			Testing_Is_Active = false;
			Set_State(EBS_Lost, 0);
			return true;
		}
	}
	
	return false;
}
//**************************************************************************************************************
EBall_State ABall::Get_State()
{
	return Ball_State;
}
//**************************************************************************************************************
void ABall::Set_State(EBall_State new_state, double x_pos, double y_pos) // если в функции параметру присваивается значение, 
																								 // то в такую ф-ю можно передать два аргумента, а 3-му присвоится заданый
																		  						 // и записывается присваивание только в заголовочном файле
{
	switch (new_state)
	{
	case EBS_Normal:
		Center_X_Pos = x_pos;
		Center_Y_Pos = y_pos;
		Ball_Speed = 3.0;
		Rest_Distance = 0.0;
		Ball_Direction = M_PI_4;
		Redraw_Ball();
		break;


	case EBS_Lost:
		Ball_Speed = 0.0;
		break;


	case EBS_On_Platform:
		Center_X_Pos = x_pos;
		Center_Y_Pos = y_pos;
		Ball_Speed = 0.0;
		Rest_Distance = 0.0;
		Ball_Direction = M_PI_4;
		Redraw_Ball();
		break;
	}

	Ball_State = new_state;
}
//**************************************************************************************************************
double ABall::Get_Direction()
{
	return Ball_Direction;
}
//**************************************************************************************************************
void ABall::Set_Direction(double new_direction)
{
	const double pi_2 = 2.0 * M_PI;

	while (new_direction > pi_2)
		new_direction -= pi_2;

	while (new_direction < 0.0)
		new_direction += pi_2;

	Ball_Direction = new_direction;
}
//**************************************************************************************************************
void ABall::Reflect(bool from_horizontal)
{
	if (from_horizontal)
		Set_Direction(-Ball_Direction);
	else
		Set_Direction(M_PI - Ball_Direction);
}
//**************************************************************************************************************
bool ABall::Is_Moving_Up()
{
	if (Ball_Direction >= 0.0 && Ball_Direction < M_PI)
		return true;
	else
		return false;
}
//**************************************************************************************************************
bool ABall::Is_Moving_Left()
{
	if (Ball_Direction > M_PI_2 && Ball_Direction < M_PI + M_PI_2)
		return true;
	else
		return false;
}
//**************************************************************************************************************
void ABall::Add_Hit_Checker(AHit_Checker *hit_checker)
{
	if (Hit_Checkers_Count >= sizeof(Hit_Checkers) / sizeof(Hit_Checkers[0]) )
		return;

	Hit_Checkers[Hit_Checkers_Count++] = hit_checker;
}
//**************************************************************************************************************
void ABall::Redraw_Ball()
{
	Ball_Rect.left = (int)((Center_X_Pos - Radius) * AsConfig::Global_Scale);
	Ball_Rect.top = (int)((Center_Y_Pos - Radius) * AsConfig::Global_Scale);
	Ball_Rect.right = (int)((Center_X_Pos + Radius) * AsConfig::Global_Scale);
	Ball_Rect.bottom = (int)((Center_Y_Pos + Radius) * AsConfig::Global_Scale);

	InvalidateRect(AsConfig::Hwnd, &Prev_Ball_Rect, FALSE);
	InvalidateRect(AsConfig::Hwnd, &Ball_Rect, FALSE);
}
//**************************************************************************************************************