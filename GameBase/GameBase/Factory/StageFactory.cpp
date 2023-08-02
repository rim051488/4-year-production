#include "StageFactory.h"
#include <DxLib.h>
#include <fstream>
#include "../Common/ResourceMng.h"
#include "../SceneManager.h"
#include "../Object/ObjectManager.h"
#include "../Component/Info/ObjectInfo.h"
#include "../Component/Transform/Transform.h"
#include "../Component/Render/StageRender.h"
#include "../Component/Render/ModelRender.h"
#include "../Component/Collider/TerrainCollider.h"
#include "PlayerFactory.h"
#include "EnemyFactory.h"

#include "../Common/ThreadPool.h"
#include "../Common/ObjectData.h"
#include "../Common/Utility.h"
#include "../Factory/SkyFactory.h"
#include "../Component/Behavior/StageBehavior.h"
#include "../Component/Behavior/TutorialBehavior.h"

#include "../Component/Behavior/CameraBehavior.h"

#include "../Common/Debug.h"



StageFactory::StageFactory(const std::filesystem::path& path, ObjectManager& objectManager) :
	Factory{objectManager}
{
	isLoaded_.store(false);
	path_ = path;
	Thread.Add(std::bind(&StageFactory::Load, this));
}

StageFactory::StageFactory(ObjectManager& objectManager) :
	Factory{objectManager}
{
	isLoaded_.store(false);
	path_ = "Resource/Other/Tutorial.data";
	Thread.Add(std::bind(&StageFactory::LoadTutorial, this));
}

void StageFactory::Load(void)
{
	funcMap_.emplace(FactoryID::Sky, &StageFactory::CreateSkyDome);
	funcMap_.emplace(FactoryID::Player, &StageFactory::CreatePlayer);
	funcMap_.emplace(FactoryID::Enemy, &StageFactory::CreateEnemy);
	

	id_ = objectManager_.MakeObjectID();

	Factory::Load(path_.string());

	objectManager_.AddComponent(std::make_unique<ObjectInfo>(), id_);
	objectManager_.AddComponent(std::make_unique<Transform>(), id_);
	auto trans = objectManager_.GetComponent<Transform>(id_);
	trans->Pos() = pos_;
	trans->Scale() = scale_;

	//objectManager_.AddComponent(std::make_unique<ModelRender>(), id_);
	objectManager_.AddComponent(std::make_unique<StageRender>(), id_);
	auto m = objectManager_.GetComponent<Render>(id_);
	m->Load(model_);

	objectManager_.AddComponent(std::make_unique<TerrainCollider>(), id_);
	objectManager_.AddComponent(std::make_unique<StageBehavior>(std::move(enemyPosList_)), id_);
	



	auto col = objectManager_.GetComponent<TerrainCollider>(id_);
	col->Load(stageCol_);

	auto info = objectManager_.GetComponent<ObjectInfo>(id_);
	info->SetAttribute(ObjectAttribute::Stage);

	auto camera = objectManager_.MakeObjectID();
	objectManager_.AddComponent(std::make_unique<ObjectInfo>(), camera);
	objectManager_.AddComponent(std::make_unique<Transform>(), camera);
	auto cameraBehavior = std::make_unique<CameraBehavior>();
	cameraBehavior->SetSyncID(objectManager_.GetPlayerID());
	objectManager_.AddComponent(std::move(cameraBehavior), camera);
	objectManager_.SetCameraID(camera);

	objectManager_.SetStageID(id_);
	isLoaded_.store(true);
}

