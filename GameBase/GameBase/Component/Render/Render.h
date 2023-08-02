#pragma once
#include "../ComponentBase.h"
#include <filesystem>
#include "../Transform/Transform.h"
#include "../ComponentHandle.h"

class BaseScene;

// 描画用のクラス
class Render :
	public ComponentBase
{
public:
	Render();
	virtual ~Render();

	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="shadowMap"></param>
	/// <param name="buff"></param>
	virtual void Draw(int shadowMap = -1,int buff = -1,int cubeTex = -1) = 0;

	/// <summary>
	/// シャドウマップ作成用
	/// </summary>
	/// <param name=""></param>
	virtual void SetUpDepthTex(int ps = -1,int buff = -1) = 0;
	
	/// <summary>
	/// 読み込み
	/// </summary>
	/// <param name="path"></param>
	/// <param name="scene"></param>
	virtual void Load(const std::filesystem::path& path) = 0;

	void Begin(ObjectManager& objectManager) override;

	void SetDrawFlag(bool isDraw)
	{
		isDraw_ = isDraw;
	}

	ComponentID GetID(void) const override
	{
		return id_;
	}

	SetID(ComponentID::RenderBase, ComponentID::RenderBase)

protected:

	// トランスフォーム
	ComponentHandle<Transform> transform_;

	// 描画するかのフラグ
	bool isDraw_;
};

