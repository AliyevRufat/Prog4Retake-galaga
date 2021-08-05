#pragma once
#include "ComponentIncludes.h"

class AnimationComponent final : public BaseComponent
{
public:

	AnimationComponent(float frameSwitchTimer, int nrOfColumns, int nrOfRows, bool isLoop);
	void Animate();
	void Render() override;
	int GetNrOfColumns() const;
	void SetCurrentRowIndex(int currentRowIndex);
private:
	std::shared_ptr<dae::Texture2D> m_spTexture2D{};
	int m_NrOfColumns;
	int m_NrOfRows;
	int m_CurrentRowIndex;
	int m_CurrentAnimIndex;
	bool m_IsLoop;
	//
	const float m_NextFrameTimer;
	float m_NextFrameTime;
};
