#pragma once
#include "Render.h"
#include "../../Common/SharedHandle.h"

// 画像をビルボードとして描画するクラス(常にこっち向いてるやつ)
class BillboardRender :
	public Render
{
public:

	static constexpr ComponentID id_{ ComponentID::BillBoardRender };
	ComponentID GetID(void) const override
	{
		return id_;
	}
private:

	void Load(const std::filesystem::path& path) final;
	void Draw(int shadowMap = -1, int buff = -1, int cubeTex = -1) final;
	void SetUpDepthTex(int ps = -1, int buff = -1)final;

	// 画像のハンドル
	SharedGraphicHandle handle_;
};

