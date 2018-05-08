#pragma once

#include <algorithm>
#include <stdexcept>

#ifndef _WIN32
#include <cmath>
#endif

namespace math
{
// Move this to some math utils file
template <typename T>
constexpr T clamp(const T x, const T min, const T max)
{
#undef min
#undef max
    return std::min(max, std::max(min, x));
}

// Lerps between two values depending on the weight
template <class T>
T lerp(const T& from, float fAlpha, const T& to)
{
    return (T)((to - from) * fAlpha + from);
}

// Find the relative position of Pos between From and To
inline const float unlerp(const double dFrom, const double dPos, const double dTo)
{
    // Avoid dividing by 0 (results in INF values)
    if (dFrom == dTo)
        return 1.0f;

    return static_cast<float>((dPos - dFrom) / (dTo - dFrom));
}

// Unlerp avoiding extrapolation
inline const float unlerpClamped(const double dFrom, const double dPos, const double dTo)
{
    return clamp(0.0f, unlerp(dFrom, dPos, dTo), 1.0f);
}

template <typename T>
class basic_vector2
{
private:
    T _x = 0, _y = 0;

public:
    basic_vector2(T x_, T y_) : _x(x_), _y(y_) {}

    basic_vector2() : basic_vector2(T(0), T(0)) {}

    T x() const { return _x; }

    T y() const { return _y; }

    void SetX(T value) { _x = value; }

    void SetY(T value) { _y = value; }

    T& operator[](size_t i)
    {
        if (i == 0)
            return this->_x;
        else if (i == 1)
            return this->_y;

        throw std::out_of_range("Maximum is 2");
    }

    basic_vector2 operator-(const basic_vector2& right) const
    {
        return basic_vector2{this->_x - right._x, this->_y - right._y};
    }

    basic_vector2 operator-(const T right) const { return basic_vector2{this->_x - right, this->_y - right}; }

    basic_vector2 operator+(const basic_vector2& right) const
    {
        return basic_vector2{this->_x + right._x, this->_y + right._y};
    }

    basic_vector2 operator+(const T right) const { return basic_vector2{this->_x + right, this->_y + right}; }

    basic_vector2 operator*(const basic_vector2& right) const
    {
        return basic_vector2{this->_x * right._x, this->_y * right._y};
    }

    basic_vector2 operator*(const T right) const { return basic_vector2{this->_x * right, this->_y * right}; }

    basic_vector2 operator/(const basic_vector2& right) const
    {
        return basic_vector2{this->_x / right._x, this->_y / right._y};
    }

    basic_vector2 operator/(const T right) const { return basic_vector2{this->_x / right, this->_y / right}; }

    basic_vector2& operator/=(const T rigth) const
    {
        this->_x /= right;
        this->_y /= right;
        return *this;
    }

    bool operator>(const basic_vector2& other) const { return this->_x > other._x && this->_y > other._y; }
    bool operator>=(const basic_vector2& other) const { return this->_x >= other._x && this->_y >= other._y; }

    bool operator<(const basic_vector2& other) const { return this->_x < other._x && this->_y < other._y; }

    bool operator<=(const basic_vector2& other) const { return this->_x <= other._x && this->_y <= other._y; }

    bool operator==(const basic_vector2& other) const { return this->_x == other._x && this->_y == other._y; }

    bool operator!=(const basic_vector2& other) const { return this->_x != other._x || this->_y != other._y; }

    basic_vector2& operator+=(const basic_vector2& other)
    {
        this->_x += other._x;
        this->_y += other._y;

        return *this;
    }

    basic_vector2& operator+=(const T other)
    {
        this->_x += other;
        this->_y += other;

        return *this;
    }

    basic_vector2& operator-=(const basic_vector2& other)
    {
        this->_x -= other._x;
        this->_y -= other._y;

        return *this;
    }

    basic_vector2& operator-=(const T other)
    {
        this->_x -= other;
        this->_y -= other;

        return *this;
    }

    basic_vector2& operator*=(const basic_vector2& other)
    {
        this->_x *= other._x;
        this->_y *= other._y;

        return *this;
    }

    basic_vector2& operator*=(const T other)
    {
        this->_x *= other;
        this->_y *= other;

        return *this;
    }

    basic_vector2& operator/=(const basic_vector2& other)
    {
        this->_x /= other._x;
        this->_y /= other._y;

        return *this;
    }

