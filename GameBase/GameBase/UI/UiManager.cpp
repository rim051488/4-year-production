#include <Dxlib.h>
#include "UiManager.h"
#include "../Object/ObjectManager.h"
#include "LoadIndicatorUi.h"
#include "../Component/Behavior/PlayerBehavior.h"
#include "ComboUi.h"
#include "Gage.h"
#include "Image.h"
#include "Button.h"
#include "Cursor.h"
#include "../SceneManager.h"
#include "../Common/ResourceMng.h"
#include "Slider.h"
#include "InputSetButton.h"
#include "../Scene/OptionScene.h"
#include "../Scene/PauseScene.h"
#include "../Scene/DialogScene.h"
#include "../Scene/SelectScene.h"
#include "TutorialMessage.h"
#include "../Common/Utility.h"
#include "../Common/ThreadPool.h"
#include "SwitchButton.h"
#include "../Scene/ResultScene.h"
#include "MiniMap.h"

#include "../Common/Debug.h"



constexpr auto screenHalf{ SceneManager::screenSize_<float> / 2.0f };
constexpr auto menuSizeY{ 71.0f };

constexpr char sig[] = { 'u','i','d','a' };

constexpr float versions{ 0.0f };

struct UiDataHeader
{
	char sig[4];
	float version;
	int ckData;
};

// Uiの種類
enum class UiType
{
	Image,				// 画像
	ImageArray,			// DivGraphの配列版
	Button,				// ボタン
	Slider,				// スライダー
	InputSet,			// 入力セット
	Gage,				// ゲージ
	Combo,				// コンボ
	LoadIndicator,		// ロード時の表示
	SwitchButton,		// オンオフできるボタン
	MinMap
};

// 
enum class ButtonFuncID
{
	Tutorial,       // チュートリアル
	Play,           // ゲーム開始
	SelectOption,   // セレクトシーンからオプション
	SelectDialog,   // セレクトシーンからゲーム終了ダイアログ
	PauseBack,      // ポーズシーン戻る
	PauseOption,    // ポーズシーンからオプション
	PauseDialog,    // ポーズシーンから終了ダイアログ
	OptionBack,     // オプション戻る
	OptionOk,       // オプションOK
	DialogOk,       // ダイアログOK
	DialogCancel    // ダイアログキャンセル
};


UiManager::UiManager(const std::filesystem::path& path, bool isAsync, bool isTutorial, bool isCreateCursor)
{
	isLoaded_.store(false);
	if (isTutorial)
	{
		uiList_.emplace_back(std::make_unique<TutorialMessage>());
	}
	if (!isAsync)
	{
		InitUi(path, isCreateCursor);
		isLoaded_.store(true);
		return;
	}
	// スレッドプールクラス使ってロードする
	Thread.Add([this, path, isCreateCursor]() { InitUi(path, isCreateCursor); isLoaded_.store(true); });
	
}


bool UiManager::InitUi(const std::filesystem::path& path, bool isCreateCursor)
{
	std::ifstream file{ path , std::ios::binary};

	if (!file)
	{
		DebugLog("ファイルを開けませんでした");
		return false;
	}

	UiDataHeader header{};
	file.read(reinterpret_cast<char*>(&header), sizeof(header));
	
	if (versions != header.version)
	{
		// バージョンが一致していないとき
		DebugLog("ファイルのバージョンが一致していません");
		return false;
	}

	for (int i = 0; i < 4; i++)
	{
		if (sig[i] != header.sig[i])
		{
			// シグネチャが一致してないとき
			DebugLog("ファイルのシグネチャが正しくありません");
			return false;
		}
	}

	// 生成用関数のマップ
	const std::map<UiType, void(UiManager::*)(std::ifstream&, int)> createFuncs
	{
		{UiType::Button, &UiManager::CreateButton},
		{UiType::Image, &UiManager::CreateImage},
		{UiType::ImageArray, &UiManager::CreateImageArray},
		{UiType::Slider, &UiManager::CreateSlider},
		{UiType::InputSet, &UiManager::CreateInputSet},
		{UiType::LoadIndicator, &UiManager::CreateLoadIndicator},
		{UiType::Gage, &UiManager::CreateGage},
		{UiType::Combo, &UiManager::CreateCombo},
		{UiType::SwitchButton, &UiManager::CreateSwitchButton},
		{UiType::MinMap, &UiManager::CreateMinMap}
	};

	while (true)
	{
		// Uiの種類を読み取る
		UiType type;
		file.read(reinterpret_cast<char*>(&type), sizeof(type));
		
		if (file.eof())
		{
			// ファイルの末端まで読み終わっていた時
			break;
		}

		// その種類のUIの数を取得
		int num{ 0 };
		file.read(reinterpret_cast<char*>(&num), sizeof(num));

		
		// 生成する
		if (createFuncs.contains(type))
		{
			(this->*createFuncs.at(type))(file, num);
		}

	}

	if (isCreateCursor)
	{
		// カーソル生成するとき
		uiList_.emplace_back(std::make_unique<Cursor>());
	}
	return true;
}

