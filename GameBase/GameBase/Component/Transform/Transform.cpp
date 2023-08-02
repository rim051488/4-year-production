#include "../../Scene/BaseScene.h"
#include "../../Common/Math.h"
#include "Transform.h"
#include "../../Object/ObjectManager.h"
#include "../../Common/Math.h"

#include "../../Common/Debug.h"

Transform::Transform()
{
}


void Transform::Update(BaseScene& scene, ObjectManager& objectManager, float delta, Controller& controller)
{
	DebugDrawLine(pos_, pos_ + (GetForward() * 50.0f), 0x0000ff);
	DebugDrawLine(pos_, pos_ + (GetUp() * 50.0f), 0x00ff00);
	DebugDrawLine(pos_, pos_ + (GetRight() * 50.0f), 0xff0000);

}

void Transform::End(ObjectManager& objectManager)
{
	rot_ = {};
	offset_ = zeroVector3<float>;
	scale_ = Vector3{ 1.0f,1.0f,1.0f };

}

const Vector3 Transform::GetForward(void) const
{
	auto v{ GetRotation() * forwardVector3<float>};
	return {v.x,v.y,v.z};
}

const Vector3 Transform::GetUp(void) const
{
	auto v{ GetRotation() * upVector3<float>};
	return { v.x,v.y,v.z };
}

const Vector3 Transform::GetRight(void) const
{
	auto v{GetRotation() * rightVector3<float>};
	return { v.x,v.y,v.z };
}

const Quaternion Transform::GetRotation(void) const
{
	return rot_;
}


Transform& Transform::operator=(const Transform& transform)
{
	return *this;
}

void Transform::Read(std::ifstream& file)
{
	const auto& [pos, rot, scale] = Load(file);
	pos_ = pos;
	scale_ = scale;
	rot_ = { rot.x, rot.y, rot.z };
}

std::tuple<Vector3, Vector3, Vector3> Transform::Load(std::ifstream& file)
{
	Vector3 pos, rotation, scale;
	file.read(reinterpret_cast<char*>(&pos), sizeof(pos));
	file.read(reinterpret_cast<char*>(&rotation), sizeof(rotation));
	file.read(reinterpret_cast<char*>(&scale), sizeof(scale));
	return std::tie(pos, rotation, scale);
}