    basic_vector2& operator/=(const T other)
    {
        this->_x /= other;
        this->_y /= other;

        return *this;
    }

    basic_vector2& operator=(const basic_vector2& other)
    {
        this->_x = other._x;
        this->_y = other._y;

        return *this;
    }

    double length() const { return std::sqrt(this->_x * this->_x + this->_y * this->_y); }

    /* Returns the length of the vector */
    T magnitude() const { return length(); }

    T sqrMagnitude() const { return this->_x * this->_x + this->_y * this->_y; }

    void normalize() { this->operator/=(this->magnitude()); }

    basic_vector2 normalized() const { return this->operator/(this->magnitude()); }

    T dot(const basic_vector2& otr) { return this->_x * otr._x + this->_y * otr._y; }

    T angle(const basic_vector2& to) { return angle(*this, to); }

    static T angle(const basic_vector2& from, const basic_vector2& to) { return 0.0f; }

    basic_vector2 lerp(const basic_vector2& to, float t) { return lerp(*this, to, t); }

    static basic_vector2 lerp(basic_vector2 from, basic_vector2 to, float t) { return (from + t * (to - from)); }

    basic_vector2 slerp(const basic_vector2& to, float t) { return slerp(*this, to, t); }

    static basic_vector2 slerp(const basic_vector2& from, const basic_vector2& to, float t)
    {
        // Dot product - the cosine of the angle between 2 vectors.
        float dot = from.dot(to);
        // Clamp it to be in the range of Acos()
        // This may be unnecessary, but floating point
        // precision can be a fickle mistress.
        dot = clamp(dot, -1.0f, 1.0f);
        // Acos(dot) returns the angle between start and end,
        // And multiplying that by percent returns the angle between
        // start and the final result.

        float theta = std::acos(dot) * t;
        basic_vector2 RelativeVec = to - from * dot;
        RelativeVec.normalize();  // Orthonormal basis
                                  // The final result.
        return ((from * std::cos(theta)) + (RelativeVec * std::sin(theta)));
    }

    basic_vector2 nlerp(const basic_vector2& to, float t) { return nlerp(*this, to, t); }

    static basic_vector2 nlerp(basic_vector2 start, basic_vector2 end, float percent)
    {
        return lerp(start, end, percent).normalized();
    }

    const static basic_vector2 one;
    const static basic_vector2 right;
    const static basic_vector2 up;
    const static basic_vector2 zero;
};

template <typename T>
const basic_vector2<T> basic_vector2<T>::one = basic_vector2<T>{1, 1};
template <typename T>
const basic_vector2<T> basic_vector2<T>::right = basic_vector2<T>{0, 1};
template <typename T>
const basic_vector2<T> basic_vector2<T>::up = basic_vector2<T>{1, 0};
template <typename T>
const basic_vector2<T> basic_vector2<T>::zero = basic_vector2<T>{0, 0};

template <typename T>
class basic_vector3
{
public:
    T _x = 0, _y = 0, _z = 0;

    basic_vector3() : basic_vector3(T(0), T(0), T(0)) {}

    basic_vector3(T x_, T y_, T z_) : _x(x_), _y(y_), _z(z_) {}

    T x() const { return _x; }

    T y() const { return _y; }

    T z() const { return _z; }

    T& operator[](size_t i)
    {
        if (i == 0)
            return this->_x;
        else if (i == 1)
            return this->_y;
        else if (i == 2)
            return this->_z;

        throw std::out_of_range("Maximum is 3");
    }

    void SetX(T value) { _x = value; }

    void SetY(T value) { _y = value; }

    void SetZ(T value) { _z = value; }

    basic_vector3 operator-(const basic_vector3& right) const
    {
        return basic_vector3{this->_x - right._x, this->_y - right._y, this->_z - right._z};
    }

    basic_vector3 operator-(const T right) const
    {
        return basic_vector3{this->_x - right, this->_y - right, this->_z - right};
    }

    basic_vector3 operator+(const basic_vector3& right) const
    {
        return basic_vector3{this->_x + right._x, this->_y + right._y, this->_z + right._z};
    }

    basic_vector3 operator+(const T right) const
    {
        return basic_vector3{this->_x + right, this->_y + right, this->_z + right};
    }

    basic_vector3 operator*(const basic_vector3& right) const
    {
        return basic_vector3{this->_x * right._x, this->_y * right._y, this->_z * right._z};
    }

