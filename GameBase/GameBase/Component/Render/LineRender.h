#pragma once
#include "Render.h"

// 前方方向に線を描画するクラス
class LineRender :
    public Render
{
public:

	/// <summary>
	/// 線の描画色をセットする
	/// </summary>
	/// <param name="color"></param>
	void SetColor(int color)
	{
		color_ = color;
	}

	/// <summary>
	/// 線の長さをセットする
	/// </summary>
	/// <param name="length"> 長さ </param>
	void SetLength(float length)
	{
		length_ = length;
	}

	static constexpr ComponentID id_{ ComponentID::LineRender };
	ComponentID GetID(void) const override
	{
		return id_;
	}
private:

	void Load(const std::filesystem::path& path) final;
	void Draw(int shadowMap = -1, int buff = -1, int cubeTex = -1) final;
	void SetUpDepthTex(int ps = -1, int buff = -1)final;

	// 描画する色
	int color_;

	// 線の長さ
	float length_;
};

