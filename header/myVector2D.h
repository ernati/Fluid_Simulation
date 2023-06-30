#pragma once

class Vector2D {

public:

    //벡터 성분
    float X;
    float Y;


    //생성자 - 아무 입력이 없다면 영벡터
    Vector2D() {
        X = 0;
        Y = 0;
    }

    //생성자 - 성분값이 주어지면 그대로 
    Vector2D(float x, float y) {
        X = x;
        Y = y;
    }

    ~Vector2D() {

    }

    //scalar와의 연산
    Vector2D operator+ (const float& c)
    {
        Vector2D tmp = Vector2D();
        tmp.X = c + this->X;
        tmp.Y = c + this->Y;
        return tmp;
    }

    Vector2D operator- (const float& c)
    {
        Vector2D tmp = Vector2D();
        tmp.X = this->X - c;
        tmp.Y = this->Y - c;
        return tmp;
    }

    Vector2D operator* (const float& c)
    {
        Vector2D tmp = Vector2D();
        tmp.X = this->X * c;
        tmp.Y = this->Y * c;
        return tmp;
    }

    Vector2D operator/ (const float& c)
    {
        Vector2D tmp = Vector2D();
        tmp.X = this->X / c;
        tmp.Y = this->Y / c;
        return tmp;
    }

    //연산자 재정의 - element wise
    Vector2D operator+ (const Vector2D& c)
    {
        Vector2D tmp = Vector2D();
        tmp.X = X + c.X;
        tmp.Y = Y + c.Y;
        return tmp;
    }

    Vector2D operator- (const Vector2D& c)
    {
        Vector2D tmp = Vector2D();
        tmp.X = X - c.X;
        tmp.Y = Y - c.Y;
        return tmp;
    }

    Vector2D operator* (const Vector2D& c)
    {
        Vector2D tmp = Vector2D();
        tmp.X = X * c.X;
        tmp.Y = Y * c.Y;
        return tmp;
    }

    Vector2D operator/ (const Vector2D& c)
    {
        Vector2D tmp = Vector2D();
        tmp.X = X / c.X;
        tmp.Y = Y / c.Y;
        return tmp;
    }


};