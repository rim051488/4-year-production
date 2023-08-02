#include <DxLib.h>
#include "Cursor.h"
#include "../Common/Input/Controller.h"
#include "../Common/ResourceMng.h"
#include "Button.h"
#include "../Common/SoundPross.h"

#include "../Common/Debug.h"

constexpr float cursorSize{ 20.0f };
constexpr float blinkStartTimeMax{ 5.0f };

Cursor::Cursor() :
	UiBase{zeroVector2<float>}
{
	blinkTime_ = -0.5f;
	lpSceneMng.GetResourceMng().LoadTexture(handle_, "Resource/resource/Ui/cursor.png");
}

void Cursor::Update(float delta, BaseScene& scene, ObjectManager& objMng, Controller& controller)
{
	auto now = controller.GetCursorPos();

	blinkTime_ += delta;
	pos_ = now;
	
}

void Cursor::Draw(int mainScr)
{
	if (!isDraw_)
	{
		return;
	}
	DrawRotaGraphF(pos_.x, pos_.y, 0.9f + std::abs(0.3f * std::sin(blinkTime_ * 5.0f)), blinkTime_, *handle_, true);
}

void Cursor::Check(std::list<std::unique_ptr<UiBase>>& uiList, BaseScene& scene,Controller& controller)
{
	for (auto& ui : uiList)
	{
		ButtonBase* button;
		if (!ui->IsButton())
		{
			// ボタンではないとき関係ないのでスキップ
			continue;
		}

		button = static_cast<ButtonBase*>(ui.get());
		if (ui->GetPos().x <= pos_.x && (ui->GetPos().x + ui->GetSize().x) >= pos_.x&&
			ui->GetPos().y <= pos_.y && (ui->GetPos().y + ui->GetSize().y) >= pos_.y
			)
		{
			if (!button->IsHitCursor())
			{
				// カーソルが乗ったことを伝える
				lpSooundPross.PlayBackSound(SOUNDNAME_SE::cursorMove,false);
				button->HitCursor();
			}

			if (button->IsDecision(controller))
			{
				// クリックしたことを伝える
				lpSooundPross.PlayBackSound(SOUNDNAME_SE::click,false);
				button->Push(scene, controller);
			}
		
		}
		else
		{
			if (button->IsHitCursor())
			{
				// カーソルから離れたことを伝える
				button->NotHitCursor();
			}
		}
	}
}
