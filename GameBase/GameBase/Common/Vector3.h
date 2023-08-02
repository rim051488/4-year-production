#pragma once
#include "DxLib.h"
#include <cmath>
#include "MathConcepts.h"

template<ValueC T>
class Vector3Tmp
{
public:
	T x;
	T y;
	T z;

	constexpr Vector3Tmp() :
		x{ static_cast<T>(0) }, y{ static_cast<T>(0) }, z{static_cast<T>(0)}
	{
	}

	constexpr Vector3Tmp(T inX, T inY, T inZ) :
		x{ inX }, y{ inY }, z{inZ}
	{
	}

	constexpr Vector3Tmp(const Vector3Tmp<T>& vec) :
		x{ vec.x }, y{ vec.y }, z{vec.z}
	{
	}

	T Magnitude(void) const;

	T SqMagnitude(void) const;

	void Normalize(void);

	Vector3Tmp<T> Normalized(void) const;

	/// <summary> 法線ベクトルに対しての反射ベクトルを返す </summary>
	/// <param name="n"> 法線ベクトル(正規化済み) </param>
	/// <returns> 反射ベクトル </returns>
	Vector3Tmp<T> Refrect(const Vector3Tmp<T>& n);


	template<ValueC T2>
	operator Vector3Tmp<T2>() const
	{
		return { static_cast<T2>(x), static_cast<T2>(y) };
	}

	Vector3Tmp<T> operator-() const;


	Vector3Tmp<T> operator+() const;

	constexpr Vector3Tmp<T>& operator=(const Vector3Tmp<T> vec)
	{
		x = vec.x;
		y = vec.y;
		z = vec.z;
		return *this;
	}
	constexpr Vector3Tmp<T>& operator=(const T& val)
	{
		x = val;
		y = val;
		z = val;
		return *this;
	}
	constexpr Vector3Tmp<T>& operator=(const VECTOR& vec)
	{
		x = vec.x;
		y = vec.y;
		z = vec.z;
		return *this;
	}

	Vector3Tmp<T>& operator+=(const Vector3Tmp<T>& vec);
	Vector3Tmp<T>& operator+=(const T& val);

	Vector3Tmp<T>& operator-=(const Vector3Tmp<T>& vec);
	Vector3Tmp<T>& operator-=(const T& val);

	Vector3Tmp<T>& operator*=(const Vector3Tmp<T>& vec);
	Vector3Tmp<T>& operator*=(const T& val);

	Vector3Tmp<T>& operator/=(const Vector3Tmp<T>& vec);
	Vector3Tmp<T>& operator/=(const T& val);

	Vector3Tmp<T>& operator%=(const Vector3Tmp<T>& vec);
	Vector3Tmp<T>& operator%=(const T& val);

	Vector3Tmp<T>& operator+=(const VECTOR& vec);
	Vector3Tmp<T>& operator-=(const VECTOR& vec);
	Vector3Tmp<T>& operator*=(const VECTOR& vec);
	Vector3Tmp<T>& operator/=(const VECTOR& vec);
	Vector3Tmp<T>& operator%=(const VECTOR& vec);
};

template<ValueC T>
inline T Vector3Tmp<T>::Magnitude(void) const
{
	
	return std::sqrt(std::fma(x,x, std::fma(y,y,z * z)));
}

template<ValueC T>
inline T Vector3Tmp<T>::SqMagnitude(void) const
{
	return std::fma(x, x, std::fma(y, y, z * z));
}

template<ValueC T>
inline void Vector3Tmp<T>::Normalize(void)
{
	T mag = Magnitude();
	x /= mag;
	y /= mag;
	z /= mag;
}

template<ValueC T>
inline Vector3Tmp<T> Vector3Tmp<T>::Normalized(void) const
{
	T mag = Magnitude();
	return Vector3Tmp<T>{x / mag, y / mag, z / mag};
}

template<ValueC T>
inline Vector3Tmp<T> Vector3Tmp<T>::operator-() const
{
	return Vector3Tmp<T>{-x,-y,-z};
}

template<ValueC T>
inline Vector3Tmp<T> Vector3Tmp<T>::operator+() const
{
	return Vector3Tmp<T>{+x, +y, +z };
}