    basic_vector3 operator*(const T right) const
    {
        return basic_vector3{this->_x * right, this->_y * right, this->_z * right};
    }

    basic_vector3 operator/(const basic_vector3& right) const
    {
        return basic_vector3{this->_x / right._x, this->_y / right._y, this->_z / right._z};
    }

    basic_vector3 operator/(const T right) const
    {
        return basic_vector3{this->_x / right, this->_y / right, this->_z / right};
    }

    basic_vector3 operator%(T right) const
    {
        return basic_vector3{fmodf(this->_x, right), fmodf(this->_y, right), fmodf(this->_z, right)};
    }

    bool operator>(const basic_vector3& other) const
    {
        return this->_x > other._x && this->_y > other._y && this->_z > other._z;
    }
    bool operator>=(const basic_vector3& other) const
    {
        return this->_x >= other._x && this->_y >= other._y && this->_z >= other._z;
    }

    bool operator<(const basic_vector3& other) const
    {
        return this->_x < other._x && this->_y < other._y && this->_z < other._z;
    }

    bool operator<=(const basic_vector3& other) const
    {
        return this->_x <= other._x && this->_y <= other._y && this->_z <= other._z;
    }

    bool operator==(const basic_vector3& other) const
    {
        return this->_x == other._x && this->_y == other._y && this->_z == other._z;
    }

    bool operator!=(const basic_vector3& other) const
    {
        return this->_x != other._x || this->_y != other._y || this->_z != other._z;
    }

    basic_vector3& operator+=(const basic_vector3& other)
    {
        this->_x += other._x;
        this->_y += other._y;
        this->_z += other._z;

        return *this;
    }

    basic_vector3& operator+=(const T other)
    {
        this->_x += other;
        this->_y += other;
        this->_z += other;

        return *this;
    }

    basic_vector3& operator-=(const basic_vector3& other)
    {
        this->_x -= other._x;
        this->_y -= other._y;
        this->_z -= other._z;

        return *this;
    }

    basic_vector3& operator-=(const T other)
    {
        this->_x -= other;
        this->_y -= other;
        this->_z -= other;

        return *this;
    }

    basic_vector3& operator*=(const basic_vector3& other)
    {
        this->_x /= other._x;
        this->_y /= other._y;
        this->_z /= other._z;

        return *this;
    }

    basic_vector3& operator*=(const T other)
    {
        this->_x *= other;
        this->_y *= other;
        this->_z *= other;

        return *this;
    }

    basic_vector3& operator/=(const basic_vector3& other)
    {
        this->_x /= other._x;
        this->_y /= other._y;
        this->_z /= other._z;

        return *this;
    }

    basic_vector3& operator/=(const T other)
    {
        this->_x /= other;
        this->_y /= other;
        this->_z /= other;

        return *this;
    }

    basic_vector3& operator%=(const T other)
    {
        this->_x %= other;
        this->_y %= other;
        this->_z %= other;

        return *this;
    }

    basic_vector3& operator=(const basic_vector3& other)
    {
        this->_x = other._x;
        this->_y = other._y;
        this->_z = other._z;

        return *this;
    }

    double length() const { return std::sqrt(this->_x * this->_x + this->_y * this->_y + this->_z * this->_z); }

    /* Returns the length of the vector */
    T magnitude() const { return length(); }

    T sqrMagnitude() const { return this->_x * this->_x + this->_y * this->_y + this->_z * this->_z; }

    void normalize() { this->operator/=(this->magnitude()); }

    basic_vector3 normalized() const { return this->operator/(this->magnitude()); }

    T dot(const basic_vector3& otr) { return this->_x * otr._x + this->_y * otr._y + this->_z * otr._z; }

    T angle(const basic_vector3& to) { return angle(*this, to); }

    static T angle(const basic_vector3& from, const basic_vector3& to) { return 0.0f; }

    // Or should we assign it to self?
    basic_vector3 lerp(const basic_vector3& to, float t) { return lerp(*this, to, t); }

    static basic_vector3 lerp(const basic_vector3& from, const basic_vector3& to, float t)
    {
        return (from + t * (to - from));
        ;
    }

    basic_vector3 slerp(const basic_vector3& to, float t) { return slerp(*this, to, t); }

