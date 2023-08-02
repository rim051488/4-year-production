#include "TutorialMessage.h"
#include "../SceneManager.h"
#include "../Component/Behavior/TutorialBehavior.h"
#include "../Object/ObjectManager.h"
#include "../Scene/BaseScene.h"
#include "../Common/ResourceMng.h"
#include "../Common/Input/InputType/Pad.h"
#include "../Common/Input/InputConfig/InputConfig.h"

// チュートリアルで使うInputIDのテーブル
std::unordered_map<TutorialType, InputID> tTypeInputIDTbl
{
	{TutorialType::Attack, InputID::Attack},
	{TutorialType::Dash, InputID::Dash},
	{TutorialType::Skill, InputID::Skil},
	{TutorialType::Jump, InputID::Jump}
};

// キーの画像テーブル
std::unordered_map<int, int> keyCodeTbl
{
	{KEY_INPUT_0, 0},
	{KEY_INPUT_1, 1},
	{KEY_INPUT_2, 2},
	{KEY_INPUT_3, 3},
	{KEY_INPUT_4, 4},
	{KEY_INPUT_5, 5},
	{KEY_INPUT_6, 6},
	{KEY_INPUT_7, 7},
	{KEY_INPUT_8, 8},
	{KEY_INPUT_9, 9},
	{KEY_INPUT_B, 10},
	{KEY_INPUT_C, 11},
	{KEY_INPUT_E, 12},
	{KEY_INPUT_F, 13},
	{KEY_INPUT_G, 14},
	{KEY_INPUT_H, 15},
	{KEY_INPUT_I, 16},
	{KEY_INPUT_J, 17},
	{KEY_INPUT_K, 18},
	{KEY_INPUT_L, 19},
	{KEY_INPUT_M, 20},
	{KEY_INPUT_N, 21},
	{KEY_INPUT_O, 22},
	{KEY_INPUT_P, 23},
	{KEY_INPUT_Q, 24},
	{KEY_INPUT_R, 25},
	{KEY_INPUT_U, 26},
	{KEY_INPUT_T, 27},
	{KEY_INPUT_V, 28},
	{KEY_INPUT_X, 29},
	{KEY_INPUT_Y, 30},
	{KEY_INPUT_Z, 31},
	{KEY_INPUT_CAPSLOCK, 32},
	{KEY_INPUT_LALT, 33},
	{KEY_INPUT_LCONTROL, 34},
	{KEY_INPUT_LSHIFT, 35},
	{KEY_INPUT_RALT, 36},
	{KEY_INPUT_RCONTROL, 37},
	{KEY_INPUT_RSHIFT, 38},
	{KEY_INPUT_SPACE, 39},
	{KEY_INPUT_TAB, 40},
	{0, 41},
	{KEY_INPUT_UP, 42},
	{KEY_INPUT_DOWN, 43},
	{KEY_INPUT_LEFT, 44},
	{KEY_INPUT_RIGHT, 45},
	{-MOUSE_INPUT_LEFT, 46},
	{-MOUSE_INPUT_RIGHT, 47}
};

std::unordered_map<int, int> xboxPadCodeTbl
{
	{0,6},
	{1,7},
	{2,8},
	{3,9},
	{4,4},
	{5,5}
};

