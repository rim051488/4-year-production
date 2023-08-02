#include "EnemyBullet.h"
#include "../Object/ObjectManager.h"
#include "../Component/ComponentPool.h"
#include "../Component/Info/ObjectInfo.h"
#include "../Component/Behavior/EnemyBulletBehavior.h"
#include "../Component/Render/ModelRender.h"
#include "../Component/Collider/SphereCollider.h"
#include "../Component/Transform/Transform.h"
#include "../Common/ResourceMng.h"
#include "../SceneManager.h"
constexpr int maxPool{ 20 };

EnemyBulletFactory::EnemyBulletFactory(ObjectManager& objectManager) :
	Factory{objectManager}
{
	for (int i = 0; i < maxPool; i++)
	{
		behaviorPool_.emplace_front(std::make_unique<EnemyBulletBehavior>());
	}

	lpSceneMng.GetResourceMng().LoadModel(model_,"Resource/resource/Bullet.mv1");
}

EnemyBulletFactory::~EnemyBulletFactory()
{
}

ObjectID EnemyBulletFactory::Create(ObjectID ownerID, const Vector3& pos, const Vector3& rot)
{
	if (behaviorPool_.empty())
	{
		return ObjectID{};
	}

	auto id = objectManager_.MakeObjectID();

	auto info = objectManager_.GetPool().Pop<ObjectInfo>();
	info->SetUseFactory(true);
	info->SetAttribute(ObjectAttribute::EnemyAttack);
	objectManager_.AddComponent(std::move(info), id);


	objectManager_.AddComponent(std::move(behaviorPool_.front()), id);
	behaviorPool_.pop_front();

	auto modelRender = objectManager_.GetPool().Pop<ModelRender>();
	static_cast<Render&>(*modelRender).Load("Resource/resource/Bullet.mv1");
	modelRender->SetBoundingSize({ -15.0f,15.0f,15.0f }, { 15.0f,-15.0f,-15.0f });
	objectManager_.AddComponent(std::move(modelRender), id);

	auto spCol = objectManager_.GetPool().Pop<SphereCollider>();
	spCol->SetRadius(10.0f);
	objectManager_.AddComponent(std::move(spCol), id);

	auto trans = objectManager_.GetPool().Pop<Transform>();
	trans->Pos() = pos;
	auto ownerTransform = objectManager_.GetComponent<Transform>(ownerID);
	if (ownerTransform.IsActive())
	{
		trans->SetRotation({ Deg2Rad(5.0f), 0.0f, 0.0f });
		trans->AddRotation(ownerTransform->GetRotation());
	}
	objectManager_.AddComponent(std::move(trans), id);

	return id;
}

void EnemyBulletFactory::Destroy(ObjectID id)
{
	objectManager_.GetPool().Push(objectManager_.RemoveComponent<ObjectInfo>(id));
	behaviorPool_.emplace_front(objectManager_.RemoveComponent<EnemyBulletBehavior>(id));
	objectManager_.GetPool().Push(objectManager_.RemoveComponent<ModelRender>(id));
	objectManager_.GetPool().Push(objectManager_.RemoveComponent<SphereCollider>(id));
	objectManager_.GetPool().Push(objectManager_.RemoveComponent<Transform>(id));
}
