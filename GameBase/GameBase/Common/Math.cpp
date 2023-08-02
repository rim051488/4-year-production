#include <DxLib.h>
#include <cmath>
#include "Math.h"

float GetNormalizedAngle(float angle, float valMin, float valMax)
{
	float cycle = valMax - valMin;
	float result = std::fmod((angle - valMin), cycle + valMin);
	if (result < valMin)
	{
		result += cycle;
	}
	return result;
}

double GetNormalizedAngle(double angle, double valMin, double valMax)
{
	double cycle = valMax - valMin;
	double result = std::fmod((angle - valMin), cycle + valMin);
	if (result < valMin)
	{
		result += cycle;
	}
	return result;
}

float DirNearAroundRad(float from, float to)
{
    float ret = 1.0f;
    float diff = to - from;
    if (diff >= 0.0f)
    {
        // 比較元よりも時計回りに位置する
        if (diff > DX_PI_F)
        {
            // でも、180度以上離れているので、反時計回りの方が近い
            ret = -1.0f;
        }
        else
        {
            // 時計回り
            ret = 1.0f;
        }
    }
    else
    {
        // 比較元よりも反時計回りに位置する
        if (diff < -DX_PI_F)
        {
            // でも、180度以上離れているので、時計回りの方が近い
            ret = 1.0f;
        }
        else
        {
            // 反時計回り
            ret = -1.0f;
        }
    }
    return ret;
}

MATRIX ToMatrix(Quaternion& q)
{
    auto xy2 = q.x * q.y * 2;
    auto xz2 = q.x * q.z * 2;
    auto xw2 = q.x * q.w * 2;
    auto yz2 = q.y * q.z * 2;
    auto yw2 = q.y * q.w * 2;
    auto zw2 = q.z * q.w * 2;
    auto ww2 = q.w * q.w * 2;
    return MATRIX({
      ww2 + 2 * q.x * q.x - 1, xy2 + zw2, xz2 - yw2,
      xy2 - zw2, ww2 + 2 * q.y * q.y - 1, yz2 + xw2,
      xz2 + yw2, yz2 - xw2, ww2 + 2 * q.z * q.z - 1
        });
}

Quaternion ToQuaternion(MATRIX& m)
{
    auto px = m.m[0][0] - m.m[1][1] - m.m[2][2] + 1;
    auto py = -m.m[0][0] + m.m[1][1] - m.m[2][2] + 1;
    auto pz = -m.m[0][0] - m.m[1][1] + m.m[2][2] + 1;
    auto pw = m.m[0][0] + m.m[1][1] + m.m[2][2] + 1;

    auto selected = 0;
    auto max = px;
    if (max < py) {
        selected = 1;
        max = py;
    }
    if (max < pz) {
        selected = 2;
        max = pz;
    }
    if (max < pw) {
        selected = 3;
        max = pw;
    }

    if (selected == 0) {
        auto x = std::sqrt(px) * 0.5f;
        auto d = 1 / (4 * x);
        return Quaternion(
            x,
            (m.m[1][0] + m.m[0][1]) * d,
            (m.m[0][2] + m.m[2][0]) * d,
            (m.m[2][1] - m.m[1][2]) * d
        );
    }
    else if (selected == 1) {
        auto y = std::sqrt(py) * 0.5f;
        auto d = 1 / (4 * y);
        return Quaternion(
            (m.m[1][0] + m.m[0][1]) * d,
            y,
            (m.m[2][1] - m.m[1][2]) * d,
            (m.m[0][2] + m.m[2][0]) * d
        );
    }
    else if (selected == 2) {
        auto z = std::sqrt(pz) * 0.5f;
        auto d = 1 / (4 * z);
        return Quaternion(
            (m.m[0][2] + m.m[2][0]) * d,
            (m.m[2][1] - m.m[1][2]) * d,
            z,
            (m.m[1][0] + m.m[0][1]) * d
        );
    }
    else if (selected == 3) {
        auto w = std::sqrt(pw) * 0.5f;
        auto d = 1 / (4 * w);
        return Quaternion(
            (m.m[2][1] - m.m[1][2]) * d,
            (m.m[0][2] + m.m[2][0]) * d,
            (m.m[1][0] + m.m[0][1]) * d,
            w
        );
    }
}

