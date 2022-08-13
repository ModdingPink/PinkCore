#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "logging.hpp"
#include "assets.hpp"
#include "hooks.hpp"
#include "UI/Noticeboard.hpp"
#include "GlobalNamespace/MainFlowCoordinator.hpp"
#include "GlobalNamespace/PlayerStatisticsViewController.hpp"
#include "GlobalNamespace/MainMenuViewController.hpp"
#include "HMUI/ViewController.hpp"
#include "HMUI/HoverHint.hpp"
#include "HMUI/ButtonSpriteSwap.hpp"
#include "HMUI/ViewController_AnimationType.hpp"
#include "questui/shared/BeatSaberUI.hpp"

#include "UnityEngine/SceneManagement/Scene.hpp"
#include "UnityEngine/SceneManagement/SceneManager.hpp"
#include "UnityEngine/UI/Button.hpp"
#include "UnityEngine/Events/UnityAction.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"
#include "custom-types/shared/delegate.hpp"

#include "Utils/NoticeBoardText.hpp"
#include "Utils/DonationText.hpp"
#include "Polyglot/Localization.hpp" 
#include "questui/shared/BeatSaberUI.hpp"

bool firstWarmup = true;
bool setIcons = false;
#define MakeDelegate(DelegateType, varName) (custom_types::MakeDelegate<DelegateType>(classof(DelegateType), varName))

// pink told me to comment this out idk
// MAKE_AUTO_HOOK_MATCH(MainMenuViewController_DidActivate, &GlobalNamespace::MainMenuViewController::DidActivate, void, GlobalNamespace::MainMenuViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
// {
//     MainMenuViewController_DidActivate(self, firstActivation, addedToHierarchy, screenSystemEnabling); 
//     if(setIcons) return; 
//     //reusing the editor button
//     auto newsButton = UnityEngine::GameObject::Instantiate(self->beatmapEditorButton->get_gameObject(), self->beatmapEditorButton->get_transform()->get_parent(), false);
// 	newsButton->get_gameObject()->set_active(true);
// 	QuestUI::BeatSaberUI::AddHoverHint(newsButton, "Beat Saber Modding News");
//     //swap the default button icons to our own button images
//     HMUI::ButtonSpriteSwap* spriteSwap = newsButton->get_gameObject()->GetComponent<HMUI::ButtonSpriteSwap*>();
//     auto highlightedImage = QuestUI::BeatSaberUI::VectorToSprite(std::vector<uint8_t>(_binary_MainMenuIconHighlight_png_start,_binary_MainMenuIconHighlight_png_end));
//     auto defaultImage = QuestUI::BeatSaberUI::VectorToSprite(std::vector<uint8_t>(_binary_MainMenuIcon_png_start,_binary_MainMenuIcon_png_end));
//     spriteSwap->normalStateSprite = defaultImage;
//     spriteSwap->highlightStateSprite = highlightedImage;
//     spriteSwap->pressedStateSprite = highlightedImage;
//     spriteSwap->disabledStateSprite = defaultImage;
//     setIcons = true;
// 	newsButton->GetComponent<UnityEngine::UI::Button*>()->get_onClick()->AddListener(MakeDelegate(UnityEngine::Events::UnityAction*, (std::function<void()>)[](){ PinkCore::UI::NoticeBoard::get_instance()->ToggleVisibility(); }));
// 	newsButton->get_transform()->SetSiblingIndex(newsButton->get_transform()->GetSiblingIndex()-1);
// }

MAKE_AUTO_HOOK_MATCH(SceneManager_SetActiveScene, &UnityEngine::SceneManagement::SceneManager::SetActiveScene, bool, UnityEngine::SceneManagement::Scene scene)
{
	StringW sceneNameCS = scene.get_name();
	std::string activeSceneName("");
	if (sceneNameCS) activeSceneName = static_cast<std::string>(sceneNameCS);
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

MAKE_AUTO_HOOK_MATCH(MainFlowCoordinator_DidActivate, &GlobalNamespace::MainFlowCoordinator::DidActivate, void, GlobalNamespace::MainFlowCoordinator* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
{
	// when activating, we want to provide our own view controller for the right screen, so just take whatever is activated and display ours for right
	if (firstActivation)
	{
		auto controller = PinkCore::UI::NoticeBoard::get_instance();
		self->providedRightScreenViewController = controller;
		self->providedBottomScreenViewController = controller;
		MainFlowCoordinator_DidActivate(self, firstActivation, addedToHierarchy, screenSystemEnabling);
		self->providedRightScreenViewController = controller;
		//self->ProvideInitialViewControllers(self->mainMenuViewController, self->leftScreenViewController, controller, self->bottomScreenViewController, self->topScreenViewController);
	}
	else MainFlowCoordinator_DidActivate(self, firstActivation, addedToHierarchy, screenSystemEnabling);
}

// using unsafe cause I can't bother with actually getting the types correct since they are irrelevant anyways
MAKE_AUTO_HOOK_FIND_CLASS_UNSAFE_INSTANCE(MenuTransitionsHelper_RestartGame, "", "MenuTransitionsHelper", "RestartGame", void, Il2CppObject* self, Il2CppObject* finishCallback)
{
	PinkCore::UI::NoticeBoard::clear_instance();
	setIcons = false;
	MenuTransitionsHelper_RestartGame(self, finishCallback);
}
