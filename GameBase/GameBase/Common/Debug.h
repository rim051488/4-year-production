#pragma once
#ifdef _DEBUG
#include <Windows.h>	// OutPutDebugStringnがある所
#include <iostream>		// コンソールに標準出力
#include <sstream>		// stringの上位版文字列ストリーム
#include <array>
#include <deque>
#include <functional>
#include <memory>		// ユニークポインタとかの
#include <thread>
#include <cassert>
#include <iomanip>
#include <mutex>
#include "Vector3.h"

// クリエイト関数を呼ぶマクロ関数
#define DebugStart(timeClass) (DebugClass::Create(timeClass))

// デバッグのセットアップをする
#define DebugSetUp() (DebugClass::GetInstance().SetUp())

// デバッグコンソールに出力する
#define DebugLog(...) (DebugClass::GetInstance().Log(true,"",__VA_ARGS__))

// visualstudioの出力の欄に出力する
#define DebugOutput(...) (DebugClass::GetInstance().Out(__VA_ARGS__))

// エラーとしてコンソールに出力する
#define DebugErrorLog(...)(DebugClass::GetInstance().ErrorLog(true,"",__VA_ARGS__))

// 画面に文字として描画する
#define DebugDrawString(...)(DebugClass::GetInstance().DrawStr(__VA_ARGS__));

// デバッグ情報の描画
#define DebugInfoDraw() (DebugClass::GetInstance().DebugDraw())

// 現在のスクリーンにデバッグ用3Dのものを描画する
#define DebugDraw3DScreen() (DebugClass::GetInstance().DebugDraw3D())

// デバッグようにカプセルを描画する
#define DebugDrawCapsule(top,btm,r,color) (DebugClass::GetInstance().DebugDraw3D(top,btm,r,color))

// デバッグように球体を描画する
#define DebugDrawSphere(pos, r, color) (DebugClass::GetInstance().DebugDraw3D(pos,r,color))

// デバッグように線を描画する
#define DebugDrawLine(start, end, color) (DebugClass::GetInstance().DebugDraw3D(start,end,color))

#define DebugDrawCube(pos1, pos2, color) (DebugClass::GetInstance().DebugDraw3DCube(pos1, pos2, color));

// 自作のassertみたいなの
#define Assert(val,mess) ( (val) || (_wassert(_CRT_WIDE(mess), _CRT_WIDE(__FILE__), (unsigned)(__LINE__)),0))


class TimeClass;

class DebugClass
{
public:

	// クリエイト関数
	static void Create(TimeClass& timeClass);

	// 実体を取得する
	static DebugClass& GetInstance(void);

	void SetUp(const int& targetSc = 0xfffffffe);

	

	// 複数版
	template<class T, class... A>
	void Log(bool flag,const T& t,const A&... a);

	
	// エラー用のログ
	template<class T, class... A>
	void ErrorLog(bool flag,const T& t, const A&... a);

	// VisualStudioの出力の欄に出力する
	template<class T>
	void Out(const T& t);

	template<class T, class... A>
	void Out(const T& t, const A&... a);
	

	// デバッグ用のスクリーン
	void DebugDraw(void);

	// 3D系のデバッグ用の描画を実行する
	void DebugDraw3D(void);

	// 球体のデバッグ用描画
	void DebugDraw3D(const Vector3& pos, float r, int color);

	// カプセルのデバッグ用描画
	void DebugDraw3D(const Vector3& top, const Vector3& btm, float r, int color);

	// 線のデバッグ用描画
	void DebugDraw3D(const Vector3& start, const Vector3& end, int color);

	// boxのデバッグ描画
	void DebugDraw3DCube(const Vector3& pos1, const Vector3& pos2, int color);

	// 画面に文字列を表示する
	template<class T>
	void DrawStr(const T& t)
	{
		ss_ << t << std::endl;
	}

	template<class T, class... A>
	void DrawStr(const T& t, const A&... a)
	{
		ss_ << t;
		DrawStr(a...);
	}

private:

	// ログのタイプ
	enum class LogType
	{
		ALL,
		Log,
		Error
	};

	// コンストラクタ
	DebugClass(TimeClass& timeClass);

	// デストラクタ
	~DebugClass();

	// ログの情報を出力する
	void PrintLogInfo(void);

	// エラーログの情報を出力する
	void PrintErrorLogInfor(void);

	// コンソールに出力する
	template<class T>
	void Log(bool flag, const T& t);

	// エラー用
	template<class T>
	void ErrorLog(bool flag,const T& t);

