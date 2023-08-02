#pragma once
#include "UiBase.h"
class ComboUi :
    public UiBase
{
public:
    ComboUi(const std::filesystem::path& numPath, const Vector2I& div, const Vector2I& size, const std::filesystem::path& fontPath, const Vector2& pos, const Vector2& interval);
    ~ComboUi();
private:
    void Update(float delta, BaseScene& scene, ObjectManager& objMng, Controller& controller) override;
    void Draw(int mainScr) override;
    UiID GetUIID(void) override { return UiID::Combo; }

    int combo_;
    Vector2 interval_;
    SharedGraphicHandle handle_;
    SharedDivGraphicHandle divHandle_;
};

