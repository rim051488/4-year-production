#include <fstream>
#include <DxLib.h>
#include <vector>
#include "InputConfigMng.h"
#include "InputConfig.h"
#include "../../SoundPross.h"
#include "../../Debug.h"

constexpr char path[] = "Resource/Other/Input/";
InputConfig* InputConfig::sInstance_ = nullptr;

// PadTypeをキーに合わせてコンフィグファイルの名前とデフォルトに戻す関数を持つテーブル
InputConfig::TypeDataTbl InputConfig::typeDataTbl_{
	{DX_PADTYPE_DUAL_SHOCK_4,std::make_pair("padConfigPS.data",&InputConfig::SetPsDefalutCode)},
	{DX_PADTYPE_DUAL_SENSE,std::make_pair("padConfigPS.data",&InputConfig::SetPsDefalutCode)},
	{DX_PADTYPE_XBOX_360, std::make_pair("padConfigXbox.data", &InputConfig::SetXboxDefalutCode)},
	{DX_PADTYPE_XBOX_ONE, std::make_pair("padConfigXbox.data", &InputConfig::SetXboxDefalutCode)},
	{-1, std::make_pair("KeyboardConfig.data", &InputConfig::SetKeyDefalutCode)}
};

// サウンド設定のデータのパス
constexpr char soundFile[] = "Resource/Other/Input/Sound.data";

void InputConfig::Create(void)
{
	if (sInstance_ == nullptr)
	{
		sInstance_ = new InputConfig();
	}
}

void InputConfig::Destroy(void)
{
	if (sInstance_ != nullptr)
	{
		delete sInstance_;
	}
	sInstance_ = nullptr;
}

InputConfig& InputConfig::GetInstance(void)
{
	return *sInstance_;
}



void InputConfig::ChangeInputCode(InputID id, int code)
{
	// 変更前の値をもっとく
	auto tmp = inputCode_[id];
	inputCode_[id] = code;
	auto itr = std::find_if(inputCode_.begin(), inputCode_.end(), [code](auto& data) { return data.second == code; });
	if (itr != inputCode_.end())
	{
		// 変更前の値を入れる
		itr->second = tmp;
	}
}

void InputConfig::SetDefalutCode(void)
{
	if (typeDataTbl_.contains(nowType_))
	{
		// 現在のタイプがテーブルに存在する時
		(this->*typeDataTbl_[nowType_].second)();
	}
	else
	{
		// しないときデフォルトをセットする
		SetKeyDefalutCode();
	}
}

void InputConfig::SetPsDefalutCode(void)
{
	inputCode_.emplace(InputID::Attack, 1);
	inputCode_.emplace(InputID::Dash, 0);
	inputCode_.emplace(InputID::Skil, 3);
	inputCode_.emplace(InputID::Jump, 0);
	camSpeed_ = 0.5f;
}

void InputConfig::SetXboxDefalutCode(void)
{
	inputCode_.emplace(InputID::Attack, 1);
	inputCode_.emplace(InputID::Dash, 4);
	inputCode_.emplace(InputID::Skil, 2);
	inputCode_.emplace(InputID::Jump, 0);
	camSpeed_ = 0.5f;
}

void InputConfig::SetKeyDefalutCode(void)
{
	inputCode_.emplace(InputID::Dash, KEY_INPUT_LSHIFT);
	inputCode_.emplace(InputID::Skil, KEY_INPUT_E);
	inputCode_.emplace(InputID::Attack, -MOUSE_INPUT_LEFT);
	inputCode_.emplace(InputID::Jump, KEY_INPUT_SPACE);
	camSpeed_ = 0.25f;
}

const InputConfig::PeConfig& InputConfig::GetPeConfig(void) const&
{
	return peConfig_;
}

void InputConfig::SetPeConfig(PEID id, bool flag)
{
	peConfig_[id] = flag;
}

InputConfig::InputConfig()
{
	// Inputの初期化
	InitInput();

	// サウンドの初期化
	InitSound();
	
	// ポストプロセスの初期化
	peConfig_.reserve(2);
	peConfig_[PEID::Mono] = false;
	peConfig_[PEID::VolFog] = true;

	
}

