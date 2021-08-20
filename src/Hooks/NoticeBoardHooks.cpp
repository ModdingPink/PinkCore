#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "Hooks.hpp"
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
	MainFlowCoordinator_DidActivate(self, firstActivation, addedToHierarchy, screenSystemEnabling);
	if (firstActivation)
	{
		auto controller = PinkCore::UI::NoticeBoard::get_instance();
		self->ProvideInitialViewControllers(self->mainMenuViewController, self->leftScreenViewController, controller, self->bottomScreenViewController, self->topScreenViewController);
	}
}

MAKE_HOOK_MATCH(MainFlowCoordinator_TopViewControllerWillChange, &GlobalNamespace::MainFlowCoordinator::TopViewControllerWillChange, void, GlobalNamespace::MainFlowCoordinator* self, HMUI::ViewController* oldViewController, HMUI::ViewController* newViewController, HMUI::ViewController::AnimationType animationType)
{
	// due to the menu behaving weird in how to play, this is the best way to get it to not look weird when switching the menus
	if (newViewController->Equals(self->howToPlayViewController))
	{
		self->SetLeftScreenViewController(nullptr, animationType);
		self->SetRightScreenViewController(nullptr, animationType);
		self->SetBottomScreenViewController(nullptr, animationType);
		static auto key = il2cpp_utils::newcsstr<il2cpp_utils::CreationType::Manual>("LABEL_HOW_TO_PLAY");
		self->SetTitle(Polyglot::Localization::Get(key), animationType);
		self->SetLeftScreenViewController(self->playerStatisticsViewController, animationType);
		self->set_showBackButton(true);
		return;
	}

	MainFlowCoordinator_TopViewControllerWillChange(self, oldViewController, newViewController, animationType);

	// if going to the main view controller
	// we want to then set our own controller as the right view controller, that way it should always display on the right in the main menu
	if (newViewController->Equals(self->mainMenuViewController))
	{
		INFO("Presenting noticeboard ViewController");
		auto controller = PinkCore::UI::NoticeBoard::get_instance();
		self->SetRightScreenViewController(controller, animationType);
	}
	/*
	else if (newViewController->Equals(self->howToPlayViewController))
	{
		// if we are going to present the how to play view controller, we need to do some fanagling to get it to not keep displaying our menu
		self->SetRightScreenViewController(nullptr, animationType);
		self->SetLeftScreenViewController(self->playerStatisticsViewController, animationType);
		//self->SetRightScreenViewController(self->playerStatisticsViewController, HMUI::ViewController::AnimationType::In);
	}
	*/
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
	SIMPLE_INSTALL_HOOK(MainFlowCoordinator_TopViewControllerWillChange);
	SIMPLE_INSTALL_HOOK(MenuTransitionsHelper_RestartGame);
}

// using a macro to register the method pointer to the class that stores all of the install methods, for automatic execution
PCInstallHooks(InstallNoticeBoardHooks)