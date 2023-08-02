#pragma once
#include "UiBase.h"
#include <vector>
#include <unordered_map>

enum class TutorialType;

class TutorialMessage :
    public UiBase
{
public:
	TutorialMessage();
private:

	enum class DrawType
	{
		Mess,
		Controller
	};

	void Update(float delta, BaseScene& scene, ObjectManager& objMng, Controller& controller) final;
	void Draw(int mainScr) final;
	UiID GetUIID(void) final
	{
		return UiID::TutorialMess;
	}

	/// <summary>
	/// チュートリアルメッセージを描画
	/// </summary>
	/// <param name=""></param>
	void DrawMess(void);

	/// <summary>
	/// チュートリアルの操作メッセージを描画
	/// </summary>
	/// <param name=""></param>
	void DrawControllerMess(void);

	// 現在のチュートリアルの種類
	TutorialType nowType_;

	// メッセージの画像
	std::vector<SharedGraphicHandle> messages_;

	// キーとかパッドの画像
	SharedDivGraphicHandle controllerImg_;

	// 描画タイプテーブル
	std::unordered_map<TutorialType, DrawType> drawTypeTbl_;

	// 描画関数
	std::vector<void(TutorialMessage::*)(void)> drawFunc_;

	std::unordered_map<int, int>* codeTbl_;
};

