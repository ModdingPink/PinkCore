#pragma once

#include "custom-types/shared/macros.hpp"
#include "custom-types/shared/coroutine.hpp"
#include "HMUI/ViewController.hpp"

DECLARE_CLASS_CODEGEN(PinkCore::UI, PinkCoreDonationViewController, HMUI::ViewController,
	DECLARE_OVERRIDE_METHOD(void, DidActivate, il2cpp_utils::FindMethodUnsafe("HMUI", "ViewController", "DidActivate", 3), bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
)