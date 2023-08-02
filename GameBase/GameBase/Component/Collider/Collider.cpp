#include "Collider.h"
#include "CapsuleCollider.h"
#include "SphereCollider.h"
#include "MeshCollider.h"
#include "CharactorCollider.h"
#include "TerrainCollider.h"
#include "../../Common/Debug.h"
#include "../Behavior/PlayerBehavior.h"

Collider::~Collider()
{
}

void Collider::Begin(ObjectManager& objectManager)
{
	transform_ = objectManager.GetComponent<Transform>(ownerId_);
	isActive_ = true;
}

void Collider::End(ObjectManager& objectManager)
{
	offset_ = zeroVector3<float>;
	hit_ = nullptr;
}

void Collider::Hit(Collider& col, ObjectManager& objMng)
{
	if (hit_)
	{
		// ヒット時に呼び出すべきfunctionがある時呼び出す
		hit_(col, objMng);
	}
}

bool Collider::HitCheck(CapsuleCollider& a, CapsuleCollider& b)
{
	auto aTop{ a.GetTop() };
	auto aBotm{ a.GetBottom() };

	auto bTop{ b.GetTop() };
	auto bBotm{ b.GetBottom() };

	return HitCheck_Capsule_Capsule(
		VGet(bTop.x, bTop.y, bTop.z), VGet(bBotm.x, bBotm.y, bBotm.z), b.GetRadius(),
		VGet(aTop.x, aTop.y, aTop.z), VGet(aBotm.x, aBotm.y, aBotm.z), a.GetRadius()) == TRUE;
}

bool Collider::HitCheck(CapsuleCollider& a, SphereCollider& b)
{
	auto r{ b.GetRadius() };
	auto pos{ b.GetPos() };


	auto aTop{ a.GetTop() };
	auto aBotm{ a.GetBottom() };

	return HitCheck_Sphere_Capsule(
		VGet(pos.x, pos.y, pos.z), r,
		VGet(aTop.x, aTop.y, aTop.z), VGet(aBotm.x, aBotm.y, aBotm.z), b.GetRadius()) == TRUE;
}

bool Collider::HitCheck(CapsuleCollider& a, MeshCollider& b)
{
	auto aTop{ a.GetTop() };
	auto aBotm{ a.GetBottom() };

	auto result = MV1CollCheck_Capsule(b.GetHandle(), -1, VGet(aTop.x, aTop.y, aTop.z), VGet(aBotm.x, aBotm.y, aBotm.z), a.GetRadius());
	auto ret{ result.HitNum > 0 };
	DxLib::MV1CollResultPolyDimTerminate(result);
	return ret;
}

bool Collider::HitCheck(CapsuleCollider& a, CharactorCollider& b)
{
	// カプセルのトップとボトムの座標を取得
	auto aTop{ a.GetTop() };
	auto aBotm{ a.GetBottom() };

	// キャラクターのトップとボトムの座標を取得
	auto bTop{ b.GetTop() };
	auto bBotm{ b.GetBottom() };

	return HitCheck_Capsule_Capsule(
		VGet(bTop.x, bTop.y, bTop.z), VGet(bBotm.x, bBotm.y, bBotm.z), b.GetRadius(),
		VGet(aTop.x, aTop.y, aTop.z), VGet(aBotm.x, aBotm.y, aBotm.z), a.GetRadius()) == TRUE;

}

bool Collider::HitCheck(CapsuleCollider& a, TerrainCollider& b)
{
	auto aTop{ a.GetTop() };
	auto aBotm{ a.GetBottom() };

	auto result = MV1CollCheck_Capsule(b.GetHandle(), -1, VGet(aTop.x, aTop.y, aTop.z), VGet(aBotm.x, aBotm.y, aBotm.z), a.GetRadius());
	auto ret{ result.HitNum > 0 };
	DxLib::MV1CollResultPolyDimTerminate(result);
	return ret;
}

bool Collider::HitCheck(SphereCollider& a, SphereCollider& b)
{
	auto r{ Square(a.GetRadius() + b.GetRadius())};
	auto sqLength{ (a.GetPos() - b.GetPos()).SqMagnitude() };
	return (r >= sqLength);
}

bool Collider::HitCheck(SphereCollider& a, MeshCollider& b)
{
	auto r{ a.GetRadius() };
	auto pos{ a.GetPos() };
	auto result = MV1CollCheck_Sphere(b.GetHandle(), -1, VGet(pos.x, pos.y, pos.z), r);
	auto ret{ result.HitNum > 0 };
	DxLib::MV1CollResultPolyDimTerminate(result);
	return ret;
}

bool Collider::HitCheck(SphereCollider& a, CharactorCollider& b)
{
	// 球体の情報を取得
	auto r{ a.GetRadius() };
	auto pos{ a.GetPos() };

	// カプセルのトップとボトムの座標を取得
	auto bTop{ b.GetTop() };
	auto bBotm{ b.GetBottom() };

	return HitCheck_Sphere_Capsule(
		VGet(pos.x, pos.y, pos.z), r,
		VGet(bTop.x, bTop.y, bTop.z), VGet(bBotm.x, bBotm.y, bBotm.z), b.GetRadius()) == TRUE;
}