TutorialMessage::TutorialMessage() :
	UiBase{SceneManager::screenSize_<float>/2.0f}
{
	nowType_ = TutorialType::Move;
	messages_.resize(static_cast<size_t>(TutorialType::Max));
	drawTypeTbl_.reserve(static_cast<size_t>(TutorialType::Max));
	drawTypeTbl_[TutorialType::Move] = DrawType::Mess;
	drawTypeTbl_[TutorialType::View] = DrawType::Mess;
	drawTypeTbl_[TutorialType::Jump] = DrawType::Controller;
	drawTypeTbl_[TutorialType::Dash] = DrawType::Controller;
	drawTypeTbl_[TutorialType::Attack] = DrawType::Controller;
	drawTypeTbl_[TutorialType::Enemy] = DrawType::Mess;
	drawTypeTbl_[TutorialType::SkillMess] = DrawType::Mess;
	drawTypeTbl_[TutorialType::SkillChargeMess] = DrawType::Mess;
	drawTypeTbl_[TutorialType::SkillCharge] = DrawType::Mess;
	drawTypeTbl_[TutorialType::Skill] = DrawType::Controller;
	drawTypeTbl_[TutorialType::End] = DrawType::Mess;
	drawFunc_.resize(2);
	drawFunc_[0] = &TutorialMessage::DrawMess;
	drawFunc_[1] = &TutorialMessage::DrawControllerMess;
	
	if (lpSceneMng.GetController().GetCntType() == CntType::Key)
	{
		lpSceneMng.GetResourceMng().LoadTexture(messages_[static_cast<size_t>(TutorialType::Move)], "Resource/resource/TutorialMoveKeyMess.png");
		lpSceneMng.GetResourceMng().LoadTexture(messages_[static_cast<size_t>(TutorialType::View)], "Resource/resource/TutorialViewKeyMess.png");
		codeTbl_ = &keyCodeTbl;
		lpSceneMng.GetResourceMng().LoadDivTexture(controllerImg_, "Resource/resource/TutorialKeyMess.png", { 6,8 }, { 460,70 });
	}
	else
	{
		lpSceneMng.GetResourceMng().LoadTexture(messages_[static_cast<size_t>(TutorialType::Move)], "Resource/resource/TutorialMovePadMess.png");
		lpSceneMng.GetResourceMng().LoadTexture(messages_[static_cast<size_t>(TutorialType::View)], "Resource/resource/TutorialViewPadMess.png");
		auto& ctrl = static_cast<Pad&>(lpSceneMng.GetController());
		lpSceneMng.GetResourceMng().LoadDivTexture(controllerImg_, "Resource/resource/TutorialPadMess.png", { 2,5 }, { 385,70 });
		codeTbl_ = &xboxPadCodeTbl;
	}
	lpSceneMng.GetResourceMng().LoadTexture(messages_[static_cast<size_t>(TutorialType::Jump)], "Resource/resource/TutorialJump.png");
	lpSceneMng.GetResourceMng().LoadTexture(messages_[static_cast<size_t>(TutorialType::Dash)], "Resource/resource/TutorialDash.png");
	lpSceneMng.GetResourceMng().LoadTexture(messages_[static_cast<size_t>(TutorialType::Attack)], "Resource/resource/TutorialAttack.png");
	lpSceneMng.GetResourceMng().LoadTexture(messages_[static_cast<size_t>(TutorialType::Skill)], "Resource/resource/TutorialSkill.png");
	lpSceneMng.GetResourceMng().LoadTexture(messages_[static_cast<size_t>(TutorialType::End)], "Resource/resource/TutorialEndMess.png");
	lpSceneMng.GetResourceMng().LoadTexture(messages_[static_cast<size_t>(TutorialType::Enemy)], "Resource/resource/TutorialEnemyMess.png");
	lpSceneMng.GetResourceMng().LoadTexture(messages_[static_cast<size_t>(TutorialType::SkillMess)], "Resource/resource/TutorialSkillMess.png");
	lpSceneMng.GetResourceMng().LoadTexture(messages_[static_cast<size_t>(TutorialType::SkillChargeMess)], "Resource/resource/TutorialGageMess.png");
}

void TutorialMessage::Update(float delta, BaseScene& scene, ObjectManager& objMng, Controller& controller)
{
	auto [result, id] = objMng.Find(ObjectAttribute::Stage);
	if (result)
	{
		auto behavior = objMng.GetComponent<TutorialBehavior>(id);
		nowType_ = behavior->GetNowType();
	}
}

void TutorialMessage::Draw(int mainScr)
{
	DrawType type = DrawType::Mess;
	if (drawTypeTbl_.contains(nowType_))
	{
		type = drawTypeTbl_[nowType_];
	}
	(this->*drawFunc_[static_cast<size_t>(type)])();
}

void TutorialMessage::DrawMess(void)
{
	int x, y;
	GetGraphSize(*messages_[static_cast<size_t>(nowType_)], &x, &y);
	DrawGraph(SceneManager::screenSize_<int>.x - x, 0, *messages_[static_cast<size_t>(nowType_)], true);
}

void TutorialMessage::DrawControllerMess(void)
{
	int x, y;
	GetGraphSize(*messages_[static_cast<size_t>(nowType_)], &x, &y);
	DrawGraph(SceneManager::screenSize_<int>.x - x, 0, *messages_[static_cast<size_t>(nowType_)], true);

	int ctrX, ctrY;
	GetGraphSize(controllerImg_[0], &ctrX, &ctrY);

	InputID inputID = InputID::Max;
	if (tTypeInputIDTbl.contains(nowType_))
	{
		inputID = tTypeInputIDTbl.at(nowType_);
	}

	int tmpCode = lpConfigMng.GetInputCode().at(inputID);
	if (!codeTbl_->contains(tmpCode))
	{
		tmpCode = 0;
	}
	DrawGraph((SceneManager::screenSize_<int>.x - x) - ctrX, 0, controllerImg_[codeTbl_->at(tmpCode)],true);
}

