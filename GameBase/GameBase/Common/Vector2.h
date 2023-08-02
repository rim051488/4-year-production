#pragma once
#include <cmath>
#include "MathConcepts.h"




template<ValueC T>
class Vector2Tmp
{
public:
	T x;
	T y;


	constexpr Vector2Tmp() :
		x{ static_cast<T>(0) }, y{ static_cast<T>(0) }
	{
	}

	constexpr Vector2Tmp(T inX, T inY) :
		x{ inX }, y{ inY }
	{
	}

	constexpr Vector2Tmp(const Vector2Tmp<T>& vec) :
		x{ vec.x }, y{ vec.y }
	{
	}

	T Magnitude(void) const;

	T SqMagnitude(void) const;

	void Normalize(void);

	Vector2Tmp<T> Normalized(void) const;
	
	/// <summary> 法線ベクトルに対しての反射ベクトルを返す </summary>
	/// <param name="n"> 法線ベクトル(正規化済み) </param>
	/// <returns> 反射ベクトル </returns>
	Vector2Tmp<T> Refrect(const Vector2Tmp<T>& n);
	
	/// <summary> ベクトルの角度を求める </summary>
	/// <param name=""></param>
	/// <returns> 角度(弧度法) </returns>
	T GetAngle(void) const;

	/// <summary> ベクトルを回転させる </summary>
	/// <param name="angle"> 回転させる角度(弧度法) </param>
	/// <returns></returns>
	void Rota(T angle);

	template<ValueC T2>
	operator Vector2Tmp<T2>() const
	{
		return {static_cast<T2>(x), static_cast<T2>(y)};
	}

	Vector2Tmp<T> operator-() const;
		

	Vector2Tmp<T> operator+() const;

	constexpr Vector2Tmp<T>& operator=(const Vector2Tmp<T> vec)
	{
		x = vec.x;
		y = vec.y;
		return *this;
	}
	constexpr Vector2Tmp<T>& operator=(const T& val)
	{
		x = val;
		y = val;
		return *this;
	}

	Vector2Tmp<T>& operator+=(const Vector2Tmp<T>& vec);
	Vector2Tmp<T>& operator+=(const T& val);

	Vector2Tmp<T>& operator-=(const Vector2Tmp<T>& vec);
	Vector2Tmp<T>& operator-=(const T& val);

	Vector2Tmp<T>& operator*=(const Vector2Tmp<T>& vec);
	Vector2Tmp<T>& operator*=(const T& val);

	Vector2Tmp<T>& operator/=(const Vector2Tmp<T>& vec);
	Vector2Tmp<T>& operator/=(const T& val);

	Vector2Tmp<T>& operator%=(const Vector2Tmp<T>& vec);
	Vector2Tmp<T>& operator%=(const T& val);
};

template<ValueC T>
constexpr Vector2Tmp<T> operator+(const Vector2Tmp<T>& lVec, const Vector2Tmp<T>& rVec)
{
	return { lVec.x + rVec.x ,lVec.y + rVec.y };
}

template<ValueC T>
constexpr Vector2Tmp<T> operator+(const Vector2Tmp<T>& lVec, const T& val)
{
	return { lVec.x + val, lVec.y + val };
}

template<ValueC T>
constexpr Vector2Tmp<T> operator-(const Vector2Tmp<T>& lVec, const Vector2Tmp<T>& rVec)
{
	return { lVec.x - rVec.x, lVec.y - rVec.y };
}

template<ValueC T>
constexpr Vector2Tmp<T> operator-(const Vector2Tmp<T>& lVec, const T& val)
{
	return { lVec.x - val, lVec.y - val };
}

template<ValueC T>
constexpr Vector2Tmp<T> operator*(const Vector2Tmp<T>& lVec, const Vector2Tmp<T>& rVec)
{
	return { lVec.x * rVec.x, lVec.y * rVec.y };
}

template<ValueC T>
constexpr Vector2Tmp<T> operator*(const Vector2Tmp<T>& lVec, const T& val)
{
	return { lVec.x * val, lVec.y * val };
}

template<ValueC T>
constexpr Vector2Tmp<T> operator/(const Vector2Tmp<T>& lVec, const Vector2Tmp<T>& rVec)
{
	if (rVec.x == static_cast<T>(0) || rVec.y == static_cast<T>(0))
	{
		return lVec;
	}
	return { lVec.x / rVec.x, lVec.y / rVec.y };
}

template<ValueC T>
constexpr Vector2Tmp<T> operator/(const Vector2Tmp<T>& lVec, const T& val)
{
	if (val == static_cast<T>(0))
	{
		return lVec;
	}
	return { lVec.x / val, lVec.y / val };
}

template<ValueC T>
constexpr Vector2Tmp<T> operator%(const Vector2Tmp<T>& lVec, const Vector2Tmp<T>& rVec)
{
	if (rVec.x == static_cast<T>(0) || rVec.y == static_cast<T>(0))
	{
		return lVec;
	}
	return { std::fmod(lVec.x , rVec.x), std::fmod(lVec.y , rVec.y) };
}
template<ValueC T>
constexpr Vector2Tmp<T> operator%(const Vector2Tmp<T>& lVec, const T& val)
{
	if (val == static_cast<T>(0))
	{
		return lVec;
	}
	return { std::fmod(lVec.x , val), std::fmod(lVec.y , val) };
}

