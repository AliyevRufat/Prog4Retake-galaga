#include "MiniginPCH.h"
#include "EnemyManager.h"
#include "EnemyStateManager.h"
#include "SceneManager.h"
#include "Texture2DComponent.h"
#include "TransformComponent.h"
#include "Scene.h"
#include "CollisionDetectionManager.h"
#include "EnemyWeaponComponent.h"
#include "AnimationComponent.h"

void EnemyManager::Update()
{
	m_SpawnTime += EngineTime::GetInstance().GetDeltaTime();

	if (m_SpawnTime >= m_SpawnTimer && m_Index < m_QueuedEnemies.size())
	{
		m_SpawnTime -= m_SpawnTime;
		SpawnEnemy(m_QueuedEnemies[m_Index].first, m_QueuedEnemies[m_Index].second.first, m_QueuedEnemies[m_Index].second.second);
		++m_Index;
	}
}

void EnemyManager::QueueEnemy(EnemyType enemyType, int formationRowIndex, int formationIndex)
{
	m_QueuedEnemies.push_back(std::pair(enemyType, std::pair(formationRowIndex, formationIndex)));
}

void EnemyManager::SpawnEnemy(EnemyType enemyType, int formationRowIndex, int formationIndex)
{
	switch (enemyType)
	{
	case EnemyType::Bee:
		SpawnBee(enemyType, formationRowIndex, formationIndex);
		break;
	case EnemyType::Butterfly:
		SpawnButterfly(enemyType, formationRowIndex, formationIndex);
		break;
	case EnemyType::Boss:
		SpawnBoss(enemyType, formationRowIndex, formationIndex);
		break;
	}
}

void EnemyManager::SpawnBee(EnemyType enemyType, int formationRowIndex, int formationIndex)
{
	const int beeWidth = 48;
	const int beeHeight = 37;
	//
	auto beeEnemy = std::make_shared<GameObject>("Bee");
	beeEnemy->AddComponent(new TransformComponent(glm::vec2(350 + 50, -10), glm::vec2(beeWidth, beeHeight)));
	beeEnemy->AddComponent(new Texture2DComponent("Bee.png", 1, true));
	beeEnemy->AddComponent(new AnimationComponent(0.2f, 2, 1, true));
	beeEnemy->AddComponent(new EnemyStateManager(enemyType, formationRowIndex, formationIndex));
	beeEnemy->AddComponent(new EnemyWeaponComponent(beeWidth));
	dae::SceneManager::GetInstance().GetCurrentScene()->Add(beeEnemy);
	CollisionDetectionManager::GetInstance().AddCollisionGameObject(beeEnemy);
}

void EnemyManager::SpawnButterfly(EnemyType enemyType, int formationRowIndex, int formationIndex)
{
	const int butterflyWidth = 48;
	const int butterflyHeight = 37;
	//
	auto butterflyEnemy = std::make_shared<GameObject>("Butterfly");
	butterflyEnemy->AddComponent(new TransformComponent(glm::vec2(350 - 50, -10), glm::vec2(butterflyWidth, butterflyHeight)));
	butterflyEnemy->AddComponent(new Texture2DComponent("Butterfly.png", 1, true));
	butterflyEnemy->AddComponent(new AnimationComponent(0.2f, 2, 1, true));
	butterflyEnemy->AddComponent(new EnemyStateManager(enemyType, formationRowIndex, formationIndex));
	butterflyEnemy->AddComponent(new EnemyWeaponComponent(butterflyWidth));
	dae::SceneManager::GetInstance().GetCurrentScene()->Add(butterflyEnemy);
	CollisionDetectionManager::GetInstance().AddCollisionGameObject(butterflyEnemy);
}

void EnemyManager::SpawnBoss(EnemyType, int, int)
{
}

void EnemyManager::ClearEnemies()
{
	m_Index = 0;
	m_SpawnTime = 0.0f;
	m_QueuedEnemies.clear();
}

bool EnemyManager::CanDive() const
{
	return m_AmountOfDivingEnemies < m_MaxAmountOfDivingEnemies;
}

void EnemyManager::IncreaseAmountOfDivingEnemies()
{
	++m_AmountOfDivingEnemies;
}

void EnemyManager::DecreaseAmountOfDivingEnemies()
{
	--m_AmountOfDivingEnemies;
}