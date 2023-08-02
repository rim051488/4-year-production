#pragma once
#include <map>
#include <memory>
#include <functional>
#include <span>
#include "Math.h"
#include "ScreenID.h"

// DxLibのハンドルをリークしないように共有するためのクラス
class SharedHandle
{
public:
	SharedHandle(){}
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="handle"> 管理を任せるハンドル </param>
	SharedHandle(const int handle);

	/// <summary>
	/// ムーブコンストラクタ
	/// </summary>
	/// <param name="sharedHandle"></param>
	/// <returns></returns>
	SharedHandle(SharedHandle&& sharedHandle) noexcept;

	/// <summary>
	/// コピーコンストラクタ
	/// </summary>
	/// <param name="sharedHandle"></param>
	/// <returns></returns>
	SharedHandle(const SharedHandle& sharedHandle) noexcept;

	/// <summary>
	/// デストラクタ
	/// </summary>
	/// <returns></returns>
	virtual ~SharedHandle() noexcept;


	SharedHandle& operator=(SharedHandle&& sharedHandle) noexcept;

	/// <summary>
	/// ハンドルを取得するため
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	virtual const int operator*(void) const noexcept;

	void SetPtr(std::shared_ptr<int>& ptr);

protected:

	// ハンドルを管理するためのshared_ptr
	std::shared_ptr<int> ptr_;
};

// グラフィック用
class SharedGraphicHandle :
	public SharedHandle
{
public:
	SharedGraphicHandle(){}
	explicit SharedGraphicHandle(const int handle) ;
	SharedGraphicHandle(const SharedGraphicHandle& sharedHandle) noexcept;
	~SharedGraphicHandle();
	SharedGraphicHandle& operator=(const SharedGraphicHandle& sharedHandle) noexcept;
};

// 分割グラフィック用
class SharedDivGraphicHandle :
	public SharedHandle
{
public:
	SharedDivGraphicHandle() {}
	explicit SharedDivGraphicHandle(const int handle);
	SharedDivGraphicHandle(const SharedDivGraphicHandle& sharedHandle) noexcept;
	~SharedDivGraphicHandle();
	SharedDivGraphicHandle& operator=(const SharedDivGraphicHandle& sharedHandle) noexcept;


	/// <summary>
	/// 指定のインデックスの中のハンドルにアクセスする
	/// </summary>
	/// <param name="idx"> インデックス </param>
	/// <returns> ハンドル </returns>
	const int  operator[](size_t idx) const;

	/// <summary>
	/// 参照するspanをセットする
	/// </summary>
	/// <param name="handles"> 参照元のspan </param>
	void Set(const std::span<int> handles);
private:
	// spanで参照できるように持つ
	std::span<int> handles_;
};

// スクリーン用
class SharedRenderTargetHandle :
	public SharedHandle
{
public:
	SharedRenderTargetHandle() : id_{} {}
	explicit SharedRenderTargetHandle(const int handle, const ScreenID id);
	SharedRenderTargetHandle(const SharedRenderTargetHandle& sharedHandle) noexcept;
	~SharedRenderTargetHandle();
	SharedRenderTargetHandle& operator=(const SharedRenderTargetHandle& sharedHandle) noexcept;
	void SetScreenID(ScreenID id) { id_ = id; }
private:
	ScreenID id_;
};

// MV1model用
class SharedModelHandle :
	public SharedHandle
{
public:
	SharedModelHandle(){}
	~SharedModelHandle();
	SharedModelHandle& operator=(const SharedModelHandle& sharedHandle) noexcept;
	void CopyParent(void);
	void SetHandle(const int handle);
	void SetParent(std::shared_ptr<int>& parent);
	const int operator*(void) const noexcept final;
private:
	int handle_{-1};
};

// シェーダ用
class SharedShaderHandle :
	public SharedHandle
{
public:
	SharedShaderHandle(){}
	explicit SharedShaderHandle(const int handle);
	SharedShaderHandle(const SharedShaderHandle& sharedHandle) noexcept;
	~SharedShaderHandle();
	SharedShaderHandle& operator=(const SharedShaderHandle& sharedHandle) noexcept;
};

// サウンド用
class SharedSoundHandle :
	public SharedHandle
{
public:
	SharedSoundHandle() {}
	~SharedSoundHandle();
	SharedSoundHandle& operator=(const SharedSoundHandle& sharedHandle) noexcept;
	void SetHandle(const int handle);
	void CopyParent(void);
	int handle_{-1};
};

// フォント用
class SharedFontHandle :
	public SharedHandle
{
public:
	SharedFontHandle() {}
	SharedFontHandle(const int handle);
	SharedFontHandle(const SharedFontHandle& sharedHandle) noexcept;
	~SharedFontHandle();
	SharedFontHandle& operator=(const SharedFontHandle& sharedHandle) noexcept;
};

class SharedEffectHandle :
	public SharedHandle
{
public:
	SharedEffectHandle() {}
	explicit SharedEffectHandle(const int handle);
	SharedEffectHandle(const SharedEffectHandle& sharedHandle) noexcept;
	~SharedEffectHandle();
	SharedEffectHandle& operator=(const SharedEffectHandle& sharedHandle) noexcept;
};