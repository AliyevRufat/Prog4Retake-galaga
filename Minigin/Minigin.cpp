#include "MiniginPCH.h"
#include "Minigin.h"
#include "SDL.h"
#include <chrono>
#include <thread>
#include "Commands.h"
#include "Observer.h"
#include "ConsoleAudioService.h"
#include "Locator.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "InputManager.h"
#include "EngineTime.h"
#include "LivesObserver.h"
#include "ScoreObserver.h"
#include "GyaragaMovementComponent.h"
#include "PlayerWeaponComponent.h"
#include "BeeStateManager.h"
#include "CollisionDetectionManager.h"
#include "FormationManager.h"

using namespace std;
using namespace std::chrono;

void dae::Minigin::Initialize()
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		throw std::runtime_error(std::string("SDL_Init_Video Error: ") + SDL_GetError());
	}

	if (SDL_Init(SDL_INIT_AUDIO) != 0)
	{
		throw std::runtime_error(std::string("SDL_Init_Audio Error: ") + SDL_GetError());
	}

	int frequency = 44100;
	int chunkSize = 2048;
	int channels = 2;

	if (Mix_OpenAudio(frequency, MIX_DEFAULT_FORMAT, channels, chunkSize) < 0)
		throw std::runtime_error(std::string("SDL_Audio Error: ") + Mix_GetError());

	m_Window = SDL_CreateWindow(
		"GALAGA",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		700,
		850,
		SDL_WINDOW_OPENGL
	);
	if (m_Window == nullptr)
	{
		throw std::runtime_error(std::string("SDL_CreateWindow Error: ") + SDL_GetError());
	}
	Renderer::GetInstance().Init(m_Window);

	//Locator::Provide(new ConsoleAudioService());
	//Locator::GetAudio().AddSound(AudioService::SoundIds::FallEffect, "../Data/Sounds/.wav", true); // TODO : get correct sound/music names
	//Locator::GetAudio().AddSound(AudioService::SoundIds::JumpEffect, "../Data//Sounds/.wav", true);
	//Locator::GetAudio().AddSound(AudioService::SoundIds::LiftEffect, "../Data//Sounds/.wav", true);
	//Locator::GetAudio().AddSound(AudioService::SoundIds::VictoryEffect, "../Data//Sounds/.wav", true);
	//Locator::GetAudio().AddSound(AudioService::SoundIds::Hurt, "../Data//Sounds/.wav", true);

	//where second parameter = volume
}

void dae::Minigin::InitGame()
{
	//window size
	const auto window = SDL_GetWindowSurface(m_Window);
	//create and get scene
	SceneManager::GetInstance().CreateScene("Galaga");
	auto scene = SceneManager::GetInstance().GetCurrentScene();

	//---------------------------------------------------------------------FPS COUNTER--------------------------------------------------
	//fps counter
	auto go = std::make_shared<GameObject>("FPSCounter");
	auto font = ResourceManager::GetInstance().LoadFont("Lingua.otf", 14);
	go->AddComponent(new FPSTextComponent(font));
	scene->Add(go);
	//---------------------------------------------------------------------SCORE DISPLAY--------------------------------------------------
	auto scoreDisplay = std::make_shared<GameObject>("ScoreDisplay");
	scoreDisplay->AddComponent(new TransformComponent(glm::vec2(150, 50)));
	font = ResourceManager::GetInstance().LoadFont("Lingua.otf", 20);
	auto scoreCounter = new TextComponent("Score: 0", font, SDL_Color{ 255,255,255 });
	scoreDisplay->AddComponent(scoreCounter);
	scene->Add(scoreDisplay);
	//---------------------------------------------------------------------LIVES DISPLAY--------------------------------------------------
	auto livesDisplay = std::make_shared<GameObject>("LivesDisplay");
	livesDisplay->AddComponent(new TransformComponent(glm::vec2(350, 50)));
	font = ResourceManager::GetInstance().LoadFont("Lingua.otf", 20);
	auto livesCounter = new TextComponent("Remaining lives: 3", font, SDL_Color{ 255,255,255 });
	livesDisplay->AddComponent(livesCounter);
	scene->Add(livesDisplay);
	//---------------------------------------------------------------------PLAYER--------------------------------------------------
	const int playerScale = 1;
	const int playerWidth = 45 * playerScale;
	const int playerHeight = 43 * playerScale;
	auto gyaraga = std::make_shared<GameObject>("Gyaraga");
	//Components
	gyaraga->AddComponent(new TransformComponent(glm::vec2(window->w / 2 - playerWidth / 2, window->h - window->h / 8 - playerHeight / 2), glm::vec2(playerWidth, playerHeight)));
	gyaraga->AddComponent(new HealthComponent(3));
	gyaraga->AddComponent(new ScoreComponent(0));
	gyaraga->AddComponent(new Texture2DComponent("Gyaraga.png", playerScale, false));
	gyaraga->AddComponent(new GyaragaMovementComponent(window));
	gyaraga->AddComponent(new PlayerWeaponComponent(window, playerWidth));
	//watchers
	gyaraga->AddWatcher(new ScoreObserver());
	gyaraga->AddWatcher(new LivesObserver());
	//add
	scene->Add(gyaraga);
	scene->AddPlayer(gyaraga);
	CollisionDetectionManager::GetInstance().AddCollisionGameObject(gyaraga);
	//player died text
	auto playerDied = std::make_shared<GameObject>("Player 1 Died!");
	playerDied->AddComponent(new TransformComponent(glm::vec2(500, 300)));
	playerDied->AddComponent(new TextComponent("Player 1 Died!", font, SDL_Color{ 255,255,255 }, false));
	scene->Add(playerDied);
}

