#include "Controller.h"
#include <DxLib.h>
#include <time.h>
#include "../Debug.h"

Controller::Controller()
{
	cursorSpeed_ = 120.0f;
	cursorPos_ = lpSceneMng.screenSize_<float> / 2.0f;
	data_.fill(std::make_pair(false, false));
}

Controller::~Controller()
{

}

bool Controller::PressDecision(void) const
{
	return decisionData_.first;
}

bool Controller::PressedDecision(void) const
{
	return decisionData_.first && !decisionData_.second;
}

bool Controller::PressCancel(void) const
{
	return cancelData_.first;
}

bool Controller::PressdCancel(void) const
{
	return cancelData_.first && !cancelData_.second;
}

bool Controller::Press(InputID id)
{
	return data_[static_cast<size_t>(id)].first;
}

bool Controller::Pressed(InputID id)
{
	return data_[static_cast<size_t>(id)].first && !data_[static_cast<size_t>(id)].second;
}

bool Controller::Released(InputID id)
{
	return !data_[static_cast<size_t>(id)].first && data_[static_cast<size_t>(id)].second;
}

bool Controller::NotPress(InputID id)
{
	return !data_[static_cast<size_t>(id)].first;
}

bool Controller::IsNeutral()
{
	bool rtnflag = false;
	for (int n = 0; n < static_cast<int>(InputID::Max); n++)
	{
		rtnflag |= data_[static_cast<size_t>(static_cast<InputID>(n))].first;
	}
	return !rtnflag;
}
