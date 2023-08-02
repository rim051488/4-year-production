#pragma once
#include "Render.h"
#include "../../Common/SharedHandle.h"



// 3Dmodelを描画するクラス
class ModelRender :
	public Render
{
public:
	ModelRender();
	virtual ~ModelRender();
	static constexpr ComponentID id_{ ComponentID::ModelRender };
	ComponentID GetID(void) const override
	{
		return id_;
	}
	void Draw(int shadowMap = -1, int buff = -1, int cubeTex = -1) override;
	void SetUpDepthTex(int ps = -1, int buff = -1) override;

	static std::string Load(std::ifstream& file);

	SharedModelHandle& GetHandle(void)&
	{
		return handle_;
	}

	/// <summary>
	/// 初期状態の角度をセット
	/// </summary>
	/// <param name="rot"></param>
	void SetDefaultRot(const Vector3& rot)
	{
		defaultRot_ = rot;
	}

	/// <summary>
	/// カメラとの当たり判定用サイズ
	/// </summary>
	/// <param name="rt"></param>
	/// <param name="rb"></param>
	void SetBoundingSize(const Vector3& lt, const Vector3& rb)
	{
		bb_.isCheck_ = true;
		bb_.ltSize_ = lt;
		bb_.rbSize_ = rb;
	}

	/// <summary>
	/// シェーダを使用しない描画処理をする
	/// </summary>
	/// <param name=""></param>
	void SetNonShader(void);

protected:
	// モデルのハンドル
	SharedModelHandle handle_;
	// 使用するシェーダ
	SharedShaderHandle ps_;
	SharedShaderHandle vs_;
	// シャドウマップ用シェーダ
	SharedShaderHandle shadowPs_;
	SharedShaderHandle shadowVs_;

private:

	// カメラとの判定用
	struct CameraBB
	{
		CameraBB();
		bool IsHit(const Vector3& pos, const Quaternion& rot) const&;
		Vector3 ltSize_;
		Vector3 rbSize_;
		bool isCheck_;

	};

	void Load(const std::filesystem::path& path) final;
	void Update(BaseScene& scene, ObjectManager& objectManager, float delta, Controller& controller) final;

	void Begin(ObjectManager& objectManager) final;
	void End(ObjectManager& objectManager) final;

	/// <summary>
	/// カメラから見えるか?
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	bool IsCameraView(void);

	/// <summary>
	/// シェーダ使用して描画する
	/// </summary>
	/// <param name="shadowMap"></param>
	/// <param name="buff"></param>
	void DrawShader(int shadowMap = -1, int buff = -1, int cubeTex = -1);

	/// <summary>
	/// シェーダなしで描画する
	/// </summary>
	/// <param name="shadowMap"></param>
	/// <param name="buff"></param>
	void DrawNonShader(int shadowMap = -1, int buff = -1, int cubeTex = -1);

	void DrawDepthShader(int ps = -1, int buff = -1);

	void DrawDepthNonShader(int ps = -1, int buff = -1);

	int normMap;

	void (ModelRender::* draw_)(int, int,int);
	void (ModelRender::* drawDepth_)(int, int);

	Vector3 defaultRot_;

	// カメラの判定
	CameraBB bb_;

};

