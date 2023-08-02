#pragma once
#include <memory>
#include "PEID.h"
#include "ShrinkBuff.h"
#include "../Common/Vector2.h"

// 重さの数
struct WEIGHT_NUM
{
	float weight[8];
};

// ガウスアンブラの重さ
constexpr int NUM_WEIGHTS = 8;

class PEBase
{
public:
	PEBase(int postPS, Vector2 pos,Vector2 rate);
	~PEBase();
	virtual void Update(float delta) = 0;
	virtual int Draw(int beforeScr, int afterScr,int depth,int skyScr, int redScr) = 0;
	virtual void SetBuffer(int buff);
	virtual PEID GetPEID(void) = 0;
private:
	// ガウス用のシェーダ
	int gaussPS_;

protected:
	/// <summary>
	/// ポストエフェクトをかける場合
	/// </summary>
	/// <param name="firstScr">ポストエフェクトをかけたいスクリーン</param>
	/// <param name="secondScr">ぼかし画像</param>
	/// <param name="thirdScr">深度画像</param>
	/// <param name="postPS">ピクセルシェーダ</param>
	/// <param name="pos">左上の座標</param>
	/// <param name="rate">スクリーンの倍率</param>
	void SetPostEffect(int firstScr, int secondScr, int thirdScr,int depthScr, int postPS, Vector2 pos, Vector2 rate);

	// どのポストエフェクトもかけない場合
	void DefaultScreen(int berforeScr,int afterScr, Vector2 pos, Vector2 rate);
	/// <summary>
	/// ガウスアンブラ用
	/// </summary>
	/// <param name="pos">左上の座標</param>
	/// <param name="scr">スクリーン</param>
	/// <param name="div">受け取ったスクリーンを加工するかどうか</param>
	/// <param name="vs">縦か横の頂点シェーダ</param>
	void SetXYBlurEffect(Vector2 pos, int scr, Vector2 div, int vs);

	/// <summary>
	/// 重み計算
	/// </summary>
	/// <param name="Weights"></param>
	/// <param name="sigma"></param>
	void WeitghtsGaussian(int Weights, float sigma);

	// 自分自身のDraw
	int ownScr_;

	// 縦ブラー用のスクリーン
	int vertBlur_;
	// 横ブラー用のスクリーン
	int sideBlur_;

	// 重み計算
	WEIGHT_NUM* weight;
	WEIGHT_NUM weights_;
	int weibuff_;

	// スクリーンのサイズ
	Vector2* screen;
	Vector2 screen_;
	int scrbuff_;

	// 左上の座標
	Vector2 pos_;
	// 画像の描画倍率
	Vector2 rate_;
	// ポストエフェクト用のシェーダ
	int postPS_;

	// バッファ受け取り
	int buff_;

	// 縮小バッファ作成用クラス
	std::shared_ptr<ShrinkBuff> shrink_;
};

