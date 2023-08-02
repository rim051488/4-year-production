#pragma once
#include "../Controller.h"

// キーボード用
class Keyboard :
    public Controller
{
public:
	Keyboard();
	~Keyboard();

	// 初期化
	bool Init(void)override;
	
	// 更新 Update(deltaTime)
	void Update(float delta) override;

	// コントローラータイプの取得private:
	CntType GetCntType(void) override { return CntType::Key; }

	// キーデータの取得
	const std::array<char, 256>& GetKeyData(void) const&
	{
		return keyData_;
	}
private:

	/// <summary>
	/// カーソルの座標をセットする
	/// </summary>
	/// <param name="pos"> 座標 </param>
	void SetCursorPos(const Vector2& pos = lpSceneMng.screenSize_<float> / 2.0f) final;

	// キーデータ格納用
	std::array<char, 256> keyData_ = {};
};

