#include "Render.h"
#include "../../Object/ObjectManager.h"

Render::Render() :
	isDraw_{true}
{
}

Render::~Render()
{
}

void Render::Begin(ObjectManager& objectManager)
{
	transform_ = objectManager.GetComponent<Transform>(ownerId_);
}


