#pragma once

#include <cmath>
#include <exception>
#include <ostream>

template <typename T>
struct Vector2
{
	T x{};
	T y{};

	Vector2(const T x, const T y) : x(x), y(y) {}
	Vector2() { x = 0.0; y = 0.0; }

	/**
	 * @brief Returns the unit vector
	 * @return Vector2
	 */
	[[nodiscard]] Vector2 unit() const
	{
		const double magnitude = sqrt(x * x + y * y);
		return { x / magnitude, y / magnitude };
	}
	/**
	 * @brief Returns the dot product with another vector
	 * @return double
	 */
	[[nodiscard]] T dot(const Vector2& other) const
	{
		return x * other.x + y * other.y;
	}

    Vector2 operator+(const Vector2& other) const
	{
        return Vector2(x + other.x, y + other.y);
    }

    Vector2& operator+=(const Vector2& other)
	{
        x += other.x;
        y += other.y;
        return *this;
    }

    Vector2 operator-(const Vector2& other) const
	{
        return Vector2(x - other.x, y - other.y);
    }

    Vector2& operator-=(const Vector2& other)
	{
        x -= other.x;
        y -= other.y;
        return *this;
    }

    Vector2 operator*(T scalar) const
	{
        return Vector2(x * scalar, y * scalar);
    }

    Vector2& operator*=(T scalar)
	{
        x *= scalar;
        y *= scalar;
        return *this;
    }

    Vector2 operator/(T scalar) const
	{
        if (scalar == 0) 
            throw std::exception("Error: Division by zero");
        return Vector2<T>(x / scalar, y / scalar);
    }

    Vector2& operator/=(T scalar)
	{
        if (scalar == 0) 
            throw std::exception("Error: Division by zero");
        x /= scalar;
        y /= scalar;
        return *this;
    }
};

template <typename T>
std::ostream& operator<<(std::ostream& os, const Vector2<T>& v)
{
    os << "(" << v.x << ", " << v.y << ")";
    return os;
}

