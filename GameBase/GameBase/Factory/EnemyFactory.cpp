#include "EnemyFactory.h"
#include "../Common/ObjectData.h"
#include "../Object/ObjectManager.h"
#include "../Component/Transform/Transform.h"
#include "../Component/Render/ModelRender.h"
#include "../Component/Info/ObjectInfo.h"
#include "../Component/Collider/CharactorCollider.h"
#include "../Component/Behavior/EnemyBehavior.h"
#include "../Component/Animator/Animator.h"
#include "MuzzleFlashEffect.h"
#include "EnemyBullet.h"
#include "../Common/Debug.h"
#include "../Component/ComponentPool.h"
#include "../Common/ResourceMng.h"
#include "../SceneManager.h"
#include "EnemyAttackFactory.h"

EnemyFactory::EnemyFactory(ObjectID& stageID,const std::string& path, ObjectManager& objectManager) :
	Factory{objectManager}, height_{0.0f}, radius_{0.0f}, stageID_{stageID},
	hitTime_{0.0f}, searchDist_{0.0f}, speed_{0.0f}, stopDist_{0.0f}
{
	Factory::Load(path);
	objectManager_.AddFactory(std::make_unique<EnemyBulletFactory>(objectManager_));
	objectManager_.AddFactory(std::make_unique<EnemyAttackFactory>(objectManager_));
	objectManager_.AddFactory(std::make_unique<MuzzleFlashEffect>(objectManager_));
}

ObjectID EnemyFactory::Create(ObjectID ownerID, const Vector3& pos, const Vector3& rot)
{
	auto id = objectManager_.MakeObjectID();
	objectManager_.AddComponent(std::make_unique<ObjectInfo>(), id);
	objectManager_.AddComponent(std::make_unique<Transform>(), id);
	objectManager_.AddComponent(std::make_unique<ModelRender>(), id);
	objectManager_.AddComponent(std::make_unique<CharactorCollider>(), id);
	objectManager_.AddComponent(std::make_unique<EnemyBehavior>(), id);
	objectManager_.AddComponent(std::make_unique<Animator>(), id);



	// レンダーに描画するモデルのパスを読み込ませる
	auto render = objectManager_.GetComponent<Render>(id);
	render->Load(modelPath_);

	// トランスフォームを設定
	auto trans = objectManager_.GetComponent<Transform>(id);
	trans->Pos() = pos;
	trans->Scale() = scale_;
	trans->SetRotation({ rot_.x,0.0f,0.0f });

	// 当たり判定を設定
	auto col = objectManager_.GetComponent<CharactorCollider>(id);

	col->SetHeight(height_);
	col->SetRadius(radius_);
	col->SetHeight(80.0f);
	col->SetRadius(200.0f);
	col->SetOffset({ 0.0f, 160, -200.0f });

	// オブジェクトの情報を設定
	auto info = objectManager_.GetComponent<ObjectInfo>(id);
	info->SetUseFactory(false);
	info->SetAttribute(ObjectAttribute::Enemy);

	// 動きに関する設定
	auto  behavior = objectManager_.GetComponent<EnemyBehavior>(id);
	behavior->SetStopDistance(stopDist_);
	behavior->SetHitTime(hitTime_);
	behavior->SetSearchDistance(searchDist_);
	behavior->SetSpeed(speed_);
	behavior->SetStageID(stageID_);
	behavior->SetShotData({ shotInterval_,burstInterval_,static_cast<int>(burstNum_) });
	objectManager_.SetEnemyID(id);

	// デフォルトのモデルの角度をセット
	objectManager_.GetComponent<ModelRender>(id)->SetDefaultRot({ 0.0f,Deg2Rad(180.0f), 0.0f });
	objectManager_.GetComponent<ModelRender>(id)->SetBoundingSize({ -50.0f,200.0f,50.0f }, { 50.0f,1.0f,-50.0f });

	// レーザーサイトを作成
	auto laserID = objectManager_.MakeObjectID();
	objectManager_.AddComponent(objectManager_.GetPool().Pop<EnemyLaserSightBehavior>(), laserID);
	auto laserSight = objectManager_.GetComponent<EnemyLaserSightBehavior>(laserID);
	laserSight->SetEnemyID(id);

	objectManager_.AddComponent(objectManager_.GetPool().Pop<Transform>(), laserID);
	objectManager_.AddComponent(objectManager_.GetPool().Pop<ObjectInfo>(), laserID);

	objectManager_.AddComponent(objectManager_.GetPool().Pop<LineRender>(), laserID);
	auto line = objectManager_.GetComponent<LineRender>(laserID);
	line->SetColor(0xff0000);
	laserSight->SetOffset({ 0.0f, 220.0f,10.0f });

	objectManager_.GetComponent<EnemyBehavior>(id)->SetLaserSightID(laserID);

	return id;
}

void EnemyFactory::LoadTrans(std::ifstream& file, std::uint32_t size)
{
	const auto& [pos, rot, scale] = Transform::Load(file);
	scale_ = scale;
	rot_ = rot;
}

void EnemyFactory::LoadModel(std::ifstream& file, std::uint32_t size)
{
	modelPath_ = ModelRender::Load(file);
	lpSceneMng.GetResourceMng().LoadModel(preLoadHandle_, modelPath_);
}

void EnemyFactory::LoadCollider(std::ifstream& file, std::uint32_t size)
{
	ColDataType type;
	file.read(reinterpret_cast<char*>(&type), sizeof(type));
	if (type != ColDataType::Capsule)
	{
		// カプセルではないとき読み飛ばす
		file.ignore(size - sizeof(type));
		return;
	}
	file.read(reinterpret_cast<char*>(&colCeter_), sizeof(colCeter_));
	file.read(reinterpret_cast<char*>(&height_), sizeof(height_));
	file.read(reinterpret_cast<char*>(&radius_), sizeof(radius_));

}

void EnemyFactory::LoadOther(std::ifstream& file, std::uint32_t size)
{
	std::uint32_t num = 0;
	file.read(reinterpret_cast<char*>(&num), sizeof(num));
	file.read(reinterpret_cast<char*>(&hitTime_), sizeof(hitTime_));
	file.read(reinterpret_cast<char*>(&searchDist_), sizeof(searchDist_));
	file.read(reinterpret_cast<char*>(&speed_), sizeof(speed_));
	file.read(reinterpret_cast<char*>(&stopDist_), sizeof(stopDist_));
	file.read(reinterpret_cast<char*>(&shotInterval_), sizeof(shotInterval_));
	file.read(reinterpret_cast<char*>(&burstInterval_), sizeof(burstInterval_));
	file.read(reinterpret_cast<char*>(&burstNum_), sizeof(burstNum_));
}
