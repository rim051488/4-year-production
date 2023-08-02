#pragma once
#include "Vector3.h"

// クォータニオンクラス
class Quaternion
{
public:

	/// <summary>
	/// デフォルトコンストラクタ
	/// </summary>
	constexpr Quaternion() :
		w{1.0f}, x{0.0f}, y{0.0f}, z{0.0f}
	{
	}

	/// <summary>
	/// 要素の値を指定できるコンストラクタ
	/// </summary>
	/// <param name="inW"></param>
	/// <param name="inX"></param>
	/// <param name="inY"></param>
	/// <param name="inZ"></param>
	constexpr Quaternion(float inW, float inX, float inY, float inZ) :
		w{inW}, x{inX}, y{inY}, z{inZ}
	{

	}

	/// <summary>
	/// コピーコンストラクタ
	/// </summary>
	/// <param name="q"></param>
	constexpr Quaternion(const Quaternion& q) :
		w{q.w}, x{q.x}, y{q.y}, z{q.z}
	{

	}

	/// <summary>
	/// オイラー角(x,y,z)で指定できるコンストラクタ
	/// </summary>
	/// <param name="radX"> x軸の角度(ラジアン) </param>
	/// <param name="radY"> y軸の角度(ラジアン) </param>
	/// <param name="radZ"> z軸の角度(ラジアン) </param>
	Quaternion(float radX, float radY, float radZ);

	Quaternion(float rad, const Vector3& axis);

	constexpr Quaternion& operator=(const Quaternion& q) &
	{
		w = q.w;
		x = q.x;
		y = q.y;
		z = q.z;
		return *this;
	}

	constexpr Quaternion operator-() const
	{
		return { -w, -x, -y,-z };
	}

	/// <summary>
	/// 自身に対してクォータニオンq回転させる
	/// </summary>
	/// <param name="q"></param>
	/// <returns></returns>
	Quaternion& operator*=(const Quaternion& q)&;

	/// <summary>
	/// オイラー角へ変換した値を取得する
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	Vector3 ToEuler(void) const;

	/// <summary>
	/// 反転させる
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	void Inverse(void) &;

	/// <summary>
	/// 反転した値を取得する
	/// </summary>
	/// <param name=""></param>
	/// <returns> 反転させたクォータニオン </returns>
	Quaternion Inversed(void) const;

	/// <summary>
	/// 回転行列に変換する
	/// </summary>
	/// <param name="outMatrix"> 出力先の回転行列 </param>
	void ToMatrix(MATRIX& outMatrix) const;

	float Magnitude(void) const&;

	constexpr float SqMagnitude(void) const&
	{
		return (w * w + x * x + y * y + z * z);
	}

	void Normalize(void);

	Quaternion Normalized(void) const&;

	float w, x, y, z;
};

/// <summary>
/// クォータニオン同士の掛け算(qrをql分回転させる)
/// </summary>
/// <param name="ql"> 左辺 </param>
/// <param name="qr"> 右辺 </param>
/// <returns> 回転後のクォータニオン </returns>
Quaternion operator*(const Quaternion& ql, const Quaternion& qr);

/// <summary>
/// ベクトルを回転させる
/// </summary>
/// <param name="ql"></param>
/// <param name="vec"></param>
/// <returns></returns>
Vector3 operator*(const Quaternion& ql, const Vector3& vec);


