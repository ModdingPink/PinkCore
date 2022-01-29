#include "UI/Settings/PinkCoreSettingsViewController.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "Utils/UIUtils.hpp"
#include "config.hpp"
#include "logging.hpp"

DEFINE_TYPE(PinkCore::UI, PinkCoreSettingsViewController);

using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace HMUI;
using namespace QuestUI;
using namespace QuestUI::BeatSaberUI;

// simple toggle macro because typing the same thing every time is dumb
#define TOGGLE(name, displayName) \
	CreateToggle(container->get_transform(), displayName, config.name, [](bool value) -> void { \
		config.name = value; \
		SaveConfig(); \
	});

namespace PinkCore::UI
{
	void PinkCoreSettingsViewController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
	{
		if (firstActivation)
		{
			GameObject* container = CreateScrollableSettingsContainer(get_transform());
			
			TOGGLE(enableExtraSongDetails, "Enable Extra Song Details");
			TOGGLE(enableCustomSongColours, "Enable Custom Song Colors");
			TOGGLE(enableCustomDiffNames, "Enable Custom Difficulty Names");
			TOGGLE(enableBurnMarks, "Enable Burn Marks");
			TOGGLE(openToCustomLevels, "Open To Custom Levels");

			CreateUIButton(container->get_transform(), "string", [](){
				std::string test1 = "test";
				StringW test2(test1);
				std::string test3 = test2.operator std::string();
				INFO("%s == %s: %d", test1.c_str(), test3.c_str(), test1 == test3);
			});

			CreateUIButton(container->get_transform(), "u16string", [](){
				std::u16string test1 = u"test";
				StringW test2(test1);
				std::u16string test3 = test2.operator std::u16string();
				INFO("%s == %s: %d", to_utf8(test1).c_str(), to_utf8(test3).c_str(), test1 == test3);
			});

			CreateUIButton(container->get_transform(), "string -> u16string", [](){
				std::string test1 = "test";
				StringW test2(test1);
				std::u16string test3 = test2.operator std::u16string();
				INFO("%s == %s: %d", test1.c_str(), to_utf8(test3).c_str(), test1 == to_utf8(test3));
			});

			CreateUIButton(container->get_transform(), "u16string -> string", [](){
				std::u16string test1 = u"test";
				StringW test2(test1);
				std::string test3 = test2.operator std::string();
				INFO("%s == %s: %d", to_utf8(test1).c_str(), test3.c_str(), to_utf8(test1) == test3);
			});
		}
	}
}