    static basic_vector3 slerp(const basic_vector3& from, const basic_vector3& to, float t)
    {
        // Dot product - the cosine of the angle between 2 vectors.
        float dot = from.dot(to);
        // Clamp it to be in the range of Acos()
        // This may be unnecessary, but floating point
        // precision can be a fickle mistress.
        dot = clamp(dot, -1.0f, 1.0f);
        // Acos(dot) returns the angle between start and end,
        // And multiplying that by percent returns the angle between
        // start and the final result.

        float theta = std::acos(dot) * t;
        basic_vector3 RelativeVec = to - from * dot;
        RelativeVec.normalize();  // Orthonormal basis
                                  // The final result.
        return ((from * std::cos(theta)) + (RelativeVec * std::sin(theta)));
    }

    basic_vector3 nlerp(const basic_vector3& to, float t) { return nlerp(*this, to, t); }

    static basic_vector3 nlerp(const basic_vector3& start, const basic_vector3& end, float percent)
    {
        return lerp(start, end, percent).normalized();
    }

    const static basic_vector3 back;
    const static basic_vector3 down;
    const static basic_vector3 forward;
    const static basic_vector3 left;

    const static basic_vector3 one;
    const static basic_vector3 right;
    const static basic_vector3 up;
    const static basic_vector3 zero;
};

template <typename T>
class basic_vector4
{
public:
    T x = 0;
    T y = 0;
    T z = 0;
    T a = 0;

    basic_vector4() = default;
    basic_vector4(T _x, T _y, T _z, T _a)
    {
        x = _x;
        y = _y;
        z = _z;
        a = _a;
    }

    T& operator[](size_t i)
    {
        if (i == 0)
            return this->x;
        else if (i == 1)
            return this->y;
        else if (i == 2)
            return this->z;
        else if (i == 3)
            return this->a;

        throw std::out_of_range("Maximum is 4");
    }

    double length() { return std::sqrt(this->x * this->x + this->y * this->y + this->z * this->z + this->a * this->a); }
};

template <typename T>
const basic_vector3<T> basic_vector3<T>::back = basic_vector3<T>{0, 0, -1};
template <typename T>
const basic_vector3<T> basic_vector3<T>::down = basic_vector3<T>{0, -1, 0};
template <typename T>
const basic_vector3<T> basic_vector3<T>::forward = basic_vector3<T>{0, 0, 1};
template <typename T>
const basic_vector3<T> basic_vector3<T>::left = basic_vector3<T>{-1, 0, 0};

template <typename T>
const basic_vector3<T> basic_vector3<T>::one = basic_vector3<T>{1, 1, 1};
template <typename T>
const basic_vector3<T> basic_vector3<T>::right = basic_vector3<T>{1, 0, 0};
template <typename T>
const basic_vector3<T> basic_vector3<T>::up = basic_vector3<T>{0, 1, 0};
template <typename T>
const basic_vector3<T> basic_vector3<T>::zero = basic_vector3<T>{0, 0, 0};

using Vector2 = basic_vector2<int32_t>;
using Vector3 = basic_vector3<int32_t>;
using Vector4 = basic_vector4<int32_t>;

using Vector2f = basic_vector2<float>;
using Vector3f = basic_vector3<float>;
using Vector4f = basic_vector4<float>;

template <typename T>
basic_vector3<T> operator*(const T left, basic_vector3<T> right)
{
    return right * left;
}

using Vector2by = basic_vector2<uint8_t>;
using Vector3by = basic_vector3<uint8_t>;
using Vector4by = basic_vector4<uint8_t>;

using Vector2bo = basic_vector2<bool>;
using Vector3bo = basic_vector3<bool>;
using Vector4bo = basic_vector4<bool>;

inline Vector3f RadianToRotation(math::Vector3f r)
{
    float x = (r.x() * (180.0f / 3.14159265358979323846f));
    float y = (r.y() * (180.0f / 3.14159265358979323846f));
    float z = (r.z() * (180.0f / 3.14159265358979323846f));
    return Vector3f{x, y, z};
}

inline Vector3f RotationToRadian(math::Vector3f r)
{
    float x = (r.x() * (3.14159265358979323846f / 180.0f));
    float y = (r.y() * (3.14159265358979323846f / 180.0f));
    float z = (r.z() * (3.14159265358979323846f / 180.0f));
    return Vector3f{x, y, z};
}
}  // namespace math