bool Collider::HitCheck(SphereCollider& a, TerrainCollider& b, ObjectManager& objectmanager)
{
	// 球体の情報を取得
	auto r{ a.GetRadius() };
	auto pos{ a.GetPos() };

	auto result = MV1CollCheck_Sphere(b.GetHandle(), -1, VGet(pos.x, pos.y, pos.z), r);
	auto ret{ result.HitNum > 0 };

	if (ret && a.IsBlock())
	{
		// 当たっていてかつ押し出し処理をするとき
		auto trans = objectmanager.GetComponent<Transform>(a.GetOwnerID());
		for (int i = 0; i < result.HitNum; i++)
		{
			for (int tcount = 0; tcount < 10; tcount++)
			{
				if (!HitCheck_Sphere_Triangle(VGet(pos.x, pos.y, pos.z), r, result.Dim[i].Position[0], result.Dim[i].Position[1], result.Dim[i].Position[2]))
				{
					break;
				}
				trans->Pos() += Vector3{ result.Dim[i].Normal.x,result.Dim[i].Normal.y, result.Dim[i].Normal.z}.Normalized() * 1.0f;
				pos = a.GetPos();
				r = a.GetRadius();
				
			}
		}
	}

	DxLib::MV1CollResultPolyDimTerminate(result);
	return ret;
}

bool Collider::HitCheck(MeshCollider& a, MeshCollider& b)
{
	// 当たり判定しない
	return false;
}

bool Collider::HitCheck(MeshCollider& a, CharactorCollider& b)
{
	auto bTop{ b.GetTop() };
	auto bBotm{ b.GetBottom() };

	auto result = MV1CollCheck_Capsule(a.GetHandle(), -1, VGet(bTop.x, bTop.y, bTop.z), VGet(bBotm.x, bBotm.y, bBotm.z), b.GetRadius());
	auto ret{ result.HitNum > 0 };
	DxLib::MV1CollResultPolyDimTerminate(result);
	return ret;
}

bool Collider::HitCheck(MeshCollider& a, TerrainCollider& b)
{
	// 当たり判定しない
	return false;
}

bool Collider::HitCheck(CharactorCollider& a, CharactorCollider& b, ObjectManager& objectManager)
{
	// それぞれのトップとボトムの座標を取得
	auto atop{ a.GetTop() };
	auto abotm{ a.GetBottom() };
	auto bBtm{ b.GetBottom() };
	auto bTop{ b.GetTop() };

	// 最短距離を求める
	auto dis = Segment_Segment_MinLength(
		VGet(atop.x, atop.y, atop.z), VGet(abotm.x, abotm.y, abotm.z),
		VGet(bBtm.x, bBtm.y, bBtm.z), VGet(bTop.x, bTop.y, bTop.z));

	if (dis <= a.GetRadius() + b.GetRadius())
	{
		// 当たった時押し出し処理をする
		auto colT = objectManager.GetComponent<Transform>(a.GetOwnerID());
		auto myT = objectManager.GetComponent<Transform>(b.GetOwnerID());

		// 押し出しようベクトルを作る
		auto vec{ colT->GetPos() - myT->GetPos()};

		// 上下には押し出さないので
		vec.y = 0.0f;
		vec.Normalize();

		auto deff = (a.GetRadius() + b.GetRadius() - dis);
		colT->Pos() += vec * (deff * 0.5f);
		myT->Pos() += -vec * (deff * 0.5f);
		return true;
	}
	return false;
}

