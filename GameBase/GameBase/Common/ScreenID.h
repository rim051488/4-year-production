#pragma once

// スクリーンのID
enum class ScreenID
{
	Title,			// タイトルシーン
	Select,			// 選択画面
	SelectUi,
	Game,			// ゲームシーンの
	Result,			// リザルトシーン
	Transition,		// トランジションシーン
	Loading,		// ロード中
	Pause,			// ポーズ画
	Option,			// 設定画面
	OptionUi,
	InputSet,		// キー用設定シーン
	PauseUi,		// ポーズ画面のUiを開くときよう
	Dialog,			// ダイアログシーン
	DialogUi,		// ダイアログのUiを開くよう
	ResultCapture,	// リザルトシーンで使うゲーム終了時のキャプチャ画面
};