void dae::Minigin::BindCommands()
{
	auto& inputManager = InputManager::GetInstance();

	for (int i = 0; i < XUSER_MAX_COUNT; i++)
	{
		//assign buttons
		//inputManager.AssignKey<ChooseCoOpGameMode>(ControllerButton::ButtonA, i);
		//inputManager.AssignKey<ChooseVersusGameMode>(ControllerButton::ButtonB, i);
		//inputManager.AssignKey<ChooseSinglePlayerGameMode>(ControllerButton::ButtonX, i);
		//inputManager.AssignKey<GoToMenu>(ControllerButton::ButtonY, i);
		inputManager.AssignKey<ExitCommand>(ControllerButton::ButtonSelect, i);
		//move
		inputManager.AssignKey<SteerLeft>(ControllerButton::ButtonLeft, i);
		inputManager.AssignKey<SteerRight>(ControllerButton::ButtonRight, i);
	}
	//keyboard
	inputManager.AssignKey<SteerLeft>(KeyboardButton::ArrowLeft, 0);
	inputManager.AssignKey<SteerRight>(KeyboardButton::ArrowRight, 0);
	inputManager.AssignKey<Fire>(KeyboardButton::SPACE, 0);

	//inputManager.AssignKey<ChooseCoOpGameMode>(KeyboardButton::O, 0);
	//inputManager.AssignKey<ChooseVersusGameMode>(KeyboardButton::P, 0);
	//inputManager.AssignKey<ChooseSinglePlayerGameMode>(KeyboardButton::I, 0);
	//inputManager.AssignKey<GoToMenu>(KeyboardButton::U, 0);
	inputManager.AssignKey<ExitCommand>(KeyboardButton::ESCAPE, 0);
}

void dae::Minigin::Cleanup()
{
	Locator::FreeResources();
	Renderer::GetInstance().Destroy();
	SDL_DestroyWindow(m_Window);
	m_Window = nullptr;
	Mix_Quit();
	SDL_Quit();
}

void dae::Minigin::Run()
{
	Initialize();

	// tell the resource manager where he can find the game data
	ResourceManager::GetInstance().Init("../Data/");

	auto& renderer{ Renderer::GetInstance() };
	auto& sceneManager{ SceneManager::GetInstance() };
	auto& input{ InputManager::GetInstance() };

	bool doContinue{ true };
	auto lastTime{ high_resolution_clock::now() };

	std::thread soundThread(&AudioService::Update, &Locator::GetAudio());

	BindCommands();
	FormationManager::GetInstance().Init();
	InitGame();

	while (doContinue)
	{
		const auto currentTime{ high_resolution_clock::now() };
		const float deltaTime{ duration<float>(currentTime - lastTime).count() };
		lastTime = currentTime;

		//---------------------------------------------------------------------ENEMIES--------------------------------------------------
		const int beeWidth = 35;
		const int beeHeight = 41;
		timer += EngineTime::GetInstance().GetDeltaTime();
		if (timer >= 0.2f && size < 10)
		{
			timer = 0;
			auto beeEnemy = std::make_shared<GameObject>("Bee");
			beeEnemy->AddComponent(new TransformComponent(glm::vec2(350 + 50, -10), glm::vec2(beeWidth, beeHeight)));
			beeEnemy->AddComponent(new Texture2DComponent("Bee.png", 1, false));
			beeEnemy->AddComponent(new BeeStateManager(size));
			SceneManager::GetInstance().GetCurrentScene()->Add(beeEnemy);
			CollisionDetectionManager::GetInstance().AddCollisionGameObject(beeEnemy);
			size++;
		}

		input.ProcessInput();
		input.ControllerAnalogs();
		input.InputHandler();
		doContinue = input.KeyboardInput();
		EngineTime::GetInstance().SetDeltaTime(deltaTime);
		FormationManager::GetInstance().Update();
		CollisionDetectionManager::GetInstance().Update();

		//collision check
		//enemy etc. update
		sceneManager.Update();
		renderer.Render();
	}

	soundThread.detach();

	Cleanup();
}