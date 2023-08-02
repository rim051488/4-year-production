#pragma once
#include <concepts>
#include <DxLib.h>
#include <array>
#include <cassert>

// ポインタ入れられないようにコンセプトに
template<class T>
concept CBufC = std::is_class<T>::value;

// 定数バッファ用クラス
template<CBufC T>
class ConstantBuffer
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	ConstantBuffer();

	~ConstantBuffer();

	/// <summary>
	/// アップデート
	/// </summary>
	/// <param name=""></param>
	void Update(void);


	/// <summary>
	/// ハンドルの取得
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	const int& GetHandle(void) const;

	// ポインタ
	T* ptr_;
private:

	// コピー演算子とコピーコンストラクタを削除しとく
	ConstantBuffer(const ConstantBuffer&) = delete;
	ConstantBuffer& operator=(const ConstantBuffer&) = delete;

	// ハンドル
	int handle_;
};

template<CBufC T>
inline ConstantBuffer<T>::ConstantBuffer()
{
	constexpr int size = (sizeof(T) + 0x0f) & ~0x0f;
	handle_ = CreateShaderConstantBuffer(size);
	ptr_ = reinterpret_cast<T*>(GetBufferShaderConstantBuffer(handle_));
}

template<CBufC T>
inline ConstantBuffer<T>::~ConstantBuffer()
{
	DeleteShaderConstantBuffer(handle_);
	ptr_ = nullptr;
}

template<CBufC T>
inline void ConstantBuffer<T>::Update(void)
{
	UpdateShaderConstantBuffer(handle_);
}

template<CBufC T>
inline const int& ConstantBuffer<T>::GetHandle(void) const
{
	return handle_;
}

template<CBufC T, size_t size>
class ConstantBufferArray
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	ConstantBufferArray();

	~ConstantBufferArray();

	/// <summary>
	/// アップデート
	/// </summary>
	/// <param name=""></param>
	void Update(void);


	/// <summary>
	/// ハンドルの取得
	/// </summary>
	/// <param name=""></param>
	/// <returns> ハンドル(int) </returns>
	const int& GetHandle(void) const;

	/// <summary>
	/// 配列にアクセスする
	/// </summary>
	/// <param name="s"> アクセスするインデックス </param>
	/// <returns></returns>
	T& operator[](size_t idx)&;

	/// <summary>
	/// サイズの取得
	/// </summary>
	/// <param name=""> 配列のサイズ </param>
	/// <returns></returns>
	constexpr size_t Length(void) { return size; }

	
private:

	// コピー演算子とコピーコンストラクタを削除しとく
	ConstantBufferArray(const ConstantBuffer&) = delete;
	ConstantBufferArray& operator=(const ConstantBuffer&) = delete;

	std::array<T, size> array_;

	// ポインタ
	T* ptr_;

	// ハンドル
	int handle_;
};

template<CBufC T, size_t size>
inline ConstantBufferArray<T, size>::ConstantBufferArray()
{
	constexpr auto bufSize = (sizeof(T) * size + 0x0f) & ~0x0f;
	handle_ = CreateShaderConstantBuffer(bufSize);
	ptr_ = reinterpret_cast<T*>(GetBufferShaderConstantBuffer(handle_));
}

template<CBufC T, size_t size>
inline ConstantBufferArray<T, size>::~ConstantBufferArray()
{
	DeleteShaderConstantBuffer(handle_);
	ptr_ = nullptr;
}

template<CBufC T, size_t size>
inline void ConstantBufferArray<T, size>::Update(void)
{
	std::copy(array_.begin(), array_.end(), ptr_);
}

template<CBufC T, size_t size>
inline const int& ConstantBufferArray<T, size>::GetHandle(void) const
{
	return handle_;
}

template<CBufC T, size_t size>
inline T& ConstantBufferArray<T, size>::operator[](size_t idx)&
{
	assert(size > idx);
	return array_[idx];
}
