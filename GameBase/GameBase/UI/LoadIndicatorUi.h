#pragma once
#include "UiBase.h"
class LoadIndicatorUi :
    public UiBase
{
public:
    LoadIndicatorUi(const std::filesystem::path& indPath, const Vector2& indPos, const std::filesystem::path& fontPath);
private:
    static constexpr int Interval = 3;

    void Update(float delta, BaseScene& scene, ObjectManager& objMng, Controller& controller) override;
    void Draw(int mainScr) override;
    UiID GetUIID(void) override { return UiID::Indicator; }

    // •`‰æŠp“x
    double angle_;

    SharedGraphicHandle handle_;
    SharedGraphicHandle loadHandle_;

    int cnt_;
};

