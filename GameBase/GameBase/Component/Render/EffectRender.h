#pragma once
#include "Render.h"
#include "../../Common/SharedHandle.h"
#include "../../Common/Vector3.h"

struct effectTime
{
	float desEx;					//死亡時の爆発再生時間
	float bossDesEx;				//ボス死亡時のエフェクトと再生時間
	float sordLineVertical;			//垂直切りの再生時間
	float sordLineHorizontal;		//水平切りの再生時間
	float sordLineThrust;			//突きの再生時間
};

typedef enum EffectIndex{
	DesEffect,						//死亡時の爆発(ショート)
	BossDes,						//死亡時の爆発(ロング)
	SordLineVertical,				//垂直切りの再生時間
	SordLineHorizontal,				//水平切りの再生時間
	SordLineThrust,					//突きの再生時間
	EffectMax,						//最大値
}EffectIndex_;
//
////エフェクトの基本情報構造体
//typedef struct EffectBaseInit {
//
//	int ImageHandle;
//
//}EffectInit;
//
//typedef struct EffectInfo {
//	//	エフェクトの使用中かどうかのフラグ
//	bool usingEffect_;
//
//	//エフェクトの終了リクエスト
//	bool endRequest_;
//
//	//エフェクト
//	EffectIndex_ effect_;
//
//	//基本情報構造体のアドレス
//	EffectInit* imageInit_;
//
//	// エフェクト別のデータを格納するメモリ領域のアドレス
//	void * subDate_;
//
//}EffectInfo_;
//
//typedef struct EffectFunction
//{
//	// 初期化時に呼ばれる関数ポインタ
//	bool (*Init)(EffectInit *information);
//
//	// 後始末後に呼ばれる関数ポインタ
//	void (*Terminate)(EffectInit* information);
//
//	// 作成時に呼ばれる関数ポインタ
//	bool (*Create)(EffectInfo* info_);
//
//	// 削除後に呼ばれる関数ポインタ
//	void (*Delete)(EffectInfo* info_);
//
//	// 状態遷移時に呼ばれる関数ポインタ
//	void (*Step)(EffectInfo* info_,float stepTime_);
//
//	// 描画時に呼ばれる関数ポインタ
//	void (*Draw)(EffectInfo* info_);
//
//}EffectFunction_;

//エフェクトの描画用クラス
class EffectRender :
    public Render
{
public:
	EffectRender();
	~EffectRender();

	static constexpr ComponentID id_{ ComponentID::EffectRender };

	static constexpr int EFFECT_MAXNUM = 256;

	ComponentID GetID(void) const override
	{
		return id_;
	}
	
	SharedEffectHandle& GetHandle(void) &
	{
		return handle_;
	}
	
	bool IsEnd(void);

	void Play(void);

	void Stop(void);
protected:
	// エフェクトのハンドル
	SharedEffectHandle handle_;
private:
	// エフェクトのイニシャライズ
	bool Init(void);

	// ロード処理
	void Load(const std::filesystem::path& path) final;

	// エフェクトのアップデート
	void Update(BaseScene& scene, ObjectManager& objectManager, float delta, Controller& controller) final;

	// エフェクトのDraw処理
	void Draw(int shadowMap = -1, int buff = -1, int cubeTex = -1) final{}

	void SetUpDepthTex(int ps = -1, int buff = -1) final{}
	
	// エフェクトに対する終了リクエスト
	//bool EffectEndRequest(EffectInfo* info);

	//// エフェクトの作成
	//EffectInfo* EffectCreate(EffectIndex index_);
	//
	// エフェクトの削除
	//void EffectDeleate(void);

	//// 全エフェクトの削除
	//void EffectDeleateAll(void);

	//// 全エフェクトの状態遷移
	//void EffectStepAll(float stepTime);

	void UpdateEffekseer(void);

	//void LoadEffect(void);

	void End(ObjectManager& objectManager) final;

	//static EffectFunction_ functionTable[EffectMax] = {};
	/*
	static EffectInit baseInit_[EffectMax];

	static EffectInfo effectInfo_[EFFECT_MAXNUM];
*/


	//再生用プレイングハンドル
	int playingHandle_;
	
	//エフェクトの再生時間
	int effectTime_;

	//エフェクトのリソースハンドル
	int effectResourceHandle;

	//エフェクトの総再生時間
	float timeMax_ = 0.0f;

	//エフェクトの総再生時間一覧
	effectTime effectTimeMax_ = { 120.0f, 315.0f, 60.0f, 40.0f, 40.0f };


	//エフェクトの描画開始座標
	Vector3 playPos_;

	//エフェクトの終了フラグ
	bool isEnd_;

	//エフェクト時間
	//effectTime effecttime_;

	std::vector<SharedEffectHandle>useShaders_;

	

};