bool UiManager::IsLoaded(void)
{
	return isLoaded_.load();
}


void UiManager::CreateButton(std::ifstream& file, int num)
{
	// ボタンの処理
	std::vector<std::function<void(BaseScene&)>> funcs{
		[](BaseScene& scene) { static_cast<SelectScene&>(scene).Tutorial(); },
		[](BaseScene& scene) { static_cast<SelectScene&>(scene).Play(); },
		[](BaseScene& scene) { static_cast<SelectScene&>(scene).Option(); },
		[](BaseScene& scene) { static_cast<SelectScene&>(scene).Dialog(); },
		[](BaseScene& scene) {  static_cast<PauseScene&>(scene).BackGame(); },
		[](BaseScene& scene) { static_cast<PauseScene&>(scene).Option(); },
		[](BaseScene& scene) { static_cast<PauseScene&>(scene).Dialog(); },
		[](BaseScene& scene) {  static_cast<OptionScene&>(scene).Close(); },
		[](BaseScene& scene) { static_cast<OptionScene&>(scene).Save(); static_cast<OptionScene&>(scene).Close(); },
		[](BaseScene& scene) { static_cast<DialogScene&>(scene).Ok(); },
		[](BaseScene& scene) {static_cast<DialogScene&>(scene).Cancel(); },
		[](BaseScene& scene) { scene.ChangeSceneID(SceneID::Game); },
		[](BaseScene& scene) { scene.ChangeSceneID(SceneID::Select); },
		[](BaseScene& scene) { scene.ChangeSceneID(SceneID::Dialog); },
		[](BaseScene& scene) { static_cast<OptionScene&>(scene).Default(); },
		[](BaseScene& scene) { static_cast<ResultScene&>(scene).NextStage(); }
	};

	// 処理のID
	int funcID{0};

	// 画像名
	std::string imgName;

	// 枠画像名
	std::string frameName;

	// 座標
	Vector2 pos;
	for (int i = 0; i < num; i++)
	{
		// 処理のIDを読み取る
		file.read(reinterpret_cast<char*>(&funcID), sizeof(funcID));

		// 画像名を読み取る
		imgName = ReadString(file);

		// 枠画像名を読み取る
		frameName = ReadString(file);

		// 座標を読み取る
		file.read(reinterpret_cast<char*>(&pos), sizeof(pos));

		// ボタン生成
		uiList_.emplace_back(std::make_unique<Button>(
			funcs[funcID],
			imgName,
			frameName,
			pos
		));
	}
}

void UiManager::CreateImage(std::ifstream& file, int num)
{
	// 座標
	Vector2 pos;

	// 画像名
	std::string imgName;

	// 表示の種類
	ViewType viewType{ 0 };
	for (int i = 0; i < num; i++)
	{
		// 座標を読み取る
		file.read(reinterpret_cast<char*>(&pos), sizeof(pos));

		// 画像名を読み取る
		imgName = ReadString(file);

		// 表示の種類を読み取る
		file.read(reinterpret_cast<char*>(&viewType), sizeof(viewType));

		// 画像を生成
		uiList_.emplace_back(
			std::make_unique<Image>(imgName,pos, viewType)
		);
	}
}

void UiManager::CreateSlider(std::ifstream& file, int num)
{
	std::vector< std::pair<std::function<void(const float)>, float>> funcs
	{
		{[](const float v) { lpSooundPross.SetSEVolumeUserSet(v); },lpSooundPross.GetSEVolumeUserSet()},
		{[](const float v) { lpSooundPross.SetBGMVolumeUserSet(v); }, lpSooundPross.GetBGMVolumeUserSet()},
		{[](const float v) { lpConfigMng.SetCameraSpeed(v); },lpConfigMng.GetCameraSpeed()} 
	};

	// 処理のID
	int funcID{ 0 };

	// バーの画像名
	std::string barImgName;

	// タブの画像名
	std::string tabImgName;

	std::string valueImgName;

	// 開始時の値
	float startValue{ 0.0f };

	// 座標
	Vector2 pos;
	for (int i = 0; i < num; i++)
	{
		// 処理の種類を読み取る
		file.read(reinterpret_cast<char*>(&funcID), sizeof(funcID));

		// バーの画像名を読み取る
		barImgName = ReadString(file);

		// タブの画像名を読み取る
		tabImgName = ReadString(file);

		// 値の部分の画像名を読み取る
		valueImgName = ReadString(file);

		// 座標を読み取る
		file.read(reinterpret_cast<char*>(&pos), sizeof(pos));

		// スライダー生成
		uiList_.emplace_back(std::make_unique<Slider>(
			funcs[funcID].first,
			barImgName,
			tabImgName,
			valueImgName,
			funcs[funcID].second, pos
		));
	}
}

