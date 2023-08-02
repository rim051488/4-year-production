#pragma once
#include <algorithm>
#include "Vector2.h"
#include "Vector3.h"
#include "Quaternion.h"


// Vector2Tmpのx=0,y=0を表す定数
template<ValueC T>
constexpr Vector2Tmp<T> zeroVector2{ static_cast<T>(0),static_cast<T>(0) };

// Vector2Tmpの上方向x=0,y=-1を表す定数
template<ValueC T>
constexpr Vector2Tmp<T> upVector2{ static_cast<T>(0),static_cast<T>(-1) };

// Vector2Tmpの下方向x=0,y=1を表す定数
template<ValueC T>
constexpr Vector2Tmp<T> downVector2{ static_cast<T>(0),static_cast<T>(1) };

// Vector2Tmpの右方向x=1,y=0を表す定数
template<ValueC T>
constexpr Vector2Tmp<T> rightVector2{ static_cast<T>(1),static_cast<T>(0) };

// Vector2Tmpの上方向x=-1,y=0を表す定数
template<ValueC T>
constexpr Vector2Tmp<T> leftVector2{ static_cast<T>(-1),static_cast<T>(0) };

// Vector3Tmpのx=0,y=0,z=0を表す定数
template<ValueC T>
constexpr Vector3Tmp<T> zeroVector3{ static_cast<T>(0),static_cast<T>(0) ,static_cast<T>(0) };

// Vector3Tmpの上方向を表す定数
template<ValueC T>
constexpr Vector3Tmp<T> upVector3{ static_cast<T>(0),static_cast<T>(1) ,static_cast<T>(0) };

// Vector3Tmpの下方向を表す定数
template<ValueC T>
constexpr Vector3Tmp<T> downVector3{ static_cast<T>(0),static_cast<T>(-1) ,static_cast<T>(0) };

// Vector3Tmpの前方方向を表す定数
template<ValueC T>
constexpr Vector3Tmp<T> forwardVector3{ static_cast<T>(0),static_cast<T>(0) ,static_cast<T>(1) };

// Vector3Tmpの後方方向を表す定数
template<ValueC T>
constexpr Vector3Tmp<T> backVector3{ static_cast<T>(0),static_cast<T>(0) ,static_cast<T>(-1) };

// Vector3Tmpの左方向を表す定数
template<ValueC T>
constexpr Vector3Tmp<T> leftVector3{ static_cast<T>(-1),static_cast<T>(0) ,static_cast<T>(0) };

// Vector3Tmpの右方向を表す定数
template<ValueC T>
constexpr Vector3Tmp<T> rightVector3{ static_cast<T>(1),static_cast<T>(0) ,static_cast<T>(0) };

/// <summary>
/// 二乗する
/// </summary>
/// <param name="val"> 二乗したい値 </param>
/// <returns> 二乗した値 </returns>
template<ValueC T>
constexpr T Square(T val)
{
	return val * val;
}

/// <summary>
/// 内積(Vector2用)
/// </summary>
/// <param name="lVec"></param>
/// <param name="rVec"></param>
/// <returns></returns>
template<ValueC T>
constexpr T Dot(const Vector2Tmp<T>& lVec, const Vector2Tmp<T>& rVec)
{
	return lVec.x * rVec.x + lVec.y * rVec.y;
}

/// <summary>
/// 内積(Vector3用)
/// </summary>
/// <param name="lVec"></param>
/// <param name="rVec"></param>
/// <returns></returns>
template<ValueC T>
constexpr T Dot(const Vector3Tmp<T>& lVec, const Vector3Tmp<T>& rVec)
{
	return lVec.x * rVec.x + lVec.y * rVec.y + lVec.z + rVec.z;
}

/// <summary>
/// 外積(Vector2用)
/// </summary>
/// <param name="lVec"></param>
/// <param name="rVec"></param>
/// <returns></returns>
template<ValueC T>
constexpr T Cross(const Vector2Tmp<T>& lVec, const Vector2Tmp<T>& rVec)
{
	return lVec.x * rVec.y - lVec.y * rVec.x;
}


template<ValueC T>
constexpr Vector3Tmp<T> Cross(const Vector3Tmp<T>& lVec, const Vector3Tmp<T>& rVec)
{
	return { std::fma(lVec.y, rVec.z,-(lVec.z * rVec.y)),std::fma(lVec.z , rVec.x,-(lVec.x * rVec.z)), std::fma(lVec.x, rVec.y, -(lVec.y * rVec.x)) };
}

// 円周率
template<ValueC T>
constexpr T pi = static_cast<T>(3.1415926535897932384626433832795);

// 円周率*2
template<ValueC T>
constexpr T pi2 = pi<T> * static_cast<T>(2.0);

