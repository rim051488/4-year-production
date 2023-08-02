#pragma once

enum class SceneID
{
	Title,		// タイトル
	Select,		// 選択
	Game,		// ゲーム中
	Result,		// 結果
	Pause,		// ポーズ
	Option,		// オプション
	Transition,	// 遷移
	Loading,	// ロード中
	InputSet,	// キーやパッドの設定中のシーン
	Dialog,		// Ok&Cancel用ダイアログクラス
};

enum class ResultAttribute
{
	Clear,
	GameOver,
	Max,
};
