#pragma once
#include "UiBase.h"

enum class ViewType
{
	Non,
	Blink
};

class Image :
    public UiBase
{
public:
	Image(const std::filesystem::path& path, const Vector2 pos, ViewType type = ViewType::Non);
	Image(const std::filesystem::path& path, const Vector2I& div, const Vector2I& size, const Vector2& pos, const Vector2& interval);
private:
	void Update(float delta, BaseScene& scene, ObjectManager& objMng, Controller& controller) final;
	void Draw(int mainScr);
	UiID GetUIID(void)
	{
		return UiID::Frame;
	}
private:
	void DrawNon(void);
	void DrawBlink(void);
	Vector2 interval_;
	SharedGraphicHandle handle_;
	SharedDivGraphicHandle divHandle_;
	float time_;
	void(Image::* draw_)(void);

	// âÊëúï™äÑêî
	Vector2I div_;
};