Quaternion ToQuaternion(Vector3& e)
{
    auto cx = std::cos(0.5f * e.x);
    auto sx = std::sin(0.5f * e.x);
    auto cy = std::cos(0.5f * e.y);
    auto sy = std::sin(0.5f * e.y);
    auto cz = std::cos(0.5f * e.z);
    auto sz = std::sin(0.5f * e.z);
    return Quaternion(
        -cx * sy * sz + sx * cy * cz,
        cx * sy * cz - sx * cy * sz,
        sx * sy * cz + cx * cy * sz,
        sx * sy * sz + cx * cy * cz
    );
    //switch (e.order) {
    //case EulerOrder::XYZ:
    //    return Quaternion(
    //        cx * sy * sz + sx * cy * cz,
    //        -sx * cy * sz + cx * sy * cz,
    //        cx * cy * sz + sx * sy * cz,
    //        -sx * sy * sz + cx * cy * cz
    //    );
    //case EulerOrder::XZY:
    //    return Quaternion(
    //        -cx * sy * sz + sx * cy * cz,
    //        cx * sy * cz - sx * cy * sz,
    //        sx * sy * cz + cx * cy * sz,
    //        sx * sy * sz + cx * cy * cz
    //    );
    //case EulerOrder::YXZ:
    //    return Quaternion(
    //        cx * sy * sz + sx * cy * cz,
    //        -sx * cy * sz + cx * sy * cz,
    //        cx * cy * sz - sx * sy * cz,
    //        sx * sy * sz + cx * cy * cz
    //    );
    //case EulerOrder::YZX:
    //    return Quaternion(
    //        sx * cy * cz + cx * sy * sz,
    //        sx * cy * sz + cx * sy * cz,
    //        -sx * sy * cz + cx * cy * sz,
    //        -sx * sy * sz + cx * cy * cz
    //    );
    //case EulerOrder::ZXY:
    //    return Quaternion(
    //        -cx * sy * sz + sx * cy * cz,
    //        cx * sy * cz + sx * cy * sz,
    //        sx * sy * cz + cx * cy * sz,
    //        -sx * sy * sz + cx * cy * cz
    //    );
    //case EulerOrder::ZYX:
    //    return Quaternion(
    //        sx * cy * cz - cx * sy * sz,
    //        sx * cy * sz + cx * sy * cz,
    //        -sx * sy * cz + cx * cy * sz,
    //        sx * sy * sz + cx * cy * cz
    //    );
    //}
}

Vector3 ToEuler(MATRIX& m)
{
    return Vector3();
}

Vector3 ToEuler(Quaternion& q)
{
    double ww = q.w * q.w;
    double  wx = q.w * q.x;
    double  wy = q.w * q.y;
    double  wz = q.w * q.z;
    double  xx = q.x * q.x;
    double  xy = q.x * q.y;
    double  xz = q.x * q.z;
    double  yy = q.y * q.y;
    double  yz = q.y * q.z;
    double  zz = q.z * q.z;

    return {
        static_cast<float>(std::asin(GetNormalizedAngle(-2.0 * (yz - wx)))),
        static_cast<float>(std::atan2((xz + wy) * 2.0, ww - xx - yy + zz)),
        static_cast<float>(std::atan2((xy + wz) * 2.0,ww - xx + yy - zz))
    };
}

void Vector3ToDxVector(const Vector3& vec3, VECTOR& dxVec)
{
	dxVec = VGet(vec3.x, vec3.y, vec3.z);
}

