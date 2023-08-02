#pragma once
#include <filesystem>
#include <map>
#include <mutex>
#include <list>
#include <tuple>
#include "SharedHandle.h"
#include "ScreenID.h"

// 既に読み込んだリースが有れば返すためのクラス
class ResourceMng
{
	using HandleFlagPair = std::pair<std::shared_ptr<int>, bool>;
	using HandleMap = std::map<std::uint64_t, HandleFlagPair>;
	using HandleDivMap = std::map <std::uint64_t, std::tuple<std::vector<int>, std::shared_ptr<int>, bool>>;
	using ScreenMap = std::map<ScreenID, HandleFlagPair>;
	using ModelMap = std::map<std::uint64_t, std::tuple<std::shared_ptr<int>, bool>>;

public:
	ResourceMng();
	~ResourceMng();

	/// <summary>
	/// 画像の読み込み
	/// </summary>
	/// <param name="path"> 画像のfileパス </param>
	/// <param name="isNotRelese"> Releseが呼ばれた時に削除しないか? </param>
	void LoadTexture(SharedGraphicHandle& out,const std::filesystem::path& path, bool isNotRelese = false);

	/// <summary>
	/// 画像の分割読み込み
	/// </summary>
	/// <param name="out"></param>
	/// <param name="path"> 画像のfileパス </param>
	/// <param name="divCount"></param>
	/// <param name="divSize"></param>
	/// <param name="isNotRelese"> Releseが呼ばれた時に削除しないか? </param>
	void LoadDivTexture(SharedDivGraphicHandle& out, const std::filesystem::path& path, const Vector2I& divCount, const Vector2I& divSize, bool isNotRelese = false);

	/// <summary>
	/// 上記のMV1版
	/// </summary>
	/// <param name="path"></param>
	/// <param name="isNotRelese"></param>
	/// <returns></returns>
	void LoadMv1Texture(SharedGraphicHandle& out,const std::filesystem::path& path, bool isNotRelese = false);

	/// <summary>
	/// 一致するグラフィックハンドルを管理から削除する
	/// </summary>
	/// <param name="handle"></param>
	void Remove(SharedGraphicHandle& handle);

	/// <summary>
	/// 一致するグラフィックハンドルを管理から削除する
	/// </summary>
	/// <param name="handle"></param>
	void Remove(SharedDivGraphicHandle& handle);

	/// <summary>
	/// レンダーターゲットを作成する
	/// </summary>
	/// <param name="id"> ID </param>
	/// <param name="size"> サイズ </param>
	/// <param name="alpha"> アルファチャンネルを使うか </param>
	/// <param name="isNotRelese"> Releseが呼ばれた時に削除しないか </param>
	/// <returns></returns>
	void MakeRenderTarget(SharedRenderTargetHandle& out,const ScreenID id,const Vector2& size, bool alpha, bool isNotRelese = false);

	/// <summary>
	/// 削除する
	/// </summary>
	/// <param name="id"></param>
	void Remove(const ScreenID id);

	/// <summary>
	/// modelをロードする
	/// </summary>
	/// <param name="path"></param>
	/// <returns></returns>
	void LoadModel(SharedModelHandle& out,const std::filesystem::path& path, bool isNotRelese = false);

	/// <summary>
	/// 一致するモデルのハンドルを管理から削除する
	/// </summary>
	/// <param name="handle"></param>
	void RemoveModel(const int handle);

	/// <summary>
	/// サウンドのロード
	/// </summary>
	/// <param name="path"> サウンドファイルのパス </param>
	/// <param name="isNotRelese"> Releseが呼ばれた時に削除しないか </param>
	/// <returns></returns>
	void LoadSound(SharedSoundHandle& out,const std::filesystem::path& path, bool isNotRelese);


	/// <summary>
	/// 一致するサウンドのハンドルを管理から削除する
	/// </summary>
	/// <param name="handle"></param>
	void RemoveSound(const int handle);

	/// <summary>
	/// ピクセルシェーダを読み込む
	/// </summary>
	/// <param name="path"></param>
	/// <param name="isNotRelese"></param>
	/// <returns></returns>
	void LoadPS(SharedShaderHandle& out,const std::filesystem::path& path, bool isNotRelese = false);

	/// <summary>
	/// 頂点シェーダを読み込む
	/// </summary>
	/// <param name="path"></param>
	/// <param name="isNotRelese"></param>
	/// <returns></returns>
	void LoadVS(SharedShaderHandle& out,const std::filesystem::path& path, bool isNotRelese = false);

	/// <summary>
	/// 一致するシェーダハンドルが管理から削除すべきか
	/// </summary>
	/// <param name="handle"></param>
	/// <returns></returns>
	bool IsRemove(SharedShaderHandle& handle);

	/// <summary>
	/// 一致するシェーダハンドルを管理から削除する
	/// </summary>
	/// <param name="handle"></param>
	void Remove(SharedShaderHandle& handle);

	/// <summary>
	/// エフェクシアのエフェクトリソースを読み込む
	/// </summary>
	/// <param name="out"></param>
	/// <param name="path"></param>
	/// <param name="isNotRelese"></param>
	void LoadEffect(SharedEffectHandle& out, const std::filesystem::path& path, bool isNotRelese = false);

	/// <summary>
	/// エフェクシアのエフェクトリソースを削除する
	/// </summary>
	/// <param name="handle"></param>
	void Remove(SharedEffectHandle& handle);


	void Loaded(void);
private:
	std::mutex mutex_;

	// ロードしたパスをキーに解放すべきかのフラグとそれぞれのハンドルを持つ
	HandleMap imageMap_;
	HandleDivMap imageDivMap_;
	ScreenMap screenMap_;
	HandleMap soundMap_;
	ModelMap modelMap_;
	HandleMap shaderMap_;
	HandleMap effectMap_;

	std::list<std::function<void(void)>> loadedFunc_;
};

