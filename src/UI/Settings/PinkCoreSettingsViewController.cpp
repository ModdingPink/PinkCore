#include "UI/Settings/PinkCoreSettingsViewController.hpp"
#include "bsml/shared/BSML.hpp"
#include "Utils/UIUtils.hpp"
#include "config.hpp"
#include "logging.hpp"

DEFINE_TYPE(PinkCore::UI, PinkCoreSettingsViewController);

using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace HMUI;
using namespace BSML::Lite;

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
			customColourToggle = TOGGLE(enableCustomSongColours, "Enable Custom Song Colors");
			TOGGLE(enableCustomSongColours, "Never Use Custom Song Note Colors");
			TOGGLE(enableCustomDiffNames, "Enable Custom Difficulty Names");
			TOGGLE(enableCustomCharacteristics, "Enable Custom Characteristic Cosmetics");
			TOGGLE(enableBurnMarks, "Enable Burn Marks");
			TOGGLE(openToCustomLevels, "Open To Custom Levels");
		} else if (customColourToggle && customColourToggle->m_CachedPtr) {
			customColourToggle->set_Value(config.enableCustomSongColours);
		}

	}
}