void StageFactory::LoadTutorial(void)
{
	funcMap_.emplace(FactoryID::Sky, &StageFactory::CreateSkyDome);
	funcMap_.emplace(FactoryID::Player, &StageFactory::CreatePlayer);
	objectManager_.AddFactory(std::make_unique<EnemyFactory>(id_, "Resource/Other/Enemy.data", objectManager_));
	id_ = objectManager_.MakeObjectID();

	Factory::Load(path_.string());

	objectManager_.AddComponent(std::make_unique<ObjectInfo>(), id_);
	objectManager_.AddComponent(std::make_unique<Transform>(), id_);
	auto trans = objectManager_.GetComponent<Transform>(id_);
	trans->Pos() = pos_;
	trans->Scale() = scale_;

	//objectManager_.AddComponent(std::make_unique<ModelRender>(), id_);
	objectManager_.AddComponent(std::make_unique<StageRender>(), id_);
	auto m = objectManager_.GetComponent<Render>(id_);
	m->Load(model_);

	objectManager_.AddComponent(std::make_unique<TerrainCollider>(), id_);
	objectManager_.AddComponent(std::make_unique<TutorialBehavior>(), id_);

	auto col = objectManager_.GetComponent<TerrainCollider>(id_);
	col->Load("Resource/resource/StageCol.mv1");

	auto info = objectManager_.GetComponent<ObjectInfo>(id_);
	info->SetAttribute(ObjectAttribute::Stage);

	auto camera = objectManager_.MakeObjectID();
	objectManager_.AddComponent(std::make_unique<ObjectInfo>(), camera);
	objectManager_.AddComponent(std::make_unique<Transform>(), camera);
	auto cameraBehavior = std::make_unique<CameraBehavior>();
	cameraBehavior->SetSyncID(objectManager_.GetPlayerID());
	objectManager_.AddComponent(std::move(cameraBehavior), camera);
	objectManager_.SetCameraID(camera);

	objectManager_.SetStageID(id_);
	isLoaded_.store(true);
}

const bool StageFactory::IsLoaded(void)
{
	return isLoaded_.load();
}

void StageFactory::CreateSkyDome(std::ifstream& file)
{
	const auto& [pos, rot, scale] = Transform::Load(file);
	auto name = ReadString(file);
	 objectManager_.AddFactory(std::make_unique<SkyFactory>(name,objectManager_));
	 objectManager_.CreateFromFactory(FactoryID::Sky, id_, pos);
}

void StageFactory::CreatePlayer(std::ifstream& file)
{
	// ç¿ïWÇ∆Ç©
	const auto& [pos, rot, scale] = Transform::Load(file);

	auto name = ReadString(file);

	objectManager_.AddFactory(std::make_unique<PlayerFactory>(name, objectManager_));
	objectManager_.CreateFromFactory(FactoryID::Player, id_, pos);
}

void StageFactory::CreateEnemy(std::ifstream& file)
{
	const auto& [pos, rot, scale] = Transform::Load(file);
	auto name = ReadString(file);
	if (!objectManager_.HaveFactory(FactoryID::Enemy))
	{
		objectManager_.AddFactory(std::make_unique<EnemyFactory>(id_,name, objectManager_));
	}
	enemyPosList_[*objectManager_.CreateFromFactory(FactoryID::Enemy, id_, pos)] = { pos.x,pos.z };
}

void StageFactory::LoadTrans(std::ifstream& file, std::uint32_t size)
{
	auto [pos,rot,scale] = Transform::Load(file);
	pos_ = pos;
	scale_ = scale;
}


void StageFactory::LoadModel(std::ifstream& file, std::uint32_t size)
{
	model_ = ModelRender::Load(file);
}

void StageFactory::LoadSpawn(std::ifstream& file, std::uint32_t size)
{
	FactoryID fID;
	file.read(reinterpret_cast<char*>(&fID), sizeof(fID));
	if (funcMap_.contains(fID))
	{
		(this->*funcMap_.at(fID))(file);
	}
}

void StageFactory::LoadCollider(std::ifstream& file, std::uint32_t size)
{
	ColDataType type;
	file.read(reinterpret_cast<char*>(&type), sizeof(type));
	if (type != ColDataType::Mesh)
	{
		// ÉÅÉbÉVÉÖÇ≈ÇÕÇ»Ç¢Ç∆Ç´ì«Ç›îÚÇŒÇ∑
		file.ignore(size - sizeof(type));
		return;
	}

	stageCol_ = ReadString(file);
}