bool Collider::HitCheck(CharactorCollider& a, TerrainCollider& b, ObjectManager& objectManager)
{
	// カプセルのトップとボトムの座標を取得
	auto aTop{ a.GetTop() };
	auto aBtm{ a.GetBottom() };

	// トランスフォームを取得
	auto trans = objectManager.GetComponent<Transform>(a.GetOwnerID());

	// 接地判定
	// 判定用の終点を求める
	auto aEnd{ aBtm + (downVector3<float> *(a.GetRadius())) };
	auto aStart{ aEnd + (upVector3<float> * a.GetGravityPow()) };

	// 少し長めに判定を取るようにする
	constexpr auto overSize{ 10.0f };
	aEnd += downVector3<float> * overSize;
	aStart += upVector3<float> *overSize;

	// チェック
	auto lineResult = MV1CollCheck_Line(b.GetHandle(), -1, VGet(aStart.x, aStart.y, aStart.z), VGet(aEnd.x, aEnd.y, aEnd.z));
	if (lineResult.HitFlag > 0)
	{
		// 当たった座標
		auto hitPos = Vector3{ lineResult.HitPosition.x, lineResult.HitPosition.y , lineResult.HitPosition.z };

		// 当たったポリゴンの法制
		auto hitNormal = Vector3{ lineResult.Normal.x , lineResult.Normal.y, lineResult.Normal.z };

		// 重力
		auto g = downVector3<float> *(a.GetGravityPow() > 0 ? a.GetGravityPow() : 4.9f);

		// 正規化
		hitNormal.Normalize();

		// 当たったポリゴンの法線をもとに傾斜を考慮したベクトルを生成(v = g - n(n * g))
		auto v = g - (hitNormal * Dot(hitNormal, g));

		trans->Pos() += v ;
		a.SetGroundFlag(true);
		a.SetGravityPow(0.0f);

		// 補正を掛けたので座標を再取得
		aTop = a.GetTop();
		aBtm = a.GetBottom();
	}
	else
	{
		a.SetGroundFlag(false);
	}
	

	


	// ダッシュを考慮した判定を取る
	auto topDashStart = aTop + (-a.GetMoveDir() * a.GetSpeed());
	auto btmDashStart = aBtm + (-a.GetMoveDir() * a.GetSpeed());
	
	// 上下二点から当たり判定を取る
	auto topResult = MV1CollCheck_Line(b.GetHandle(), -1, VGet(topDashStart.x, topDashStart.y, topDashStart.z),VGet(aTop.x, aTop.y, aTop.z));
	auto btmResult = MV1CollCheck_Line(b.GetHandle(), -1, VGet(btmDashStart.x, btmDashStart.y, btmDashStart.z), VGet(aBtm.x, aBtm.y, aBtm.z));
	bool ret = false;
	if (topResult.HitFlag > 0 || btmResult.HitFlag > 0)
	{
		ret = true;
		if (topResult.HitFlag > 0 && btmResult.HitFlag > 0)
		{
			// 上下両方当たってる時
			auto topHitPos{ Vector3{topResult.HitPosition.x,topResult.HitPosition.y, topResult.HitPosition.z} };
			auto btmHitPos{ Vector3{btmResult.HitPosition.x,btmResult.HitPosition.y, btmResult.HitPosition.z} };
			DebugDrawSphere(topHitPos, 50.0f, 0xff0000);
			DebugDrawSphere(btmHitPos, 50.0f, 0xff0000);
			if ((topHitPos - topDashStart).SqMagnitude() < (btmHitPos - btmDashStart).SqMagnitude())
			{
				// 上の方が元の位置からの交点が近いとき
				trans->Pos() += -a.GetMoveDir() * ((a.GetSpeed() - (topHitPos - topDashStart).Magnitude()) + a.GetRadius());
			}
			else
			{
				// 下の方が元の位置からの交点が近いとき
				trans->Pos() += -a.GetMoveDir() * ((a.GetSpeed() - (btmHitPos - btmDashStart).Magnitude()) + a.GetRadius());
			}
		}
		else
		{
			if (topResult.HitFlag > 0)
			{
				// 上の方が元の位置からの交点が近いとき
				auto topHitPos{ Vector3{topResult.HitPosition.x,topResult.HitPosition.y, topResult.HitPosition.z} };
				trans->Pos() += -a.GetMoveDir() * ((a.GetSpeed() - (topHitPos - topDashStart).Magnitude()) + a.GetRadius());
			}
			else
			{
				// 下の方が元の位置からの交点が近いとき
				auto btmHitPos{ Vector3{btmResult.HitPosition.x,btmResult.HitPosition.y, btmResult.HitPosition.z} };
				trans->Pos() += -a.GetMoveDir() * ((a.GetSpeed() - (btmHitPos - btmDashStart).Magnitude()) + a.GetRadius());
			}
		}

		// 座標に補正を掛けたので再取得
		aTop = a.GetTop();
		aBtm = a.GetBottom();
	}
	

	// 地形との判定
	auto result{ MV1CollCheck_Capsule(b.GetHandle(), -1 , VGet(aTop.x, aTop.y, aTop.z), VGet(aBtm.x, aBtm.y, aBtm.z), a.GetRadius()) };
	if (result.HitNum > 0)
	{
		ret = true;
		for (int i = 0; i < result.HitNum; i++)
		{
			// 押し出しする
			for (int tcount = 0; tcount < 50; tcount++)
			{
				if (!HitCheck_Capsule_Triangle(
					VGet(aTop.x, aTop.y, aTop.z), VGet(aBtm.x, aBtm.y, aBtm.z), a.GetRadius(),
					result.Dim[i].Position[0], result.Dim[i].Position[1], result.Dim[i].Position[2]
				))
				{
					// ポリゴンと当たってないときのループを抜ける
					break;
				}

				// ポリゴンの法線方向に押し出す
				trans->Pos() += Vector3{ result.Dim[i].Normal.x, result.Dim[i].Normal.y, result.Dim[i].Normal.z }.Normalized() * 1.0f;
				aTop = a.GetTop();
				aBtm = a.GetBottom();
			}
		}
	}

	// 当たり判定情報を破棄する
	MV1CollResultPolyDimTerminate(result);

	return ret;
}

bool Collider::HitCheck(TerrainCollider& a, TerrainCollider& b)
{
	// 当たり判定を取らない
	return false;
}