template<ValueC T>
inline Vector3Tmp<T>& Vector3Tmp<T>::operator+=(const Vector3Tmp<T>& vec)
{
	x += vec.x;
	y += vec.y;
	z += vec.z;
	return *this;
}

template<ValueC T>
inline Vector3Tmp<T>& Vector3Tmp<T>::operator+=(const T& val)
{
	return { x + val, y + val, z + val};
}

template<ValueC T>
inline Vector3Tmp<T>& Vector3Tmp<T>::operator-=(const Vector3Tmp<T>& vec)
{
	x -= vec.x;
	y -= vec.y;
	z -= vec.z;
	return *this;
}

template<ValueC T>
inline Vector3Tmp<T>& Vector3Tmp<T>::operator-=(const T& val)
{
	return { x - val, y - val, z - val };
}

template<ValueC T>
inline Vector3Tmp<T>& Vector3Tmp<T>::operator*=(const Vector3Tmp<T>& vec)
{
	x *= vec.x;
	y *= vec.y;
	z *= vec.z;
	return *this;
}

//template<ValueC T>
//inline Vector3Tmp<T>& Vector3Tmp<T>::operator*=(const Vector3Tmp<T>& vec)
//{
//	return { x * vec.x, y * vec.y, z * vec.z };
//}

template<ValueC T>
inline Vector3Tmp<T>& Vector3Tmp<T>::operator*=(const T& val)
{
	return { x * val, y * val, z * val };
}

template<ValueC T>
inline Vector3Tmp<T>& Vector3Tmp<T>::operator/=(const Vector3Tmp<T>& vec)
{
	return { x / vec.x, y / vec.y, z / vec.z };
}

template<ValueC T>
inline Vector3Tmp<T>& Vector3Tmp<T>::operator/=(const T& val)
{
	return { x / val, y / val, z / val };
}

template<ValueC T>
inline Vector3Tmp<T>& Vector3Tmp<T>::operator%=(const Vector3Tmp<T>& vec)
{
	return { std::fmod(x ,vec.x), std::fmod(y , vec.y), std::fmod(z , vec.z) };
}

template<ValueC T>
inline Vector3Tmp<T>& Vector3Tmp<T>::operator%=(const T& val)
{
	return { std::fmod(x , val), std::fmod(y , val), std::fmod(z , val) };
}

template<ValueC T>
constexpr Vector3Tmp<T> operator+(const Vector3Tmp<T>& lVec, const Vector3Tmp<T>& rVec)
{
	return { lVec.x + rVec.x ,lVec.y + rVec.y, lVec.z + rVec.z };
}

template<ValueC T>
constexpr Vector3Tmp<T> operator+(const Vector3Tmp<T>& lVec, const T& val)
{
	return { lVec.x + val, lVec.y + val, lVec.z + val };
}

template<ValueC T>
constexpr Vector3Tmp<T> operator-(const Vector3Tmp<T>& lVec, const Vector3Tmp<T>& rVec)
{
	return { lVec.x - rVec.x, lVec.y - rVec.y , lVec.z - rVec.z};
}

template<ValueC T>
constexpr Vector3Tmp<T> operator-(const Vector3Tmp<T>& lVec, const T& val)
{
	return { lVec.x - val, lVec.y - val , lVec.z - val};
}

template<ValueC T>
constexpr Vector3Tmp<T> operator*(const Vector3Tmp<T>& lVec, const Vector3Tmp<T>& rVec)
{
	return { lVec.x * rVec.x, lVec.y * rVec.y, lVec.z * rVec.z };
}

template<ValueC T>
constexpr Vector3Tmp<T> operator*(const Vector3Tmp<T>& lVec, const T& val)
{
	return { lVec.x * val, lVec.y * val , lVec.z * val};
}

template<ValueC T>
constexpr Vector3Tmp<T> operator/(const Vector3Tmp<T>& lVec, const Vector3Tmp<T>& rVec)
{
	if (rVec.x == static_cast<T>(0) || rVec.y == static_cast<T>(0))
	{
		return lVec;
	}
	return { lVec.x / rVec.x, lVec.y / rVec.y , lVec.z / rVec.z};
}

template<ValueC T>
constexpr Vector3Tmp<T> operator/(const Vector3Tmp<T>& lVec, const T& val)
{
	if (val == static_cast<T>(0))
	{
		return lVec;
	}
	return { lVec.x / val, lVec.y / val , lVec.z / val};
}