/// <summary>
/// 度数法を弧度法にする
/// </summary>
/// <param name="deg"> 変換したい度数法の値 </param>
/// <returns>  </returns>
template<ValueC T>
constexpr T Deg2Rad(const T& deg)
{
	return deg * (pi<T> / static_cast<T>(180));
}

/// <summary>
/// 弧度法を度数法にする
/// </summary>
/// <param name="rad"></param>
/// <returns></returns>
template<ValueC T>
constexpr T Rad2Deg(const T& rad)
{
	return rad * (static_cast<T>(180) / pi<T>);
}

/// <summary>
/// 角度を指定範囲に正規化する
/// </summary>
/// <param name="angle"> 変換する値 </param>
/// <param name="valMin"> 最小値(デフォルト=0) </param>
/// <param name="valMax"> 最大値(デフォルト=pi2) </param>
/// <returns></returns>
float GetNormalizedAngle(float angle, float valMin = 0.0f, float valMax = pi2<float>);

double GetNormalizedAngle(double angle, double valMin = 0.0f, double valMax = pi2<double>);

float DirNearAroundRad(float from, float to);

MATRIX ToMatrix(Quaternion& q);

Quaternion ToQuaternion(MATRIX& m);
Quaternion ToQuaternion(Vector3& e);

Vector3 ToEuler(MATRIX& m);
Vector3 ToEuler(Quaternion& q);

template<class T>
constexpr T RadIn2PI(const T& rad)
{
	T tmp = std::fmod(rad, pi<T> *static_cast<T>(2));
	if (tmp < static_cast<T>(0))
	{
		tmp += pi<T> *static_cast<T>(2);
	}
	return tmp;
}
	
/// <summary>
/// クランプする
/// </summary>
/// <param name="val"> 対象の値 </param>
/// <param name="minVal"> 最小値(デフォルト0) </param>
/// <param name="maxVal"> 最大値(デフォルト1) </param>
/// <returns></returns>
constexpr float Clamp(float val, float minVal = 0, float maxVal = 1)
{
	return std::clamp(val,minVal,maxVal);
}

/// <summary> Vector2とVector2の間の角度を求める </summary>
/// <param name="v1"></param>
/// <param name="v2"></param>
/// <returns></returns>
template<ValueC T>
T GetAngle2Vector(const Vector2Tmp<T>& v1, const Vector2Tmp<T>& v2)
{
	auto tmpv1 = v1.Normalized();
	auto tmpv2 = v2.Normalized();
	auto dot = Dot(tmpv1, tmpv2);
	auto cross = Cross(tmpv1, tmpv2);
	auto angle = atan2(cross, dot);
	if (angle < 0)
	{
		angle += pi<float> *2.0f;
	}
	return angle;
}



/// <summary> aとbをtで線形補完する関数 </summary>
/// <param name="a">  </param>
/// <param name="b"></param>
/// <param name="t">　1.0f から0.0fの値　</param>
/// <returns></returns>
template<ValueC T>
constexpr T Learp(const T& a, const T& b, const T& t)
{
	return a +  ((b - a) * t);
}

/// <summary> ベクトルaとベクトルbをtで線形補完する関数 </summary>
/// <param name="a">  </param>
/// <param name="b"></param>
/// <param name="t"> 1.0f から0.0fの値 </param>
/// <returns></returns>
template<ValueC T>
constexpr Vector2Tmp<T> Learp(const Vector2Tmp<T>& a, const Vector2Tmp<T>& b, const T& t)
{
	return a + ((b - a) * t) ;
}

template<ValueC T>
constexpr Vector3Tmp<T> Learp(const Vector3Tmp<T>& a, const Vector3Tmp<T>& b, const T& t)
{
	return a + ((b - a) * t);
}


/// <summary> Vector2TmpをClampする </summary>
/// <param name="val"> Clampしたい値 </param>
/// <param name="min"> 最小値 </param>
/// <param name="max"> 最大値 </param>
/// <returns> Clamp後の値 </returns>
template<ValueC T>
constexpr Vector2Tmp<T> Clamp(const Vector2Tmp<T>& val, const Vector2Tmp<T>& min, const Vector2Tmp<T>& max)
{
	return { std::clamp(val.x,min.x,max.x),std::clamp(val.y,min.y,max.y) };
}

// <summary> Vector3TmpをClampする </summary>
/// <param name="val"> Clampしたい値 </param>
/// <param name="min"> 最小値 </param>
/// <param name="max"> 最大値 </param>
/// <returns> Clamp後の値 </returns>
template<ValueC T>
constexpr Vector3Tmp<T> Clamp(const Vector3Tmp<T>& val, const Vector3Tmp<T>& min, const Vector3Tmp<T>& max)
{
	return { std::clamp(val.x,min.x,max.x),std::clamp(val.y,min.y,max.y),std::clamp(val.z,min.z,max.z) };
}

