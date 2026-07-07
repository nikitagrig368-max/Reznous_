#ifndef CORE_CONSTANTS_HPP
#define CORE_CONSTANTS_HPP

#include <cmath>

// ============================================================================
// Базовые константы WGS-84 и физические константы
// ============================================================================
const double WGS84_MU = 398600.4418;          // гравитационный параметр, км³/с²
const double WGS84_OMEGA_E = 7.2921151467e-5; // угловая скорость Земли, рад/с
const double WGS84_A = 6378.137;              // экваториальный радиус, км
const double WGS84_F = 1.0 / 298.257223563;   // сжатие
const double WGS84_E2 = 2.0 * WGS84_F - WGS84_F * WGS84_F; // квадрат эксцентриситета
const double WGS84_J2 = 1.08262617385222e-3;  // вторая зональная гармоника
const double R_EARTH = 6378.137;              // радиус Земли, км
const double PI = 3.14159265358979323846;
const double TWO_PI = 2.0 * PI;
const double DEG2RAD = PI / 180.0;
const double RAD2DEG = 180.0 / PI;

// ============================================================================
// Вспомогательные структуры
// ============================================================================
struct Vector3D {
    double x, y, z;

    Vector3D() : x(0), y(0), z(0) {}
    Vector3D(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}

    double norm() const {
        return std::sqrt(x*x + y*y + z*z);
    }

    double dot(const Vector3D& v) const {
        return x*v.x + y*v.y + z*v.z;
    }

    Vector3D cross(const Vector3D& v) const {
        return {
            y*v.z - z*v.y,
            z*v.x - x*v.z,
            x*v.y - y*v.x
        };
    }

    Vector3D operator+(const Vector3D& v) const {
        return {x + v.x, y + v.y, z + v.z};
    }

    Vector3D operator*(double s) const {
        return {x * s, y * s, z * s};
    }

    Vector3D operator-(const Vector3D& v) const {
        return {x - v.x, y - v.y, z - v.z};
    }

    Vector3D& operator+=(const Vector3D& v) {
        x += v.x; y += v.y; z += v.z;
        return *this;
    }

    Vector3D& operator*=(double s) {
        x *= s; y *= s; z *= s;
        return *this;
    }
};

struct KeplerianElements {
    double a;          // большая полуось, км
    double e;          // эксцентриситет
    double i_deg;      // наклонение, градусы
    double Omega_deg;  // долгота восходящего узла, градусы
    double omega_deg;  // аргумент перицентра, градусы
    double M_deg;      // средняя аномалия, градусы
};

struct Matrix3x3 {
    double m[3][3];

    Vector3D multiply(const Vector3D& v) const {
        return {
            m[0][0]*v.x + m[0][1]*v.y + m[0][2]*v.z,
            m[1][0]*v.x + m[1][1]*v.y + m[1][2]*v.z,
            m[2][0]*v.x + m[2][1]*v.y + m[2][2]*v.z
        };
    }

    static Matrix3x3 identity() {
        Matrix3x3 mat = {};
        for (int i = 0; i < 3; ++i) mat.m[i][i] = 1.0;
        return mat;
    }

    static Matrix3x3 rotation_z(double angle) {
        Matrix3x3 mat = {};
        double c = std::cos(angle), s = std::sin(angle);
        mat.m[0][0] = c; mat.m[0][1] = -s;
        mat.m[1][0] = s; mat.m[1][1] = c;
        mat.m[2][2] = 1.0;
        return mat;
    }

    static Matrix3x3 rotation_x(double angle) {
        Matrix3x3 mat = {};
        double c = std::cos(angle), s = std::sin(angle);
        mat.m[0][0] = 1.0;
        mat.m[1][1] = c; mat.m[1][2] = -s;
        mat.m[2][1] = s; mat.m[2][2] = c;
        return mat;
    }
};

#endif // CORE_CONSTANTS_HPP
