#pragma once

// データの種類
enum class DataType
{
	Transform,			// 座標回転拡縮データ
	Collider,			// 当たり判定データ
	Model,				// モデルデータのパス
	Spawn,				// スポーンさせる座標とデータパス
	Spawner,			// スポナーの位置
	Other				// そのほかのパラメータ用
};

// データファイルのヘッダー
struct ObjectDataHeader
{
	char sig[4];			// シグネチャ
	unsigned int check;		// チェック用
	unsigned int version;	// バージョン情報
};

// データ部のヘッダー
struct DataHeader
{
	DataType type;			// タイプ
	std::uint32_t size;		// データサイズ
};

// 当たり判定データの種類
enum class ColDataType
{
	Box,
	Capsule,
	Sphere,
	Mesh,
};
