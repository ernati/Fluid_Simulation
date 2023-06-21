#pragma once

class Vector2D {

private :
    //벡터 성분
    float X;
    float Y;

public :
    //생성자 - 아무 입력이 없다면 영벡터
    Vector2D() {
        X=0;
        Y=0;
    }

    //생성자 - 성분값이 주어지면 그대로 
    Vector2D( float x, float y ) {
        X = x;
        Y = y;
    }

    ~Vector2D() {

    }

    //연산자 재정의 - element wise
    Vector2D operator+ (const Vector2D& c)
    {
        Vector2D tmp = Vector2D();
        tmp.X = c.X + this->X;
        tmp.Y = c.Y + this->Y; 
        return tmp;
    }

    Vector2D operator- (const Vector2D& c)
    {
        Vector2D tmp = Vector2D();
        tmp.X = c.X - this->X;
        tmp.Y = c.Y - this->Y; 
        return tmp;
    }

    Vector2D operator* (const Vector2D& c)
    {
        Vector2D tmp = Vector2D();
        tmp.X = c.X * this->X;
        tmp.Y = c.Y * this->Y; 
        return tmp;
    }

    Vector2D operator/ (const Vector2D& c)
    {
        Vector2D tmp = Vector2D();
        tmp.X = c.X / this->X;
        tmp.Y = c.Y / this->Y; 
        return tmp;
    }


};