template<ValueC T>
constexpr bool operator==(const Vector2Tmp<T>& lVec, const Vector2Tmp<T>& rVec)
{
	return lVec.x == rVec.x && lVec.y == rVec.y;
}

template<ValueC T>
constexpr bool operator!=(const Vector2Tmp<T>& lVec, const Vector2Tmp<T>& rVec)
{
	return !lVec == rVec;
}


template<ValueC T>
inline T Vector2Tmp<T>::Magnitude(void) const
{
	return std::sqrt(x * x + y * y);
}

template<ValueC T>
inline T Vector2Tmp<T>::SqMagnitude(void) const
{
	return x * x + y * y;
}

template<ValueC T>
inline void Vector2Tmp<T>::Normalize(void)
{
	T mag = std::sqrt(x * x + y * y);
	if (mag == static_cast<T>(0))
	{
		return ;
	}
	x /= mag;
	y /= mag;
}

template<ValueC T>
inline Vector2Tmp<T> Vector2Tmp<T>::Normalized(void) const
{
	T mag = std::sqrt(x * x + y * y);
	return { x / mag, y / mag };
}

template<ValueC T>
inline Vector2Tmp<T> Vector2Tmp<T>::Refrect(const Vector2Tmp<T>& n)
{
	return (*this) - (n * (static_cast<T>(2) * (x * n.x + y * n.y)));
}

template<ValueC T>
inline T Vector2Tmp<T>::GetAngle(void) const
{
	if (*this == Vector2Tmp<T>{0, 0})
	{
		return static_cast<T>(0);
	}
	return std::atan2(y, x);
}

template<ValueC T>
inline void Vector2Tmp<T>::Rota(T angle)
{
	*this = Vector2Tmp<T>{ x * std::cos(angle) - y * std::sin(angle),x * std::sin(angle) + y * std::cos(angle) };
}

template<ValueC T>
inline Vector2Tmp<T> Vector2Tmp<T>::operator-() const
{
	return { -x,-y };
}

template<ValueC T>
inline Vector2Tmp<T> Vector2Tmp<T>::operator+() const
{
	return { +x,+y };
}

template<ValueC T>
inline Vector2Tmp<T>& Vector2Tmp<T>::operator+=(const Vector2Tmp<T>& vec)
{
	x += vec.x;
	y += vec.y;
	return *this;
}

template<ValueC T>
inline Vector2Tmp<T>& Vector2Tmp<T>::operator+=(const T& val)
{
	x += val;
	y += val;
	return *this;
}

template<ValueC T>
inline Vector2Tmp<T>& Vector2Tmp<T>::operator-=(const Vector2Tmp<T>& vec)
{
	x -= vec.x;
	y -= vec.y;
	return *this;
}

template<ValueC T>
inline Vector2Tmp<T>& Vector2Tmp<T>::operator-=(const T& val)
{
	x -= val;
	y -= val;
	return *this;
}

template<ValueC T>
inline Vector2Tmp<T>& Vector2Tmp<T>::operator*=(const Vector2Tmp<T>& vec)
{
	x *= vec.x;
	y *= vec.y;
	return *this;
}

template<ValueC T>
inline Vector2Tmp<T>& Vector2Tmp<T>::operator*=(const T& val)
{
	x *= val;
	y *= val;
	return *this;
}

template<ValueC T>
inline Vector2Tmp<T>& Vector2Tmp<T>::operator/=(const Vector2Tmp<T>& vec)
{
	if (vec.x == static_cast<T>(0) || vec.y == static_cast<T>(0))
	{
		return *this;
	}
	x /= vec.x;
	y /= vec.y;
	return *this;
}

template<ValueC T>
inline Vector2Tmp<T>& Vector2Tmp<T>::operator/=(const T& val)
{
	if (val == static_cast<T>(0))
	{
		return *this;
	}
	x /= val;
	y /= val;
	return *this;
}

template<ValueC T>
inline Vector2Tmp<T>& Vector2Tmp<T>::operator%=(const Vector2Tmp<T>& vec)
{
	if (vec.x == static_cast<T>(0) || vec.y == static_cast<T>(0))
	{
		return *this;
	}
	x = static_cast<T>(std::fmod(x, vec, x));
	y = static_cast<T>(std::fmod(y, vec.y));
	return *this;
}

template<ValueC T>
inline Vector2Tmp<T>& Vector2Tmp<T>::operator%=(const T& val)
{
	if (val == static_cast<T>(0))
	{
		return *this;
	}
	x = static_cast<T>(std::fmod(x, val));
	y = static_cast<T>(std::fmod(y, val));
	return *this;
}

using Vector2 = Vector2Tmp<float>;
using Vector2D = Vector2Tmp<double>;
using Vector2I = Vector2Tmp<int>;

