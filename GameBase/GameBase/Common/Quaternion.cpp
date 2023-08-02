#include "Quaternion.h"
#include "Math.h"
#include <DxLib.h>

#define ISNAN(a) (std::isfinite(a) ? a: 0.0f)

Quaternion::Quaternion(float radX, float radY, float radZ)
{
	float cosX = std::cos(RadIn2PI(radX/ 2.0f));
	float cosY = std::cos(RadIn2PI(radY/ 2.0f));
	float cosZ = std::cos(RadIn2PI(radZ/ 2.0f));
	float sinX = std::sin(RadIn2PI(radX/ 2.0f));
	float sinY = std::sin(RadIn2PI(radY/ 2.0f));
	float sinZ = std::sin(RadIn2PI(radZ/ 2.0f));

	w = cosX * cosY * cosZ + sinX * sinY * sinZ;
	x = sinX * cosY * cosZ + cosX * sinY * sinZ;
	y = cosX * sinY * cosZ - sinX * cosY * sinZ;
	z = cosX * cosY * sinZ - sinX * sinY * cosZ;
}

Quaternion::Quaternion(float rad, const Vector3& axis)
{
	if (axis.SqMagnitude() <= 0.0f)
	{
		x = y = z = 0.0f;
		w = 1.0f;
		return;
	}

	auto n = axis.Normalized();
	auto sin = std::sin(0.5f * rad);
	w = std::cos(0.5f * rad);
	x = n.x * sin;
	y = n.y * sin;
	z = n.z * sin;

}

Quaternion& Quaternion::operator*=(const Quaternion& q)&
{
	*this = (q * *this);
	return *this;
}

Vector3 Quaternion::ToEuler(void) const
{
	double ww = w * w;
	double  wx = w * x;
	double  wy = w * y;
	double  wz = w * z;
	double  xx = x * x;
	double  xy = x * y;
	double  xz = x * z;
	double  yy = y * y;
	double  yz = y * z;
	double  zz = z * z;

	return {
		static_cast<float>(std::asin(GetNormalizedAngle(-2.0 * (yz - wx)))),
		static_cast<float>(std::atan2((xz + wy) * 2.0, ww - xx - yy + zz)),
		static_cast<float>(std::atan2((xy + wz) * 2.0,ww - xx + yy - zz))
	};

	
}

void Quaternion::Inverse(void)&
{
	auto n = 1.0f / (w * w + x * x + y * y + z * z);
	w *= n;
	x = -x * n;
	y = -y * y;
	z = -z * n;
}

Quaternion Quaternion::Inversed(void) const
{
	auto n = 1.0f / (w * w + x * x + y * y + z * z);
	return { w * n, -x * n, -y * n, -z * n };
}

void Quaternion::ToMatrix(MATRIX& outMatrix) const
{
	outMatrix = MGetIdent();
	auto sqW = w * w;
	auto sqX = x * x;
	auto sqY = y * y;
	auto sqZ = z * z;
	auto invs = 1.0f / (sqX + sqY + sqZ + sqW);

	// 一行目
	outMatrix.m[0][0] = (sqW + sqX - sqY - sqZ) * invs;
	outMatrix.m[0][1] = 2.0f * (x * y + z * w) * invs;
	outMatrix.m[0][2] = 2.0f * (x * z - y * w) * invs;
	
	// 二行目
	outMatrix.m[1][0] = 2.0f * (x * y - z * w) * invs;
	outMatrix.m[1][1] = (sqW - sqX + sqY - sqZ) * invs;
	outMatrix.m[1][2] = 2.0f * (y * z - x * w) * invs;
	
	// 三行目
	outMatrix.m[2][0] = 2.0f * (x * z + y * w) * invs;
	outMatrix.m[2][1] = 2.0f * (y * z - x * w) * invs;
	outMatrix.m[2][2] = (sqW - sqX + sqY - sqZ) * invs;
}

float Quaternion::Magnitude(void) const&
{
	return std::sqrt(w * w + x * x + y * y + z * z);
}

void Quaternion::Normalize(void)
{
	auto magnitude = Magnitude();
	w /= magnitude;
	x /= magnitude;
	y /= magnitude;
	z /= magnitude;
}

Quaternion Quaternion::Normalized(void) const&
{
	auto magnitude = Magnitude();
	return { w / magnitude , x / magnitude, y / magnitude, z / magnitude };
}

Quaternion operator*(const Quaternion& ql, const Quaternion& qr)
{
	float x, y, z, w;
	w = (-ql.x * qr.x) + (-ql.y * qr.y) + (-ql.z * qr.z) + (ql.w * qr.w);
	x = (ql.w * qr.x) + (-ql.z * qr.y) + (ql.y * qr.z) + (ql.x * qr.w);
	y = (ql.z * qr.x) + (ql.w * qr.y) + (-ql.x * qr.z) + (ql.y * qr.w);
	z = (-ql.y * qr.x) + (ql.x * qr.y) + (ql.w * qr.z) + (ql.z * qr.w);
	return { w, x, y, z, };
}

Vector3 operator*(const Quaternion& ql, const Vector3& vec)
{
	Quaternion tmp{ 0.0f, vec.x, vec.y, vec.z };
	Quaternion ret{ (ql * tmp) * ql.Inversed() };
	return {ret.x, ret.y, ret.z};
}
