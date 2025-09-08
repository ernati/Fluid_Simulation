#pragma once

class Vector2D {

public :

    //벡터 성분
    double X;
    double Y;


    //생성자 - 아무 입력이 없다면 영벡터
    Vector2D() {
        X=0;
        Y=0;
    }

    //생성자 - 성분값이 주어지면 그대로 
    Vector2D( double x, double y ) {
        X = x;
        Y = y;
    }

    ~Vector2D() {

    }

    //scalar와의 연산
    //다만, Vector2D에서만 scalar와의 연산을 정의하고,
    //scalar에서는 따로 정의를 안하면,
    //Vector2D + Scalar 는 컴파일이 제대로 되지만,
    //Scalar + Vector2D 는 컴파일 에러가 뜬다...!
    Vector2D operator+ (const double& c)
    {
        Vector2D tmp = Vector2D();
        tmp.X = c + this->X;
        tmp.Y = c + this->Y;
        return tmp;
    }

    Vector2D operator- (const double& c)
    {
        Vector2D tmp = Vector2D();
        tmp.X = this->X - c;
        tmp.Y = this->Y - c;
        return tmp;
    }

    Vector2D operator* (const double& c)
    {
        Vector2D tmp = Vector2D();
        tmp.X = this->X * c;
        tmp.Y = this->Y * c;
        return tmp;
    }

    Vector2D operator/ (const double& c)
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

    // 등호 연산자 - 두 벡터가 같은지 비교
    bool operator== (const Vector2D& c) const
    {
        return (X == c.X && Y == c.Y);
    }

    // 불등호 연산자 - 두 벡터가 다른지 비교
    bool operator!= (const Vector2D& c) const
    {
        return !(*this == c);
    }

};
