#include "MiniginPCH.h"
#include "BulletMovementComponent.h"
#include "TransformComponent.h"
#include "GameObject.h"
#include "EngineTime.h"
#include "LerpComponent.h"

BulletMovementComponent::BulletMovementComponent(const glm::vec2& target, int speed)
	:m_Target{ target }
	, m_Speed{ speed }
{
}

void BulletMovementComponent::Init()
{
	if (m_Target != glm::vec2(0, 0))
	{
		auto currentPos = m_pGameObject->GetComponent<TransformComponent>()->GetTransform().GetPosition();
		const int targetOffset = 2;
		//
		auto BA = m_Target - currentPos;
		auto furtherTarget = glm::vec2(BA.x * targetOffset, BA.y * targetOffset);
		auto newTarget = glm::vec2(furtherTarget.x + currentPos.x, furtherTarget.y + currentPos.y);
		//
		m_pGameObject->AddComponent(new LerpComponent(newTarget, 50));
	}
}

void BulletMovementComponent::Update()
{
	if (m_Target != glm::vec2{ 0,0 })
	{
		return;
	}
	//shoot straight up
	auto transformComponent = m_pGameObject->GetComponent<TransformComponent>();
	auto currentPos = transformComponent->GetTransform().GetPosition();
	//
	transformComponent->SetPosition(glm::vec2{ currentPos.x, currentPos.y - m_Speed * EngineTime::GetInstance().GetDeltaTime() });
}