void UiManager::CreateInputSet(std::ifstream& file, int num)
{
	// 入力のID
	InputID id;

	// 座標
	Vector2 pos;
	for (int i = 0; i < num; i++)
	{
		// 入力の種類を読み取る
		file.read(reinterpret_cast<char*>(&id), sizeof(id));

		// 座標を読み取る
		file.read(reinterpret_cast<char*>(&pos), sizeof(pos));

		// 入力設定のUIを生成
		uiList_.emplace_back(std::make_unique<InputSetButton>(id,pos));
	}
}

void UiManager::CreateLoadIndicator(std::ifstream& file, int num)
{
	// 画像名1
	std::string imgName1;

	// 画像名2
	std::string imgName2;

	// 座標
	Vector2 pos;
	for (int i = 0; i < num; i++)
	{
		// 画像名1を読み取る
		imgName1 = ReadString(file);

		// 画像名2を読み取る
		imgName2 = ReadString(file);

		// 座標を読み取る
		file.read(reinterpret_cast<char*>(&pos), sizeof(pos));

		// ロードの表示を生成
		uiList_.emplace_back(std::make_unique<LoadIndicatorUi>(imgName1,pos,imgName2));
	}
}

void UiManager::CreateGage(std::ifstream& file, int num)
{
	std::vector< std::function<float(ObjectManager&)>> funcs
	{
		[](ObjectManager& objMng) {
				auto player = objMng.GetComponent<PlayerBehavior>(objMng.GetPlayerID());
				auto [value, minmax] = player->GetGaugeValue(UiID::Hp);
				return value;
			},
		[](ObjectManager& objMng) {
				auto player = objMng.GetComponent<PlayerBehavior>(objMng.GetPlayerID());
				auto [value, minmax] = player->GetGaugeValue(UiID::Skill);
				return value;
			},
		[](ObjectManager& objMng) {
			auto player = objMng.GetComponent<PlayerBehavior>(objMng.GetPlayerID());
			auto [value, minmax] = player->GetGaugeValue(UiID::Stamina);
			return value;
		}
	};

	// 処理の種類
	int funcID{ 0 };

	// 画像名
	std::string imgName;

	// 枠画像名
	std::string frameName;

	// 開始時の値
	float startValue{ 0.0f };

	// 座標
	Vector2 pos;
	for (int i = 0; i < num; i++)
	{
		// 処理の種類
		file.read(reinterpret_cast<char*>(&funcID), sizeof(funcID));

		// 画像名を読み取る
		imgName = ReadString(file);

		// 枠画像名を読み取る
		frameName = ReadString(file);

		// 開始時の値を読み取る
		file.read(reinterpret_cast<char*>(&startValue), sizeof(startValue));

		// 座標を読み取る
		file.read(reinterpret_cast<char*>(&pos), sizeof(pos));

		// ゲージを生成
		uiList_.emplace_back(
			std::make_unique<Gage>(
				funcID, funcs[funcID],imgName,frameName,startValue, pos
				));
	}
}

void UiManager::CreateImageArray(std::ifstream& file, int num)
{
	// 画像名
	std::string imageName;

	// 分割数
	Vector2I div;

	// 分割サイズ
	Vector2I size;

	// 座標
	Vector2 pos;

	// インターバル値
	float interval{0.0f};
	for (int i = 0; i < num; i++)
	{
		// 画像名を読み取る
		imageName = ReadString(file);

		// 分割数を読み取る
		file.read(reinterpret_cast<char*>(&div), sizeof(div));

		// 分割サイズを読み取る
		file.read(reinterpret_cast<char*>(&size), sizeof(size));

		// 座標を読み取る
		file.read(reinterpret_cast<char*>(&pos), sizeof(pos));

		// インターバルを読み取り
		file.read(reinterpret_cast<char*>(&interval), sizeof(interval));

		// DivGraphを使った画像を生成
		uiList_.emplace_back(std::make_unique<Image>(
				imageName,div,size,pos,Vector2{0.0f, interval}
		));
	}
}

