#pragma once
#include "BeeState.h"
class BeeFormationState final : public BeeState
{
public:
	BeeFormationState();

	void Update(BeeStateManager& bee) override;
	virtual BeeState* StateSwitch();
	void Enter(BeeStateManager& bee);
private:

	const int m_TimerBeforeDiving;
	float m_TimeBeforeDiving;
};
