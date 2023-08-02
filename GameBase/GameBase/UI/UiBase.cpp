#include "UiBase.h"
#include <Dxlib.h>
#include "../SceneManager.h"
#include "../Common/ResourceMng.h"
#include "../Common/Debug.h"


UiBase::UiBase(const Vector2& pos) :
	pos_{pos}, isDraw_{true}
{
}

void UiBase::Loaded(BaseScene& scene)
{
}
