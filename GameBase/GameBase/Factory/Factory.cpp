#include "Factory.h"
#include "../Common/ObjectData.h"
#include "../Common/Debug.h"

Factory::Factory(ObjectManager& objectManager) :
	objectManager_{objectManager}
{
}

Factory::~Factory()
{
}



const bool Factory::IsLoaded(void)
{
	return true;
}

void Factory::Load(const std::string& fileName)
{
	std::ifstream file{ fileName , std::ios::binary };			// <-バイナリ指定しようね忘れると大変
	if (!file)
	{
		DebugErrorLog("ファイル読み込み失敗");
		return;
	}

	ObjectDataHeader h{};
	DataHeader dh;
	file.read(reinterpret_cast<char*>(&h), sizeof(h));
	file.read(reinterpret_cast<char*>(&dh), sizeof(dh));

	while (!file.eof())
	{
		switch (dh.type)
		{
		case DataType::Transform:
			LoadTrans(file, dh.size);
			break;
		case DataType::Model:
			LoadModel(file, dh.size);
			break;
		case DataType::Spawn:
			LoadSpawn(file, dh.size);
			break;
		case DataType::Other:
			LoadOther(file, dh.size);
			break;
		case DataType::Collider:
			LoadCollider(file, dh.size);
			break;
		default:
			break;
		}
		file.read(reinterpret_cast<char*>(&dh), sizeof(dh));
	}

}

void Factory::LoadTrans(std::ifstream& file, std::uint32_t size)
{
	file.ignore(size);
}

void Factory::LoadCollider(std::ifstream& file, std::uint32_t size)
{
	file.ignore(size);
}

void Factory::LoadModel(std::ifstream& file, std::uint32_t size)
{
	file.ignore(size);
}

void Factory::LoadSpawn(std::ifstream& file, std::uint32_t size)
{
	file.ignore(size);
}

void Factory::LoadOther(std::ifstream& file, std::uint32_t size)
{
	file.ignore(size);
}
