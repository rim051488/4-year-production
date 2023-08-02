#pragma once
#include "ComponentConcept.h"
#include <cassert>
#include "../Object/ObjectID.h"

class ObjectManager;

// コンポーネントを持つためのクラス
template<CompC T>
class ComponentHandle
{
public:

	ComponentHandle() :
		ptr_{ nullptr }, objManager_{ nullptr }
	{

	}

	ComponentHandle(const ObjectID& id, T* ptr, ObjectManager* objManager) :
		ptr_{ ptr }, id_{ id }, objManager_{ objManager }
	{
	}


	const bool IsActive(void) const
	{
		return ptr_ != nullptr && objManager_->IsActive(id_);
	}

	T* operator->(void) const
	{
		if (!objManager_->HaveComponent<T>(id_))
		{
			assert(false);
			return nullptr;
		}
		return ptr_;
	}

	T* Get(void) const
	{
		if (!objManager_->HaveComponent<T>(id_))
		{
			return nullptr;
		}
		return ptr_;
	}
private:

	// id
	ObjectID id_;

	// コンポーネントのポインタ
	T* ptr_;

	ObjectManager* objManager_;
};

