#pragma once

class Vector3D {

public:

    //���� ����
    double X;
    double Y;
    double Z;


    //������ - �ƹ� �Է��� ���ٸ� ������
    Vector3D() {
        X = 0;
        Y = 0;
        Z = 0;
    }

    //������ - ���а��� �־����� �״�� 
    Vector3D(double x, double y, double z) {
        X = x;
        Y = y;
        Z = z;
    }

    ~Vector3D() {

    }

    //scalar���� ����
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

    //������ ������ - element wise
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