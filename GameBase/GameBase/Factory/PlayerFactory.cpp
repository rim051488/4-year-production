#include "../Object/ObjectManager.h"
#include "../Component/Info/ObjectInfo.h"
#include "../Component/Behavior/PlayerBehavior.h"
#include "PlayerFactory.h"
#include "../Common/Input/Controller.h"
#include "../Component/Render/ModelRender.h"
#include "../Component/Collider/CharactorCollider.h"
#include "../Factory/PlayerAttackFactory.h"
#include "PlayerSkillFactory.h"
#include "PlayerBullet.h"

#include "FireExEffect.h"
#include "HorizontalEffect.h"
#include "RedExEffect.h"
#include "ThrustEffect.h"
#include "VerticalEffect.h"
#include "BeemEffect.h"
#include "BlasterEffect.h"
#include "HitEffect.h"
#include "SandSmokeEffect.h"
#include "SandSmokeSlowMoveEffect.h"
#include "SandSmokeDiffusionEffect.h"

#include "../Common/Debug.h"

PlayerFactory::PlayerFactory(const std::string& filePath, ObjectManager& objectManager) :
	Factory{objectManager}
{
	Factory::Load(filePath);
	objectManager_.AddFactory(std::make_unique<PlayerAttackFactory>(objectManager_));
	objectManager_.AddFactory(std::make_unique<PlayerSkillFactory>(objectManager_));
	objectManager_.AddFactory(std::make_unique<PlayerBulletFactory>(objectManager_));

	objectManager_.AddFactory(std::make_unique<FireExEffect>(objectManager_));
	objectManager_.AddFactory(std::make_unique<HorizontalEffect>(objectManager_));
	objectManager_.AddFactory(std::make_unique<RedExEffect>(objectManager_));
	objectManager_.AddFactory(std::make_unique<ThrustEffect>(objectManager_));
	objectManager_.AddFactory(std::make_unique<VerticalEffect>(objectManager_));
	objectManager_.AddFactory(std::make_unique<BuleBeemEffect>(objectManager_));
	objectManager_.AddFactory(std::make_unique<BlasterEffect>(objectManager_));
	objectManager_.AddFactory(std::make_unique<HitEffect>(objectManager_));
	objectManager_.AddFactory(std::make_unique<SandSmokeEffect>(objectManager_));
	objectManager_.AddFactory(std::make_unique<SandSmokeSlowMoveEffect>(objectManager_));
	objectManager_.AddFactory(std::make_unique<SandSmokeDiffusionEffect>(objectManager_));
}

ObjectID PlayerFactory::Create(ObjectID ownerID, const Vector3& pos, const Vector3& rot )
{
	auto id = objectManager_.MakeObjectID();

	objectManager_.AddComponent(std::make_unique<ObjectInfo>(), id);
	objectManager_.GetComponent<ObjectInfo>(id)->SetAttribute(ObjectAttribute::Player);
	
	objectManager_.AddComponent(std::make_unique<ModelRender>(), id);

	// ç¿ïWÇ∆Ç©Ç…ä÷Ç∑ÇÈ

	objectManager_.AddComponent(std::make_unique<Transform>(), id);

	objectManager_.AddComponent(std::make_unique<Animator>(), id);


	objectManager_.AddComponent(std::make_unique<PlayerBehavior>(), id);

	objectManager_.AddComponent(std::make_unique<CharactorCollider>(), id);

	auto transform = objectManager_.GetComponent<Transform>(id);
	transform->Pos() = pos;
	transform->Pos().y += 15;
	transform.Get()->Scale() = scale_;
	

	objectManager_.GetComponent<Render>(id)->Load(modelPath_);
	auto col = objectManager_.GetComponent<CharactorCollider>(id);
	col->SetHeight(40.0f);
	col->SetRadius(100.0f);
	col->SetOffset({ 0.0f, 40.0f, 140.0f });
	
	objectManager_.SetPlayerID(id);
	return id;
}

void PlayerFactory::LoadTrans(std::ifstream& file, std::uint32_t size)
{
	const auto& [pos, rot, scale] = Transform::Load(file);
	scale_ = scale;
	rot_ = rot;
}

void PlayerFactory::LoadModel(std::ifstream& file, std::uint32_t size)
{
	modelPath_ = ModelRender::Load(file);
}