void InputConfig::InitSound()
{
	// サウンド設定ファイルを読み込む
	std::ifstream file{ soundFile, std::ios::binary };

	// とりあえず1.0fで初期化
	auto se = 1.0f;
	auto bg = 1.0f;
	if (file)
	{
		// ファイルを読み込めた時
		std::uint64_t ck;
		file.read(reinterpret_cast<char*>(&se), sizeof(se));
		file.read(reinterpret_cast<char*>(&bg), sizeof(bg));
		file.read(reinterpret_cast<char*>(&ck), sizeof(ck));

		if (std::hash<float>()(se + bg) != ck)
		{
			// チェックで値が一致していないときデフォルトにする
			se = lpSooundPross.GetDefaultSEVolume();
			bg = lpSooundPross.GetDefaultBGMVolume();
		}
	}

	// セットする
	lpSooundPross.SetSEVolumeUserSet(se);
	lpSooundPross.SetBGMVolumeUserSet(bg);
}

void InputConfig::InitInput()
{
	nowType_ = GetJoypadType(DX_INPUT_PAD1);

	camSpeed_ = 0.5f;
	if (typeDataTbl_.contains(nowType_))
	{
		// 現在のタイプがテーブルに存在する時設定ファイルをロードする
		if (!Load(typeDataTbl_[nowType_].first))
		{
			// ロード失敗時現在のタイプのデフォルトにする関数を実行する
			(this->*typeDataTbl_[nowType_].second)();
		}
	}
	else
	{
		// ないときキーボードの方でデフォルトにする
		nowType_ = -1;
		(this->*typeDataTbl_[nowType_].second)();
	}
}

InputConfig::~InputConfig()
{
	if (typeDataTbl_.contains(nowType_))
	{
		Save(typeDataTbl_[nowType_].first);
	}

	std::ofstream file{ soundFile, std::ios::binary };
	if (file)
	{
		auto ck = std::hash<float>()(lpSooundPross.GetSEVolumeUserSet() + lpSooundPross.GetBGMVolumeUserSet());
		auto se = lpSooundPross.GetSEVolumeUserSet();
		auto bg = lpSooundPross.GetBGMVolumeUserSet();
		file.write(reinterpret_cast<char*>(&se), sizeof(se));
		file.write(reinterpret_cast<char*>(&bg), sizeof(bg));
		file.write(reinterpret_cast<char*>(&ck), sizeof(ck));
	}
}

bool InputConfig::Load(const std::string& fname)
{
	std::ifstream ifs(path + fname);
	if (!ifs)
	{
		return false;
	}
	Header h{};
	std::vector<Data> vec;
	// ヘッダー情報を読み込む
	ifs.read(reinterpret_cast<char*>(&h), sizeof(h));
	// 設定データを読み込む
	vec.resize(h.size);
	ifs.read(reinterpret_cast<char*>(vec.data()), sizeof(vec[0]) * h.size);
	int sum = 0;
	// 以下sum値チェック
	for (auto& v : vec)
	{
		sum += static_cast<int>(v.id) * v.code;
	}
	if (sum != h.sum)
	{
		return false;
	}
	// マップに入れる
	for (auto& v : vec)
	{
		inputCode_.emplace(v.id, v.code);
	}
	return true;
}

bool InputConfig::Save(const std::string& fname)
{
	std::ofstream ofs(path + fname, std::ios::binary);
	if (!ofs)
	{
		return false;
	}
	Header h{};
	std::vector<Data> vec;
	//std::vector<Data> vec(inputCode_.size());
	vec.reserve(inputCode_.size());
	for (auto& code : inputCode_)
	{
		vec.emplace_back(Data{ code.first, code.second });
	}

	// ヘッダ情報を作る
	h.size = vec.size();
	h.ver = 1.0f;
	for (auto& v : vec)
	{
		h.sum += static_cast<int>(v.id) * v.code;
	}
	// 書き込み
	ofs.write(reinterpret_cast<char*>(&h), sizeof(h));
	ofs.write(reinterpret_cast<char*>(vec.data()), sizeof(vec[0]) * vec.size());
	return true;
}
