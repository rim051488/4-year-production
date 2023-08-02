#include "SharedHandle.h"
#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include "../SceneManager.h"
#include "../Common/Debug.h"
#include "ResourceMng.h"


SharedHandle::SharedHandle(const int handle)
{
	ptr_ = std::make_shared<int>();
	*ptr_ = handle;
}

//SharedHandle::SharedHandle(const std::vector<std::map<std::uint64_t, std::pair<std::shared_ptr<int>, bool>>> handle)
//{
//	ptr_ = std::make_shared<int>();
//	*ptr_ = handle;
//}

SharedHandle::SharedHandle(const SharedHandle& sharedHandle) noexcept :
	ptr_{sharedHandle.ptr_}
{
}

SharedHandle::SharedHandle(SharedHandle&& sharedHandle) noexcept :
	ptr_{std::move(sharedHandle.ptr_)}
{
}

SharedHandle::~SharedHandle() noexcept
{
}


SharedHandle& SharedHandle::operator=(SharedHandle&& sharedHandle) noexcept
{
	ptr_ = std::move(sharedHandle.ptr_);
	return *this;
}

const int SharedHandle::operator*(void) const noexcept
{
	return (ptr_ ? *ptr_ : -1);
}

void SharedHandle::SetPtr(std::shared_ptr<int>& ptr)
{
	ptr_ = ptr;
}


SharedGraphicHandle::SharedGraphicHandle(const int handle) :
	SharedHandle{handle}
{
}

SharedGraphicHandle::SharedGraphicHandle(const SharedGraphicHandle& sharedHandle) noexcept :
	SharedHandle{sharedHandle}
{
}

SharedGraphicHandle::~SharedGraphicHandle()
{
	if (ptr_ && ptr_.use_count() <= 2)
	{
		lpSceneMng.GetResourceMng().Remove(*this);
		if (ptr_.use_count() == 1)
		{
			// 最後なのでDelete呼ぶ
			DeleteGraph(*ptr_);
			ptr_.reset();
		}
	}
}

SharedGraphicHandle& SharedGraphicHandle::operator=(const SharedGraphicHandle& sharedHandle) noexcept
{
	if (ptr_ && ptr_.use_count() <= 2)
	{
		// 削除すべきだったら削除する
		lpSceneMng.GetResourceMng().Remove(*this);
		if (ptr_.use_count() == 1)
		{
			// 最後なのでDelete呼ぶ
			DeleteGraph(*ptr_);
			ptr_.reset();
		}
	}
	ptr_ = sharedHandle.ptr_;
	return *this;
}

SharedDivGraphicHandle::SharedDivGraphicHandle(const int handle) :
	SharedHandle{ handle }
{
}

SharedDivGraphicHandle::SharedDivGraphicHandle(const SharedDivGraphicHandle& sharedHandle) noexcept :
	SharedHandle{ sharedHandle }
{
}

SharedDivGraphicHandle::~SharedDivGraphicHandle()
{
	if (ptr_ && ptr_.use_count() <= 2)
	{
		lpSceneMng.GetResourceMng().Remove(*this);
	}
}

SharedDivGraphicHandle& SharedDivGraphicHandle::operator=(const SharedDivGraphicHandle& sharedHandle) noexcept
{

	if (ptr_ && ptr_.use_count() <= 2)
	{
		// 削除すべきだったら削除する
		lpSceneMng.GetResourceMng().Remove(*this);
	}
	ptr_ = sharedHandle.ptr_;
	handles_ = sharedHandle.handles_;
	return *this;
}

const int SharedDivGraphicHandle::operator[](size_t idx) const
{
	return handles_[idx];
}

void SharedDivGraphicHandle::Set(const std::span<int> handles)
{
	handles_ = handles;
}

SharedModelHandle::~SharedModelHandle()
{
	MV1DeleteModel(handle_);
	if (ptr_ && ptr_.use_count() <= 2)
	{
		// 大元になるハンドルを削除
		lpSceneMng.GetResourceMng().RemoveModel(*ptr_);
	}
}

SharedModelHandle& SharedModelHandle::operator=(const SharedModelHandle& sharedHandle) noexcept
{
	MV1DeleteModel(handle_);
	if (ptr_ && ptr_.use_count() <= 2)
	{
		// 大元になるハンドルを削除
		lpSceneMng.GetResourceMng().RemoveModel(*ptr_);
	}
	ptr_ = sharedHandle.ptr_;
	CopyParent();
	return *this;
}

void SharedModelHandle::CopyParent(void)
{
	handle_ = MV1DuplicateModel(*ptr_);
}

void SharedModelHandle::SetHandle(const int handle)
{
	handle_ = handle;
}

void SharedModelHandle::SetParent(std::shared_ptr<int>& parent)
{
	ptr_ = parent;
}

const int SharedModelHandle::operator*(void) const noexcept
{
	return handle_;
}


SharedShaderHandle::SharedShaderHandle(const int handle) :
	SharedHandle{handle}
{
}