	// デリーター
	struct Deleter
	{
		void operator()(DebugClass* p)
		{
			delete p;
		}
	};

	

	// 標準出力するための関数オブジェクト
	struct LogPrinter
	{
		template<class T>
		void operator()(const T& t)
		{
			std::cout << t << std::endl;
		}

		template<class T, class... A>
		void operator()(const T& t, const A&... a)
		{
			std::count << t;
			LogPrinter()(a...);
		}

	};

	// 実体をユニークポインタで持つ
	static std::unique_ptr<DebugClass, Deleter> instance_;


	// ファイルポインタ
	FILE* conFp_;

	// おおもとのスクリーンを指定できるように
	int targetSc_;

	// デバッグ用のスクリーンのハンドル
	int screen_;

	// スクリーンの高さと幅
	int scW, scH;

	// デバッグ情報の表示フラグ
	bool infoDrawFlag_;

	// 停止フラグ
	bool stopFlag_;

	// デバッグ用描画の表示フラグ
	bool debugDrawFlag_;

	std::array<char, 256> key_;
	std::array<char, 256> keyOld_;

	// ログ出力のタイプ
	LogType logType_;

	TimeClass& time_;

	std::chrono::system_clock::time_point start_;

	// ミューテックス
	std::mutex mutex_;

	// 3D系デバッグを実行するためのキュー
	std::deque<std::function<void(void)>> draw3DQueue_;

	// 現在の3D系デバッグ描画の数
	size_t nowDraw3D_;

	// 文字列描画用のキュー
	std::deque<std::function<void(std::stringstream&)>> drawStringQueue_;

#ifdef UNICODE
	std::wstringstream ss_;
#else
	std::stringstream ss_;
#endif

};

template<class T>
inline void DebugClass::Log(bool flag, const T& t)
{
	std::cout << (t) << std::endl;
}

template<class T, class ...A>
inline void DebugClass::Log(bool flag, const T& t, const A& ...a)
{
	// デバッグモードの時だけ
	if (logType_ == LogType::Error)
	{
		return;
	}
	if (flag)
	{
		// trueの時(最初に呼ばれた時)ロックして再帰的にLogを呼ぶ
		mutex_.lock();
		PrintLogInfo();
		std::cout << t;
		Log(false, a...);

		// 再帰呼び出しから復帰したのでロック解除
		mutex_.unlock();
	}
	else
	{
		// falseの時(2回目移行)出力してLogへ
		std::cout << t;
		Log(false, a...);
	}
}

template<class T>
inline void DebugClass::ErrorLog(bool flag,const T& t)
{
	std::cout << (t) << std::endl;
}

template<class T, class ...A>
inline void DebugClass::ErrorLog(bool flag,const T& t, const A & ...a)
{
	if (logType_ == LogType::Log)
	{
		return;
	}
	if (flag)
	{
		mutex_.lock();
		PrintErrorLogInfor();
		std::cout << t;
		ErrorLog(false, a...);
		mutex_.unlock();
	}
	else
	{
		std::cout << t;
		ErrorLog(false, a...);
	}
}

template<class T>
inline void DebugClass::Out(const T& t)
{
	// デバッグモードの時だけ
#ifdef UNICODE
	// ユニコードの時こっち
	std::wstringstream ss;
	ss << t << std::endl;
	OutputDebugString(ss.str().c_str());
#else
	std::stringstream ss(t);
	OutputDebugString(ss.str().c_str());
#endif
}

template<class T, class ...A>
inline void DebugClass::Out(const T& t, const A & ...a)
{
#ifdef UNICODE
	// ユニコードの時こっち
	std::wstringstream ss;
	ss << t;
	OutputDebugString(ss.str().c_str());
	Out(a...);
#else
	std::stringstream ss(t);
	OutputDebugString(ss.str().c_str());
	Out(a...);
#endif
}





#else

#define DebugStart(timeClass) 

#define DebugSetUp() 

#define DebugLog(...)

#define DebugErrorLog(...)

#define DebugOutput(...) 

#define DebugInfoDraw() 

// デバッグようにカプセルを描画する
#define DebugDrawCapsule(top,btm,r,color)

// デバッグように球体を描画する
#define DebugDrawSphere(pos, r, color)

// デバッグように線を描画する
#define DebugDrawLine(start, end, color)

#define DebugDrawCube(pos1, pos2, color)

#define DebugDrawString(...)

#define DebugDraw3DScreen()

#define Assert(val,mess) (val)

#endif







