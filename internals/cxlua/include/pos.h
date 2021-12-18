#pragma once
struct Pos
{
    float x,y;
    Pos()
    {}
    Pos(float x, float y)
    {
        this->x =x;
        this->y = y;
    }
	Pos& operator+(const Pos& pos){
		this->x += pos.x;
		this->y += pos.y;
		return *this;
	}
	Pos& operator-(const Pos& pos) {
		this->x -= pos.x;
		this->y -= pos.y;
		return *this;
	}
};


struct IntPos
{
	int x, y;
};

struct BoxPos
{
	int x, y;
};

struct Bound
{
	float left;
	float right;
	float top;
	float bottom;
};