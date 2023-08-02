#include "PlayerBullet.h"
#include "../Object/ObjectManager.h"
#include "../Component/ComponentPool.h"
#include "../Component/Info/ObjectInfo.h"
#include "../Component/Behavior/PlayerBulletBehavior.h"
#include "../Component/Render/ModelRender.h"
#include "../Component/Collider/SphereCollider.h"
#include "../Component/Transform/Transform.h"
#include "../Common/ResourceMng.h"
#include "../SceneManager.h"
constexpr int maxPool{ 20 };

PlayerBulletFactory::PlayerBulletFactory(ObjectManager& objectManager):
    Factory{objectManager}
{
	for (int i = 0; i < maxPool; i++)
	{
		behaviorPool_.emplace_front(std::make_unique<PlayerBulletBehavior>());
	}

	lpSceneMng.GetResourceMng().LoadModel(model_, "Resource/resource/Bullet.mv1");
}

PlayerBulletFactory::~PlayerBulletFactory()
{
}

ObjectID PlayerBulletFactory::Create(ObjectID ownerID, const Vector3& pos, const Vector3& rot)
{
	if (behaviorPool_.empty())
	{
		return ObjectID{};
	}

	auto id = objectManager_.MakeObjectID();

	auto info = objectManager_.GetPool().Pop<ObjectInfo>();
	info->SetUseFactory(true);
	info->SetAttribute(ObjectAttribute::PlayerAttack);
	objectManager_.AddComponent(std::move(info), id);


	objectManager_.AddComponent(std::move(behaviorPool_.front()), id);
	behaviorPool_.pop_front();

	auto modelRender = objectManager_.GetPool().Pop<ModelRender>();
	static_cast<Render&>(*modelRender).Load("Resource/resource/Bullet.mv1");
	modelRender->SetBoundingSize({ -15.0f,15.0f,15.0f }, { 15.0f,-15.0f,-15.0f });
	objectManager_.AddComponent(std::move(modelRender), id);
	// �f�t�H���g�̃��f���̊p�x���Z�b�g
	objectManager_.GetComponent<ModelRender>(id)->SetDefaultRot({ 0.0f,Deg2Rad(180.0f), 0.0f });

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

void PlayerBulletFactory::Destroy(ObjectID id)
{
	objectManager_.GetPool().Push(objectManager_.RemoveComponent<ObjectInfo>(id));
	behaviorPool_.emplace_front(objectManager_.RemoveComponent<PlayerBulletBehavior>(id));
	objectManager_.GetPool().Push(objectManager_.RemoveComponent<ModelRender>(id));
	objectManager_.GetPool().Push(objectManager_.RemoveComponent<SphereCollider>(id));
	objectManager_.GetPool().Push(objectManager_.RemoveComponent<Transform>(id));
}
