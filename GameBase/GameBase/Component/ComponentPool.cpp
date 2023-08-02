#include "ComponentPool.h"
#include "Transform/Transform.h"
#include "Info/ObjectInfo.h"
#include "Render/ModelRender.h"
#include "Collider/SphereCollider.h"


ComponentPool::ComponentPool()
{
	for (int i = 0; i < 20; i++)
	{
		Push(std::make_unique<Transform>());
		Push(std::make_unique<ObjectInfo>());
		Push(std::make_unique<ModelRender>());
		Push(std::make_unique<SphereCollider>());
	}
}

ComponentPool::~ComponentPool()
{
}