void UiManager::CreateCombo(std::ifstream& file, int num)
{
	// 数値画像名
	std::string numImgName;

	// 分割数
	Vector2I div;

	// 分割サイズ
	Vector2I size;

	// 画像名
	std::string imgName;

	// 枠画像名
	std::string frameName;

	// 開始時の値
	float startValue{0.0f};

	// 座標
	Vector2 pos;

	// インターバル
	Vector2 inerval;
	for (int i = 0; i < num; i++)
	{
		// 数値画像名を読み取る
		numImgName = ReadString(file);

		// 分割数を読み取る
		file.read(reinterpret_cast<char*>(&div), sizeof(div));

		// サイズを読み取る
		file.read(reinterpret_cast<char*>(&size), sizeof(size));

		// 画像名を読み取る
		imgName = ReadString(file);

		// 枠画像名を読み取る
		frameName = ReadString(file);

		// 開始時の値を読み取る
		file.read(reinterpret_cast<char*>(&startValue), sizeof(startValue));

		// 座標を読み取る
		file.read(reinterpret_cast<char*>(&pos), sizeof(pos));

		// インターバルを読み取る
		file.read(reinterpret_cast<char*>(&inerval), sizeof(inerval));

		// コンボを生成
		uiList_.emplace_back(
			std::make_unique<ComboUi>(
				numImgName,
				div,
				size,
				imgName,
				pos,
				inerval
				));
	}
}

void UiManager::CreateSwitchButton(std::ifstream& file, int num)
{
	// ボタンの処理
	std::vector<std::pair<SwitchButton::UpdateFunc,SwitchButton::PushFunc>> funcs{
		{[](BaseScene& scene) {  return static_cast<OptionScene&>(scene).IsPeMono();  },[](BaseScene& scene, bool isOn) { static_cast<OptionScene&>(scene).SetPeMono(isOn);  } },
		{[](BaseScene& scene) {  return static_cast<OptionScene&>(scene).IsPeFog();  },[](BaseScene& scene, bool isOn) { static_cast<OptionScene&>(scene).SetPeFog(isOn);  } }
	};

	// 処理のID
	int funcID{ 0 };

	// オンの時の画像名
	std::string onImgName;

	// オフの時の画像名
	std::string offImgName;

	// 枠画像名
	std::string frameName;

	// 座標
	Vector2 pos;

	for (int i = 0; i < num; i++)
	{
		// 処理のIDを読み取る
		file.read(reinterpret_cast<char*>(&funcID), sizeof(funcID));

		// オンの時の画像名を読み取る
		onImgName = ReadString(file);

		// オフの時の画像名を読み取る
		offImgName = ReadString(file);

		// 枠画像名を読み取る
		frameName = ReadString(file);

		// 座標を読み取る
		file.read(reinterpret_cast<char*>(&pos), sizeof(pos));

		// ボタン生成
		uiList_.emplace_back(std::make_unique<SwitchButton>(
			funcs[funcID].second,
			funcs[funcID].first,
			onImgName,
			offImgName,
			frameName,
			pos
			));
	}
}

void UiManager::CreateMinMap(std::ifstream& file, int num)
{
	for (int i = 0; i < num; i++)
	{
		uiList_.emplace_back(std::make_unique<MiniMap>(ReadString(file)));
	}
}


UiManager::~UiManager()
{
	uiList_.clear();
}


void UiManager::Begin(BaseScene& scene)
{
	for (auto& ui : uiList_)
	{
		ui->Loaded(scene);
	}
}

void UiManager::Update(float delta, BaseScene& scene, ObjectManager& obj, Controller& controller)
{
	if (scene.GetID() == SceneID::Game)
	{
		auto player = obj.GetComponent<PlayerBehavior>(obj.GetPlayerID());
		if (!player.IsActive())
		{
			uiList_.clear();
		}
	}

	for (auto& ui : uiList_)
	{
		ui->Update(delta,scene, obj, controller);
	}

	auto cursor = std::find_if(uiList_.begin(), uiList_.end(), [](std::unique_ptr<UiBase>& ui) { return ui->GetUIID() == UiID::Cursor; });
	if (cursor != uiList_.end())
	{
		static_cast<Cursor*>((*cursor).get())->Check(uiList_, scene,controller);
	}
}

void UiManager::Draw(int mainScr)
{
	for (auto& ui : uiList_)
	{
		ui->Draw(mainScr);
	}
}
