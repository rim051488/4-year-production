#pragma once
#include <DxLib.h>
#include "Vector3.h"
#include "ScreenID.h"
#include "../SceneManager.h"
#include "Quaternion.h"

class ObjectManager;
class Controller;

// カメラクラス
class Camera
{
public:

	/// <summary>
	/// 描画ようにカメラのセットアップをする
	/// </summary>
	/// <param name=""></param>
	void SetUpScreen(void) const;
	/// <summary>
	/// 描画用にカメラのセットアップ見下ろし型
	/// </summary>
	/// <param name=""></param>
	void SetGameScreen() const;

	/// <summary>
	/// カメラの上方向を取得する
	/// </summary>
	/// <param name=""></param>
	/// <returns> 上方向 </returns>
	const Vector3 GetUp(void)const;

	/// <summary>
	/// カメラの前方向を取得する
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	const Vector3 GetForward(void) const;


	/// <summary>
	/// カメラの座標をセットする
	/// </summary>
	/// <param name="pos"></param>
	void SetPos(const Vector3& pos)
	{
		pos_ = pos;
	}

	/// <summary>
	/// シャドウマップ描画用にカメラをセットする
	/// </summary>
	/// <param name="size"></param>
	/// <param name="n"> near </param>
	/// <param name="f"> fur </param>
	/// <param name="target"> 中視点 </param>
	void SetUpShadow(float size, float n, float f, const Vector3& target) const;

	/// <summary>
	/// 回転をクォータニオンでセットする
	/// </summary>
	/// <param name="q"></param>
	void SetRotation(const Quaternion& q);
private:

	// カメラの座標
	Vector3 pos_;

	// カメラの回転
	Quaternion qRot_;

};