SharedShaderHandle::SharedShaderHandle(const SharedShaderHandle& sharedHandle) noexcept :
	SharedHandle{sharedHandle}
{
}

SharedShaderHandle::~SharedShaderHandle()
{
	if (ptr_ && ptr_.use_count() <= 2)
	{
		lpSceneMng.GetResourceMng().Remove(*this);
		if (ptr_.use_count() == 1)
		{
			DeleteShader(*ptr_);
			ptr_.reset();
		}
	}
}

SharedShaderHandle& SharedShaderHandle::operator=(const SharedShaderHandle& sharedHandle) noexcept
{
	if (ptr_ && ptr_.use_count() <= 2)
	{
		lpSceneMng.GetResourceMng().Remove(*this);
		if (ptr_.use_count() == 1)
		{
			DeleteShader(*ptr_);
			ptr_.reset();
		}
	}
	ptr_ = sharedHandle.ptr_;
	return *this;
}



SharedSoundHandle::~SharedSoundHandle()
{
	if(ptr_)
	{
		if (ptr_.use_count() <= 2)
		{
			lpSceneMng.GetResourceMng().RemoveSound(*ptr_);
		}
		DeleteSoundMem(handle_);
		ptr_.reset();
	}
}

SharedSoundHandle& SharedSoundHandle::operator=(const SharedSoundHandle& sharedHandle) noexcept
{
	DeleteSoundMem(handle_);
	if (ptr_ && ptr_.use_count() <= 2)
	{
		lpSceneMng.GetResourceMng().RemoveSound(*ptr_);
	}
	ptr_ = sharedHandle.ptr_;
	handle_ = DuplicateSoundMem(*ptr_);
	return *this;
}

void SharedSoundHandle::SetHandle(const int handle)
{
	handle_ = handle;
}


void SharedSoundHandle::CopyParent(void)
{
	handle_ = DuplicateSoundMem(*ptr_);
}


SharedFontHandle::SharedFontHandle(const SharedFontHandle& sharedHandle) noexcept :
	SharedHandle{sharedHandle}
{
}

SharedFontHandle::~SharedFontHandle()
{
	if (ptr_ && ptr_.use_count() <= 1)
	{
		DeleteFontToHandle(*ptr_);
		ptr_.reset();
	}
}

SharedFontHandle& SharedFontHandle::operator=(const SharedFontHandle& sharedHandle) noexcept
{
	if (ptr_ && ptr_.use_count() <= 1)
	{
		DeleteFontToHandle(*ptr_);
	}
	else
	{
		ptr_ = std::make_shared<int>();
	}
	ptr_ = sharedHandle.ptr_;
	return *this;
}

SharedRenderTargetHandle::SharedRenderTargetHandle(const int handle, const ScreenID id) :
	SharedHandle{ handle }, id_{id}
{
}

SharedRenderTargetHandle::SharedRenderTargetHandle(const SharedRenderTargetHandle& sharedHandle) noexcept :
	SharedHandle{ sharedHandle }, id_{sharedHandle.id_}
{
}

SharedRenderTargetHandle::~SharedRenderTargetHandle()
{
	if (ptr_ && ptr_.use_count() <= 2)
	{
		lpSceneMng.GetResourceMng().Remove(id_);
		DeleteGraph(*ptr_);
	}
}

SharedRenderTargetHandle& SharedRenderTargetHandle::operator=(const SharedRenderTargetHandle& sharedHandle) noexcept
{
	if (ptr_ && ptr_.use_count() <= 2)
	{
		lpSceneMng.GetResourceMng().Remove(id_);
		DeleteGraph(*ptr_);
		ptr_.reset();
	}

	ptr_ = sharedHandle.ptr_;
	id_ = sharedHandle.id_;
	return *this;
}

SharedEffectHandle::SharedEffectHandle(const int handle) :
	SharedHandle{handle}
{
}

SharedEffectHandle::SharedEffectHandle(const SharedEffectHandle& sharedHandle) noexcept
{
	if (ptr_ && ptr_.use_count() <= 2)
	{
		// 削除する
		if (ptr_.use_count() == 1)
		{
			// 最後なのでDelete呼ぶ
			DeleteEffekseerEffect(*ptr_);
		}
	}
	ptr_ = sharedHandle.ptr_;
}

SharedEffectHandle::~SharedEffectHandle()
{
	if (ptr_ && ptr_.use_count() <= 2)
	{
		// 削除する
		if (ptr_.use_count() == 1)
		{
			// 最後なのでDelete呼ぶ
			DeleteEffekseerEffect(*ptr_);
			ptr_.reset();
		}
	}
}

SharedEffectHandle& SharedEffectHandle::operator=(const SharedEffectHandle& sharedHandle) noexcept
{
	if (ptr_ && ptr_.use_count() <= 2)
	{
		// 削除する
		if (ptr_.use_count() == 1)
		{
			// 最後なのでDelete呼ぶ
			DeleteEffekseerEffect(*ptr_);
			ptr_.reset();
		}
	}
	ptr_ = sharedHandle.ptr_;
	return *this;
}
