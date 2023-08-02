#pragma once
#include "Render.h"
#include "../../Common/Vector3.h"

// カプセルの描画用クラス
class CapsuleRender :
	public Render
{
public:
	CapsuleRender();
	~CapsuleRender();

	/// <summary>
	/// カラーのセット
	/// </summary>
	/// <param name="color"></param>
	void SetColor(const int color) { color_ = color; }

	/// <summary>
	/// 高さのセット
	/// </summary>
	/// <param name="height_"></param>
	void SetHeight(const float height) { height_ = height; }

	/// <summary>
	/// 半径のセット
	/// </summary>
	/// <param name="r"></param>
	void SetRadius(const float r) { radius_ = r; }

	static constexpr ComponentID id_{ ComponentID::CapsuleRender };
	ComponentID GetID(void) const override
	{
		return id_;
	}

private:
	void Draw(int shadowMap = -1, int buff = -1, int cubeTex = -1) final;
	void SetUpDepthTex(int ps = -1, int buff = -1) final;
	void Load(const std::filesystem::path& path) final;
	void Update(BaseScene& scene,ObjectManager& objectManager, float delta, Controller& controller) final;

	// 高さ
	float height_;

	// 半径
	float radius_;

	// 色
	int color_;

	// トップ座標
	Vector3 top_;

	// ボトム座標
	Vector3 bottom_;
};

