#pragma once

#include <cmath>
#include <exception>
#include <ostream>

template <typename T>
struct Vector2
{
    T x{};
    T y{};

    constexpr Vector2(const T x, const T y) : x(x), y(y) {}
    constexpr Vector2() : x(0.0), y(0.0) {}

    template <typename U>
    constexpr Vector2(const Vector2<U>& other) : x(static_cast<T>(other.x)), y(static_cast<T>(other.y)) {}

    [[nodiscard]]
	constexpr Vector2 unit() const
    {
        constexpr double magnitude = std::sqrt(x * x + y * y);
        return { x / magnitude, y / magnitude };
    }

    [[nodiscard]]
	constexpr T dot(const Vector2& other) const
    {
        return x * other.x + y * other.y;
    }

    constexpr Vector2 operator+(const Vector2& other) const
    {
        return Vector2(x + other.x, y + other.y);
    }

    constexpr Vector2& operator+=(const Vector2& other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    constexpr Vector2 operator-(const Vector2& other) const
    {
        return Vector2(x - other.x, y - other.y);
    }

    constexpr Vector2& operator-=(const Vector2& other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    constexpr Vector2 operator*(T scalar) const
    {
        return Vector2(x * scalar, y * scalar);
    }

    constexpr Vector2& operator*=(T scalar)
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    constexpr Vector2 operator/(T scalar) const
    {
        if (scalar == 0)
            throw std::exception("Error: Division by zero");
        return Vector2<T>(x / scalar, y / scalar);
    }

    constexpr Vector2& operator/=(T scalar)
    {
        if (scalar == 0.0f)
            throw std::exception("Error: Division by zero");
        x /= scalar;
        y /= scalar;
        return *this;
    }

    constexpr bool operator==(Vector2 other) const
    {
        return x == other.x && y == other.y;
    }

    constexpr bool operator!=(Vector2 other) const
    {
        return x != other.x && y != other.y;
    }
};

template <typename T>
std::ostream& operator<<(std::ostream& os, const Vector2<T>& v)
{
    os << "<" << v.x << ", " << v.y << ">";
    return os;
}

