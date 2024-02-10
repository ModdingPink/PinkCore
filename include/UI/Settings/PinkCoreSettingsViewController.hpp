#pragma once

#include "custom-types/shared/macros.hpp"
#include "custom-types/shared/coroutine.hpp"
#include "HMUI/ViewController.hpp"
#include "bsml/shared/BSML/Components/Settings/ToggleSetting.hpp"

DECLARE_CLASS_CODEGEN(PinkCore::UI, PinkCoreSettingsViewController, HMUI::ViewController,
	DECLARE_OVERRIDE_METHOD(void, DidActivate, il2cpp_utils::FindMethodUnsafe("HMUI", "ViewController", "DidActivate", 3), bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
	DECLARE_DEFAULT_CTOR();

	BSML::ToggleSetting* customColourToggle = nullptr;
)