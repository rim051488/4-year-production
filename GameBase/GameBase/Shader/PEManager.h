#pragma once
#include <memory>
#include <list>
#include <vector>
#include <array>
#include "PEID.h"
#include "../Common/Vector2.h"

class PEBase;

class PEManager
{
public:
	PEManager();
	~PEManager();
	void Update(float delta);
	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="beforeScr">ポストエフェクトをかけたいスクリーン</param>
	/// <param name="afterScr">描画するスクリーン</param>
	/// <param name="depth">深度値画像</param>
	/// <param name="skyScr">スカイドームのみの画像</param>
	/// <param name="redScr">スカイドームとステージの画像</param>
	void Draw(int beforeScr,int afterScr,int depth, int skyScr,int redScr);
	/// <summary>
	/// バッファを受け取る
	/// </summary>
	/// <param name="buff">バッファ</param>
	void SetBuffer(int buff);
	/// <summary>
	/// どのポストエフェクトをオンにするか
	/// </summary>
	/// <param name="id">ポストエフェクトのid</param>
	/// <param name="flag">trueにするかfalseにするか</param>	
	void SetFlag(PEID id,bool flag = true);
	/// <summary>
	/// 指定したidのポストエフェクトのフラグがどっちかを取得する
	/// </summary>
	/// <param name="id">ポストエフェクトのid</param>
	/// <returns></returns>
	bool GetFlag(PEID id);
	/// <summary>
	/// 対応するポストエフェクトを使用する(デバッグ時のみ)
	/// </summary>
	/// <param name="id">どのポストエフェクトなのか</param>
	/// <param name="hitkey">どのキーなのか</param>
	void CheckPostEffect(PEID id, int hitkey);
private:
	// 使用するシェーダを格納する変数
	std::array<int, 6> postPS_;
	// 左上の座標
	Vector2 pos_ = {0.0f,0.0f};
	// 描画するサイズの倍率
	Vector2 rate_ = { 1.0f,1.0f };
	// ポストエフェクトをまとめて管理する
	std::array<
		std::pair<std::shared_ptr<PEBase>, bool>
		, 7> PostEffect_;
	// 対応したキーを押しているかどうか
	std::array<bool,
		static_cast<size_t>(PEID::Max) + 1> usePE_;
	// シェーダをあらかじめセットしておく
	void ShaderLoad(void);
	// PostEffect_のフラグをすべてfalseにする
	void FlagSetFalse(void);
};