template<ValueC T>
constexpr Vector3Tmp<T> operator%(const Vector3Tmp<T>& lVec, const Vector3Tmp<T>& rVec)
{
	if (rVec.x == static_cast<T>(0) || rVec.y == static_cast<T>(0) || rVec.z == static_cast<T>(0))
	{
		return lVec;
	}
	return { std::fmod(lVec.x , rVec.x), std::fmod(lVec.y , rVec.y) , std::fmod(lVec.z, rVec.z)};
}
template<ValueC T>
constexpr Vector3Tmp<T> operator%(const Vector3Tmp<T>& lVec, const T& val)
{
	if (val == static_cast<T>(0))
	{
		return lVec;
	}
	return { std::fmod(lVec.x , val), std::fmod(lVec.y , val), std::fmod(lVec.z, val)};
}

template<ValueC T>
constexpr bool operator==(const Vector3Tmp<T>& lVec, const Vector3Tmp<T>& rVec)
{
	return lVec.x == rVec.x && lVec.y == rVec.y && lVec.z == rVec.z;
}

template<ValueC T>
constexpr bool operator!=(const Vector3Tmp<T>& lVec, const Vector3Tmp<T>& rVec)
{
	return !lVec == rVec;
}

template<ValueC T>
inline Vector3Tmp<T>& Vector3Tmp<T>::operator+=(const VECTOR& vec)
{
	x += vec.x;
	y += vec.y;
	z += vec.z;
	return *this;
}

template<ValueC T>
inline Vector3Tmp<T>& Vector3Tmp<T>::operator-=(const VECTOR& vec)
{
	x -= vec.x;
	y -= vec.y;
	z -= vec.z;
	return *this;
}

template<ValueC T>
inline Vector3Tmp<T>& Vector3Tmp<T>::operator*=(const VECTOR& vec)
{
	x *= vec.x;
	y *= vec.y;
	z *= vec.z;
	return *this;
}

template<ValueC T>
inline Vector3Tmp<T>& Vector3Tmp<T>::operator/=(const VECTOR& vec)
{
	x /= vec.x;
	y /= vec.y;
	z /= vec.z;
	return *this;
}

template<ValueC T>
inline Vector3Tmp<T>& Vector3Tmp<T>::operator%=(const VECTOR& vec)
{
	x %= vec.x;
	y %= vec.y;
	z %= vec.z;
	return *this;
}

template<ValueC T>
constexpr Vector3Tmp<T> operator+(const Vector3Tmp<T>& lVec, const VECTOR& vec)
{
	return { lVec.x + vec.x, lVec.y + vec.y, lVec.z + vec.z };
}

template<ValueC T>
constexpr Vector3Tmp<T> operator-(const Vector3Tmp<T>& lVec, const VECTOR& vec)
{
	return { lVec.x - vec.x, lVec.y - vec.y, lVec.z - vec.z };
}

template<ValueC T>
constexpr Vector3Tmp<T> operator*(const Vector3Tmp<T>& lVec, const VECTOR& vec)
{
	return { lVec.x * vec.x, lVec.y * vec.y, lVec.z * vec.z };
}

template<ValueC T>
constexpr Vector3Tmp<T> operator/(const Vector3Tmp<T>& lVec, const VECTOR& vec)
{
	return { lVec.x / vec.x, lVec.y / vec.y, lVec.z / vec.z };
}

template<ValueC T>
constexpr Vector3Tmp<T> operator%(const Vector3Tmp<T>& lVec, const VECTOR& vec)
{
	return { lVec.x % vec.x, lVec.y % vec.y, lVec.z % vec.z };
}

template<ValueC T>
constexpr bool operator==(const Vector3Tmp<T>& lVec, const VECTOR& rVec)
{
	return lVec.x == rVec.x && lVec.y == rVec.y && lVec.z == rVec.z;
}

template<ValueC T>
constexpr bool operator!=(const Vector3Tmp<T>& lVec, const VECTOR& rVec)
{
	return !lVec == rVec;
}

using Vector3 = Vector3Tmp<float>;
using Vector3D = Vector3Tmp<double>;
using Vector3I = Vector3Tmp<int>;
