#pragma once
#include <memory>
#include <unordered_map>
#include <forward_list>
#include "ComponentID.h"
#include "ComponentConcept.h"

#include "../Common/Debug.h"

class ComponentBase;

// 使い回したいコンポーネントをプールするためのクラス
class ComponentPool
{
	using CompUptr = std::unique_ptr<ComponentBase>;
	using CompFList = std::forward_list<CompUptr>;
public:
	ComponentPool();
	~ComponentPool();

	/// <summary>
	/// プールから指定の型を取り出す
	/// </summary>
	/// <typeparam name="T"> 取り出すコンポーネントの型 </typeparam>
	/// <param name=""></param>
	/// <returns> 取り出したコンポーネント </returns>
	template<CompC T>
	std::unique_ptr<T> Pop(void);

	/// <summary>
	/// コンポーネントをプールに格納する
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="comp"> 格納するコンポーネント </param>
	template<CompC T>
	void Push(std::unique_ptr<T>&& comp);

private:

	// コンポーネントのプール
	std::unordered_map<ComponentID, CompFList> compPool_;
};

template<CompC T>
inline std::unique_ptr<T> ComponentPool::Pop(void)
{
	if (!compPool_[T::id_].empty())
	{
		// プールが空だったら生成する
		auto ptr = compPool_[T::id_].front().get();
		compPool_[T::id_].front().release();
		compPool_[T::id_].pop_front();
		//DebugLog("Pop");
		return std::unique_ptr<T>{static_cast<T*>(ptr)};
	}
	else
	{
		// 無かったら生成する
		return std::move(std::make_unique<T>());
	}
}

template<CompC T>
inline void ComponentPool::Push(std::unique_ptr<T>&& comp)
{
	compPool_[comp->GetID()].emplace_front(std::move(comp));
}
