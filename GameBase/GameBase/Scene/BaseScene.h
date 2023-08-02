#pragma once
#include <memory>
#include <functional>
#include <unordered_map>
#include <list>
#include "SceneID.h"
#include "../Component/ComponentID.h"
#include "../Common/SharedHandle.h"
#include "../Common/ScreenID.h"
#include "../Common/Input/Controller.h"

class BaseScene;
class Camera;
class ObjectManager;
class Controller;
class UiManager;
class PEManager;

struct LIGHT_MAT
{
	MATRIX view;
	MATRIX proj;
	Vector3 eyepos;
};

class BaseScene
{
public:
	using SceneUptr = std::unique_ptr<BaseScene>;
	using LoadedFunc = std::function<void(Controller&)>;

	/// <summary> 
	/// コンストラクタ 
	/// </summary>
	/// <param name="id"> シーン </param>
	/// <param name="sceneID"> 初期シーン </param>
	/// <param name="maxObj"> オブジェクトの最大数 </param>
	BaseScene(ScreenID id, SceneID sceneID);

	/// <summary>
	/// コンストラクタ(オブジェクトマネージャーを引き継ぐ)
	/// </summary>
	/// <param name="objectManager"> オブジェクトマネージャー </param>
	/// <param name="id"> スクリーンの種類</param>
	/// <param name="sceneID"> シーンの種類 </param>
	BaseScene(std::unique_ptr<ObjectManager>&& objectManager,ScreenID id, SceneID sceneID);

	/// <summary>
	/// デストラクタ 
	/// </summary>
	virtual ~BaseScene();

	/// <summary>
	///  更新
	/// </summary>
	/// <param name="delta"> デルタタイム </param>
	virtual void Update(float delta, Controller& controller) = 0;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(void);

	/// <summary>
	/// シーンの内容の描画
	/// </summary>
	virtual void DrawScene(void) = 0;

	/// <summary>
	/// シーンのIDの取得
	/// </summary>
	/// <returns> シーンのID </returns>
	virtual const SceneID GetID(void) const = 0;

	/// <summary>+
	/// シーン内の解放処理
	/// </summary>
	virtual void Relese(void);

	/// <summary>
	/// ロードが完了したか？
	/// </summary>
	/// <returns> trueの時ロード完了 </returns>
	virtual bool IsLoaded(void);

	/// <summary>
	/// ウィンドウ系のシーンから戻った時の処理
	/// </summary>
	/// <param name=""></param>
	virtual void Back(void);

	/// <summary>
	/// シーンのポインタの更新
	/// </summary>
	/// <param name="own"> 現在のシーンのポインタ </param>
	/// <returns> 次のフレームで使うポインタ </returns>
	SceneUptr Update(SceneUptr own, float delta, Controller& controller);
	
	/// <summary>
	/// シーンを変える
	/// </summary>
	/// <param name="id"> 変更後のシーンのID </param>
	void ChangeSceneID(SceneID id);


	/// <summary>
	/// シーンを生成するためのfunctionをセットする
	/// </summary>
	/// <param name="sceneMakeFunc"> 生成するためのfunction </param>
	/// <param name="id"> 生成するシーンのID </param>
	void SetMakeSceneFunc(std::function<SceneUptr(SceneUptr)>&& sceneMakeFunc, const SceneID id);

	/// <summary>
	/// ロード完了時の処理
	/// </summary>
	void Loaded(Controller& controller);

	/// <summary>
	/// ロード完了時の処理を追加する
	/// </summary>
	/// <param name="loadedFunc"></param>
	void AddLoadedFunc(LoadedFunc&& loadedFunc);

	
	/// <summary>
	/// カメラの取得
	/// </summary>
	/// <param name=""></param>
	/// <returns> カメラの参照 </returns>
	Camera& GetCamera(void)
	{
		return *camera_;
	}

	/// <summary>
	/// オブジェクトマネージャーの取得
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	ObjectManager& GetObjectManager(void) &
	{
		return *objMng_;
	}

	UiManager& GetUiManager(void)&
	{
		return *uiMng_;
	}
protected:

	

	// 次移行するシーンID
	SceneID nextID_;

	// スクリーンID
	ScreenID scID_;

	// スクリーンハンドル
	SharedRenderTargetHandle screenHandle_;

	// カメラ
	std::unique_ptr<Camera> camera_;

	// オブジェクトマネージャー
	std::unique_ptr<ObjectManager> objMng_;

	// UIマネージャー
	std::unique_ptr<UiManager> uiMng_;

	// ポストエフェクトマネージャー
	std::unique_ptr<PEManager> peMng_;

private:

	// シーン作成のfunctionをまとめたマップ
	std::unordered_map<SceneID, std::function<SceneUptr(SceneUptr)>> sceneMakeFuncMap_;

	// ロード完了時に実行するfunction
	std::list<LoadedFunc> loadedFunc_;
};

