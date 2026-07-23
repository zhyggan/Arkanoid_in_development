#pragma once

#include "Config.h"

//**************************************************************************************************************
enum EBall_State
{
	EBS_Normal,
	EBS_Lost,
	EBS_On_Platform
};
//**************************************************************************************************************
class ABall;
class AHit_Checker
{
public:
	virtual bool Check_Hit(double next_x_pos, double next_y_pos, ABall *ball) = 0;
	// virtual значит, что метод может быть определен как в базовом классе так и в производном
	// но если добавить = 0, то это значит, что метода не должно быть в базовом классе, и такой метод обязательно должен присутсвовать в производном классе,
	// = 0 делает метод абстрактным
	// абстрактный класс - у которого все методы абстрактные (абстрактный класс = интерфейс) 

	// в контексте данного проекта AHit_Checker позволит скрыть от мячика реализацию рамки, и передавая указатель на AHit_Checker на самом деле передаем указатель на AsBorder,
	// но класс ABall уже не будет знать про реализацию метода Сheck_Border_Hit в другом классе
	// ИНТЕРФЕЙС помогает скрыть реализацию одного класса от другого, не рекомендуется злоупотреблять интерфейсами т.к. потом вносить изменения очень сложно,
	// ПРИМЕНЯТЬ ТОЛЬКО ТАМ, ГДЕ НУЖНО РАЗОРВАТЬ ЗАВИСИМОСТЬ ДВУХ КЛАССОВ

	bool Hit_Ball_On_Line(double y, double next_x_pos, double left_x, double right_x, double radius, double &x);
};
//**************************************************************************************************************
class ABall
{
public:
	ABall();

	void Draw(HDC hdc, RECT &paint_area);
	void Move();
	void Set_For_Test();
	bool Is_Test_Finished();
	EBall_State Get_State();
	void Set_State(EBall_State new_state, double x_pos, double y_pos = Start_Ball_Y_Pos);  // если в функции параметру присваивается значение, 
																														// то в такую ф-ю можно передать два аргумента, а 3-му присвоится заданый
																														// и записывается присваивание только в заголовочном файле			
	double Get_Direction();
	void Set_Direction(double new_direction);
	void Reflect(bool from_horizontal);
	bool Is_Moving_Up();
	bool Is_Moving_Left();

	double Ball_Speed;

	static void Add_Hit_Checker(AHit_Checker *hit_checker);

	static const double Radius;

private:
	void Redraw_Ball();

	EBall_State Ball_State;
	double Rest_Distance;
	double Ball_Direction;

	bool Testing_Is_Active;
	int Test_Iteration;
	double Rest_Test_Distance;

	double Center_X_Pos, Center_Y_Pos;

	RECT Ball_Rect, Prev_Ball_Rect;

	static const double Start_Ball_Y_Pos;
	static int Hit_Checkers_Count;
	static AHit_Checker *Hit_Checkers[3];
};
//**************************************************************************************************************