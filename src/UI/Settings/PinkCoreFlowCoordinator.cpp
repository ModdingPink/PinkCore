#include "UI/Settings/PinkCoreFlowCoordinator.hpp"
#include "UI/Settings/PinkCoreSettingsViewController.hpp"
#include "UI/Settings/PinkCoreDonationViewController.hpp"

#include "bsml/shared/BSML.hpp"
#include "bsml/shared/Helpers/creation.hpp"
#include "Utils/UIUtils.hpp"

#include "HMUI/TitleViewController.hpp"

DEFINE_TYPE(PinkCore::UI, PinkCoreFlowCoordinator);

using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace HMUI;
using namespace BSML::Lite;

namespace PinkCore::UI
{
	void PinkCoreFlowCoordinator::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
	{
		if (firstActivation)
		{
			pinkCoreSettingsViewController = BSML::Helpers::CreateViewController<PinkCoreSettingsViewController*>();
			pinkCoreDonationViewController = BSML::Helpers::CreateViewController<PinkCoreDonationViewController*>();

			SetTitle("PinkCore Settings", ViewController::AnimationType::Out);
			showBackButton = true;

			ProvideInitialViewControllers(pinkCoreSettingsViewController, nullptr, pinkCoreDonationViewController, nullptr, nullptr);
		}

		TitleViewController* titleView = Object::FindObjectOfType<TitleViewController*>();
		static Color pinkColor(0.94f, 0.11f, 0.5f, 1.0f);
		UIUtils::SetTitleColor(titleView, pinkColor);
	}

	void PinkCoreFlowCoordinator::BackButtonWasPressed(HMUI::ViewController* topViewController)
	{
		TitleViewController* titleView = Object::FindObjectOfType<TitleViewController*>();
		this->_parentFlowCoordinator->DismissFlowCoordinator(this, ViewController::AnimationDirection::Horizontal, nullptr, false);
		UIUtils::SetTitleColor(titleView, Color(0.0f, 0.75f, 1.0f, 1.0f), true);
	}
}
