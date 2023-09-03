#pragma once

class Vector3D {

public:

    //벡터 성분
    double X;
    double Y;
    double Z;


    //생성자 - 아무 입력이 없다면 영벡터
    Vector3D() {
        X = 0;
        Y = 0;
        Z = 0;
    }

    //생성자 - 성분값이 주어지면 그대로 
    Vector3D(double x, double y, double z) {
        X = x;
        Y = y;
        Z = z;
    }

    ~Vector3D() {

    }

    //scalar와의 연산
    Vector3D operator+ (const double& c)
    {
        Vector3D tmp = Vector3D();
        tmp.X = c + this->X;
        tmp.Y = c + this->Y;
        tmp.Z = c + this->Z;
        return tmp;
    }

    Vector3D operator- (const double& c)
    {
        Vector3D tmp = Vector3D();
        tmp.X = this->X - c;
        tmp.Y = this->Y - c;
        tmp.Z = this->Z - c;
        return tmp;
    }

    Vector3D operator* (const double& c)
    {
        Vector3D tmp = Vector3D();
        tmp.X = this->X * c;
        tmp.Y = this->Y * c;
        tmp.Z = this->Z * c;
        return tmp;
    }

    Vector3D operator/ (const double& c)
    {
        Vector3D tmp = Vector3D();
        tmp.X = this->X / c;
        tmp.Y = this->Y / c;
        tmp.Z = this->Z / c;
        return tmp;
    }

    //연산자 재정의 - element wise
    Vector3D operator+ (const Vector3D& c)
    {
        Vector3D tmp = Vector3D();
        tmp.X = X + c.X;
        tmp.Y = Y + c.Y;
        tmp.Z = Z + c.Z;
        return tmp;
    }

    Vector3D operator- (const Vector3D& c)
    {
        Vector3D tmp = Vector3D();
        tmp.X = X - c.X;
        tmp.Y = Y - c.Y;
        tmp.Z = Z - c.Z;
        return tmp;
    }

    Vector3D operator* (const Vector3D& c)
    {
        Vector3D tmp = Vector3D();
        tmp.X = X * c.X;
        tmp.Y = Y * c.Y;
        tmp.Z = Z * c.Z;
        return tmp;
    }

    Vector3D operator/ (const Vector3D& c)
    {
        Vector3D tmp = Vector3D();
        tmp.X = X / c.X;
        tmp.Y = Y / c.Y;
        tmp.Z = Z / c.Z;
        return tmp;
    }


};