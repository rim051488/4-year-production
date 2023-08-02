#pragma once
#include "../BaseScene.h"

class Loading :
	public BaseScene
{
public:
	Loading(SceneUptr befor, SceneUptr after, float transTime);
	virtual ~Loading();
protected:

	// ロードするシーン
	SceneUptr after_;

	// 遷移元のシーン
	SceneUptr befor_;

	// 遷移時間
	const float transTime_;

	// 経過時間
	float stepTime_;


private:
	const SceneID GetID(void) const final
	{
		return SceneID::Loading;
	}
	
	void LoadingIndicator(float delta);
	void Relese(void) final;
	SceneUptr MakeNextFunc(SceneUptr own);
	

	// ロード中のアイコンなどを表示するか
	bool isIndicator_;

	


};