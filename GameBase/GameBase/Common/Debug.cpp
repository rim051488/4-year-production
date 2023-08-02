#ifdef _DEBUG
#include <DxLib.h>
#include <cassert>
#include "Time.h"
#include "Debug.h"

std::unique_ptr<DebugClass, DebugClass::Deleter> DebugClass::instance_ = nullptr;

void DebugClass::Create(TimeClass& timeClass)
{
	if (instance_ == nullptr)
	{
		// ヌルポインターの時実体を作る
		instance_.reset(new DebugClass(timeClass));
	}
}

DebugClass& DebugClass::GetInstance(void)
{
	return (*instance_);
}

void DebugClass::SetUp(const int& targetSc)
{
	targetSc_ = targetSc;
	// スクリーンの幅と高さを取得
	SetDrawScreen(DX_SCREEN_BACK);
	GetDrawScreenSize(&scW, &scH);
	screen_ = MakeScreen(scW, scH,true);
}


void DebugClass::DebugDraw(void)
{
	keyOld_ = key_;
	GetHitKeyStateAll(key_.data());
	if (key_[KEY_INPUT_F1] && !keyOld_[KEY_INPUT_F1])
	{
		infoDrawFlag_ = !infoDrawFlag_;
	}
	
	if (key_[KEY_INPUT_F2] && !keyOld_[KEY_INPUT_F2])
	{
		stopFlag_ = true;
	}

	if (key_[KEY_INPUT_F3] && !keyOld_[KEY_INPUT_F3])
	{
		logType_ = static_cast<LogType>((static_cast<int>(logType_) + 1) % (static_cast<int>(LogType::Error) + 1));
		
	}
	
	if (key_[KEY_INPUT_F4] && !keyOld_[KEY_INPUT_F4])
	{
		debugDrawFlag_ = !debugDrawFlag_;
	}

	SetDrawScreen(screen_);
	ClsDrawScreen();
#ifdef UNICODE
	std::wstringstream tmpss;
#else
	std::stringstream tmpss;
#endif
	if (infoDrawFlag_)
	{
		// 情報を表示するとき
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
		DrawBox(0, 0, scW, scH, 0x000000, true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		tmpss << TEXT("DeltaTime-") << std::setprecision(4) <<  time_.GetDeltaTime<float>() << TEXT("-") << std::endl;
		tmpss << TEXT("GameTime-") << std::setprecision(6) << time_.GetGameTime<double>() << TEXT("-") << std::endl;
		tmpss << TEXT("Fps-") << std::setprecision(4) << GetFPS() << TEXT("-") << std::endl;
		tmpss << TEXT("DorawCall-") << GetDrawCallCount() << TEXT("-") << std::endl;
		tmpss << TEXT("LogType-") << static_cast<int>(logType_) << TEXT("-") << std::endl;
		tmpss << TEXT("DebugDraw-") << debugDrawFlag_ << TEXT("-") << std::endl;
		auto ss{ std::move(ss_) };
		tmpss << ss.str();
		
		DrawFormatString(0, 0, 0xffffff, tmpss.str().c_str());
		SetDrawScreen(targetSc_);
		DrawGraph(0, 0, screen_, true);
	}
	
	if (stopFlag_)
	{
		// ストップすべき時
		WaitKey();
		stopFlag_ = false;
	}
}

void DebugClass::DebugDraw3D(const Vector3& pos, float r, int color)
{
	if (nowDraw3D_ < draw3DQueue_.size())
	{
		draw3DQueue_[nowDraw3D_] = [pos, r, color]() { DrawSphere3D(VGet(pos.x, pos.y, pos.z), r, 16, color,color,false); };
	}
	else
	{
		draw3DQueue_.emplace_back([pos, r, color]() { DrawSphere3D(VGet(pos.x, pos.y, pos.z), r, 16, color, color, false); });
	}
	nowDraw3D_++;
}

void DebugClass::DebugDraw3D(void)
{
	if (debugDrawFlag_)
	{
		for (int i = 0; i < nowDraw3D_; i++)
		{
			draw3DQueue_[i]();
		}
	}
	nowDraw3D_ = 0;
}

void DebugClass::DebugDraw3D(const Vector3& top, const Vector3& btm, float r, int color)
{
	if (nowDraw3D_ < draw3DQueue_.size())
	{
		draw3DQueue_[nowDraw3D_] = [top, btm, r, color]() { DrawCapsule3D(VGet(top.x, top.y, top.z), VGet(btm.x, btm.y, btm.z), r, 16, color, color, false); };
	}
	else
	{
		draw3DQueue_.emplace_back([top, btm, r, color]() { DrawCapsule3D(VGet(top.x, top.y, top.z), VGet(btm.x, btm.y, btm.z), r, 16, color, color, false); });
	}
	nowDraw3D_++;
}

void DebugClass::DebugDraw3D(const Vector3& start, const Vector3& end, int color)
{
	if (nowDraw3D_ < draw3DQueue_.size())
	{
		draw3DQueue_[nowDraw3D_] = [start, end, color]() { DrawLine3D(VGet(start.x, start.y, start.z), VGet(end.x, end.y, end.z), color); };
	}
	else
	{
		draw3DQueue_.emplace_back([start, end, color]() { DrawLine3D(VGet(start.x, start.y, start.z), VGet(end.x, end.y, end.z), color); });
	}
	nowDraw3D_++;
}

void DebugClass::DebugDraw3DCube(const Vector3& pos1, const Vector3& pos2, int color)
{
	if (nowDraw3D_ < draw3DQueue_.size())
	{
		draw3DQueue_[nowDraw3D_] = [pos1, pos2, color]() { DrawCube3D(VGet(pos1.x, pos1.y, pos1.z), VGet(pos2.x, pos2.y, pos2.z), color,color,false); };
	}
	else
	{
		draw3DQueue_.emplace_back([pos1, pos2, color]() {DrawCube3D(VGet(pos1.x, pos1.y, pos1.z), VGet(pos2.x, pos2.y, pos2.z), color, color, false); });
	}
	nowDraw3D_++;
}



DebugClass::DebugClass(TimeClass& timeClass) :
	time_{timeClass}
{
	// デバッグ出力用のコンソールを生成
	AllocConsole();

	// 標準出力で出力できるようにする
	conFp_ = nullptr;
	freopen_s(&conFp_, "CONOUT$", "w", stdout);
	freopen_s(&conFp_, "CONIN$", "r", stdin);

	
	screen_ = DX_SCREEN_BACK;
	targetSc_= DX_SCREEN_BACK;

	key_.fill(0);
	keyOld_.fill(0);
	infoDrawFlag_ = false;
	stopFlag_ = false;
	debugDrawFlag_ = true;
	scH = 0;
	scW = 0;
	start_ = std::chrono::system_clock::now();
	nowDraw3D_ = 0;
	logType_ = LogType::ALL;
}

DebugClass::~DebugClass()
{

	fclose(conFp_);

}

void DebugClass::PrintLogInfo(void)
{
	auto now = std::chrono::system_clock::now();
	auto sec = std::chrono::duration_cast<std::chrono::nanoseconds>(now - start_).count() / 1000000000.0f;
	std::cout << "[Log]ThreadID=" << std::this_thread::get_id() << "Time=" << std::setprecision(5) << sec << ":";
}

void DebugClass::PrintErrorLogInfor(void)
{
	auto now = std::chrono::system_clock::now();
	auto sec = std::chrono::duration_cast<std::chrono::nanoseconds>(now - start_).count() / 1000000000.0f;
	std::cout << "[Error]ThreadID=" << std::this_thread::get_id() << "Time=" << std::setprecision(5) << sec << ":";
}

#endif

