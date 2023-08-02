#include <DxLib.h>
#include "Time.h"
#include "Debug.h"

void TimeClass::DeltaTimeStart(void)
{
	start_ = std::chrono::system_clock::now();
}

void TimeClass::DeltaTimeEnd(void)
{
	if (GetNoActiveState())
	{
		start_ = deltaEnd_ = std::chrono::system_clock::now();
		return;
	}

	deltaEnd_ = std::chrono::system_clock::now();
	delta_ =  std::chrono::duration_cast<std::chrono::nanoseconds>(deltaEnd_ - start_).count() / 1000000000.0;
}

void TimeClass::GameTimeEnd(void)
{
	if (GetNoActiveState())
	{
		gameTimeEnd_ = std::chrono::system_clock::now();
		return;
	}
	gameTimeEnd_ = std::chrono::system_clock::now();
	gameTime_ = std::chrono::duration_cast<std::chrono::nanoseconds>(gameTimeEnd_ - start_).count() / 1000000000.0;
}

