#pragma once

class Vector2D {

public:

    //���� ����
    float X;
    float Y;


    //������ - �ƹ� �Է��� ���ٸ� ������
    Vector2D() {
        X = 0;
        Y = 0;
    }

    //������ - ���а��� �־����� �״�� 
    Vector2D(float x, float y) {
        X = x;
        Y = y;
    }

    ~Vector2D() {

    }

    //scalar���� ����
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

    //������ ������ - element wise
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