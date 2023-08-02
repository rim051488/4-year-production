#pragma once
#include "UiBase.h"
#include <functional>
#include "../Common/SharedHandle.h"

enum class GageID
{
	Hp,
	Skill,
	Stamina,
	Max
};

// Hpとかスタミナの表示で使うゲージ
class Gage :
	public UiBase
{
	using DataBindFunc = std::function<float(ObjectManager&)>;
public:

	Gage(const int& id, const DataBindFunc& dataBindFunc, const std::filesystem::path& gagePath, const std::filesystem::path& framePath,float defalutValue, const Vector2& pos);
	
private:
	void Update(float delta, BaseScene& scene, ObjectManager& objMng, Controller& controller) final;
	void HpUpdate(ObjectManager& objMng);
	void SkillUpdate(ObjectManager& objMng);
	void FlucGageValue(ObjectManager& objMng, float cnt, bool isUp);
	void DecSubGageValue(ObjectManager& objMng, float cnt);
	void AssignGageValue(ObjectManager& objMng);
	void Draw(int mainScr) final;
	UiID GetUIID(void) final
	{
		return UiID::Frame;
	}
	void Loaded(BaseScene& scene) final;

	// ゲージの画像
	SharedGraphicHandle gageHandle_;

	// ゲージのフレームの画像
	SharedGraphicHandle frameHandle_;

	// データを紐付ける処理
	DataBindFunc dataBindFunc_;

	// 値
	float value_;

	// ゲージID
	GageID id_;

	// デフォルトのゲージ値
	float defValue_;
	// ゲージ減少前の値
	float subGageValue_;

	// 減少値
	float power_;
};

