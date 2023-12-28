#pragma once

#include "custom-types/shared/macros.hpp"
#include "custom-types/shared/coroutine.hpp"
#include "HMUI/ViewController.hpp"
#include "UnityEngine/UI/Toggle.hpp"
#include "bsml/shared/BSML/Components/Settings/ToggleSetting.hpp"

DECLARE_CLASS_CODEGEN(PinkCore::UI, PinkCoreSettingsViewController, HMUI::ViewController,
	DECLARE_INSTANCE_FIELD(BSML::ToggleSetting*, customColourToggle);
	DECLARE_OVERRIDE_METHOD_MATCH(void, DidActivate, &HMUI::ViewController::DidActivate, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
	DECLARE_DEFAULT_CTOR();
)
