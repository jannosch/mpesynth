#ifndef VECTOR_H
#define VECTOR_H

template <typename T>
class Vec2 {
public:

    T x;
    T y;

    Vec2() : x(0), y(0) { }
    Vec2(const T _x, const T _y) : x(_x), y(_y) { }

    [[nodiscard]] inline Vec2 add(const Vec2& b) const { return {x + b.x, y + b.y}; }
    [[nodiscard]] inline Vec2 sub(const Vec2& b) const { return {x - b.x, y - b.y}; }
    [[nodiscard]] inline Vec2 mul(const T f) const { return {x * f, y * f}; }
    [[nodiscard]] inline Vec2 div(const T f) const { return {x / f, y / f}; }

    Vec2 operator+(const Vec2& b) const { return add(b); }
    Vec2 operator-(const Vec2& b) const { return sub(b); }
    Vec2 operator*(const T f) const { return mul(f); }
    Vec2 operator/(const T f) const { return div(f); }

    Vec2& operator=(const Vec2& b) { x=b.x; y=b.y; return *this; }
};

#endif //VECTOR_H
