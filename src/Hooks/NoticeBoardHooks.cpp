#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "hooks.hpp"
#include "UI/Noticeboard.hpp"
#include "GlobalNamespace/MainFlowCoordinator.hpp"
#include "GlobalNamespace/PlayerStatisticsViewController.hpp"
#include "HMUI/ViewController.hpp"
#include "HMUI/ViewController_AnimationType.hpp"

#include "UnityEngine/SceneManagement/Scene.hpp"
#include "UnityEngine/SceneManagement/SceneManager.hpp"

#include "Utils/NoticeBoardText.hpp"
#include "Utils/DonationText.hpp"
#include "Polyglot/Localization.hpp"

extern Logger& getLogger();

LoggerContextObject& getNoticeBoardLogger()
{
	static LoggerContextObject logger = getLogger().WithContext("NoticeBoard Hooks");
	return logger;
}

#define INFO(...) getNoticeBoardLogger().info(__VA_ARGS__);
#define ERROR(...) getNoticeBoardLogger().error(__VA_ARGS__);

bool firstWarmup = true;
MAKE_HOOK_MATCH(SceneManager_SetActiveScene, &UnityEngine::SceneManagement::SceneManager::SetActiveScene, bool, UnityEngine::SceneManagement::Scene scene)
{
	Il2CppString* sceneNameCS = scene.get_name();
	std::string activeSceneName = sceneNameCS ? to_utf8(csstrtostr(sceneNameCS)) : "";
	INFO("Found scene %s", activeSceneName.c_str());
	
	bool result = SceneManager_SetActiveScene(scene);

	// this is a good way of doing a thing really early on, like downloading text
	if (firstWarmup && activeSceneName == "ShaderWarmup")
	{
		// when settings get reset it goes through shaderwarmup again, this way it wont run this part twice
		firstWarmup = false;

		PinkCore::NoticeBoardText::Download();
		PinkCore::DonationText::Download();
	}

	return result;
}

MAKE_HOOK_MATCH(MainFlowCoordinator_DidActivate, &GlobalNamespace::MainFlowCoordinator::DidActivate, void, GlobalNamespace::MainFlowCoordinator* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
{
	// when activating, we want to provide our own view controller for the right screen, so just take whatever is activated and display ours for right
	if (firstActivation)
	{
		auto controller = PinkCore::UI::NoticeBoard::get_instance();
		self->providedRightScreenViewController = controller;
		MainFlowCoordinator_DidActivate(self, firstActivation, addedToHierarchy, screenSystemEnabling);
		self->providedRightScreenViewController = controller;
		//self->ProvideInitialViewControllers(self->mainMenuViewController, self->leftScreenViewController, controller, self->bottomScreenViewController, self->topScreenViewController);
	}
	else MainFlowCoordinator_DidActivate(self, firstActivation, addedToHierarchy, screenSystemEnabling);
}

// using unsafe cause I can't bother with actually getting the types correct since they are irrelevant anyways
MAKE_HOOK_FIND_CLASS_UNSAFE_INSTANCE(MenuTransitionsHelper_RestartGame, "", "MenuTransitionsHelper", "RestartGame", void, Il2CppObject* self, Il2CppObject* finishCallback)
{
	PinkCore::UI::NoticeBoard::clear_instance();
	MenuTransitionsHelper_RestartGame(self, finishCallback);
}

void InstallNoticeBoardHooks(Logger& logger)
{
	SIMPLE_INSTALL_HOOK(SceneManager_SetActiveScene);
	SIMPLE_INSTALL_HOOK(MainFlowCoordinator_DidActivate);
	SIMPLE_INSTALL_HOOK(MenuTransitionsHelper_RestartGame);
}

// using a macro to register the method pointer to the class that stores all of the install methods, for automatic execution
PCInstallHooks(InstallNoticeBoardHooks)