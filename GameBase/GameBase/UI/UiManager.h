#pragma once
#include <memory>
#include <list>
#include <vector>
#include <fstream>
#include <atomic>
#include <filesystem>
#include "../Scene/SceneID.h"
#include "../Common/Vector2.h"

class BaseScene;
class ObjectManager;
class UiBase;
class Controller;

class UiManager
{
public:
	UiManager(const std::filesystem::path& path, bool isAsync = true, bool isTutorial = false, bool isCreateCursor = true);
	~UiManager();


	void Begin(BaseScene& scene);


	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="delta"></param>
	/// <param name="scene"></param>
	/// <param name="obj"></param>
	/// <param name="controller"></param>
	void Update(float delta, BaseScene& scene, ObjectManager& obj, Controller& controller);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="mainScr">メインのスクリーン</param>
	void Draw(int mainScr);

	const std::list<std::unique_ptr<UiBase>>& GetUiList(void) const { return uiList_; }


	/// <summary>
	/// 指定のファイルパスからuiを生成し初期化する
	/// </summary>
	/// <param name="path"> uiデータのファイルパス </param>
	/// <returns> 成功時true、失敗時false </returns>
	bool InitUi(const std::filesystem::path& path, bool isCreateCursor = true);

	/// <summary>
	/// ロード完了しているか
	/// </summary>
	/// <param name=""></param>
	/// <returns> ロード完了時true、未完了時false </returns>
	bool IsLoaded(void);
private:

	/// <summary>
	/// ファイルストリームからボタンを生成する
	/// </summary>
	/// <param name="file"> ファイルストリーム </param>
	/// <param name="num"> 数 </param>
	void CreateButton(std::ifstream& file, int num);

	/// <summary>
	/// ファイルストリームから画像(UI)を生成する
	/// </summary>
	/// <param name="file"> ファイルストリーム </param>
	/// <param name="num"> 数 </param>
	void CreateImage(std::ifstream& file, int num);

	/// <summary>
	/// ファイルストリームからスライダーを生成する
	/// </summary>
	/// <param name="file"> ファイルストリーム </param>
	/// <param name="num"> 数 </param>
	void CreateSlider(std::ifstream& file, int num);

	/// <summary>
	/// ファイルストリームからインプットセットを生成する
	/// </summary>
	/// <param name="file"> ファイルストリーム </param>
	/// <param name="num"> 数 </param>
	void CreateInputSet(std::ifstream& file, int num);

	/// <summary>
	/// ファイルストリームからロード表示を生成する
	/// </summary>
	/// <param name="file"> ファイルストリーム </param>
	/// <param name="num"> 数 </param>
	void CreateLoadIndicator(std::ifstream& file, int num);

	/// <summary>
	/// ファイルストリームからGageを生成する
	/// </summary>
	/// <param name="file"> ファイルストリーム </param>
	/// <param name="num"> 数 </param>
	void CreateGage(std::ifstream& file, int num);

	/// <summary>
	/// ファイルストリームから画像(divgraph版)を生成する
	/// </summary>
	/// <param name="file"></param>
	/// <param name="num"></param>
	void CreateImageArray(std::ifstream& file, int num);

	/// <summary>
	/// ファイルストリームからコンボ時のUIを生成する
	/// </summary>
	/// <param name="file"></param>
	/// <param name="num"></param>
	void CreateCombo(std::ifstream& file, int num);

	/// <summary>
	/// ファイルストリームからスイッチボタンを生成する
	/// </summary>
	/// <param name="file"></param>
	/// <param name="num"></param>
	void CreateSwitchButton(std::ifstream& file, int num);

	/// <summary>
	/// ファイルストリームからミニマップの作成
	/// </summary>
	/// <param name="file"></param>
	/// <param name="num"></param>
	void CreateMinMap(std::ifstream& file, int num);

	// UI用のリスト
	std::list<std::unique_ptr<UiBase>> uiList_;

	// ロード完了しているかのフラグ
	std::atomic_bool isLoaded_;
};

