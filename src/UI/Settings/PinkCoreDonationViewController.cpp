#include "UI/Settings/PinkCoreDonationViewController.hpp"
#include "bsml/shared/BSML.hpp"
#include "bsml/shared/BSML/Components/ExternalComponents.hpp"
#include "bsml/shared/BSML/Components/Backgroundable.hpp"

#include "Utils/UIUtils.hpp"
#include "Utils/DonationText.hpp"

#include "UnityEngine/UI/LayoutElement.hpp"
#include "UnityEngine/RectOffset.hpp"
DEFINE_TYPE(PinkCore::UI, PinkCoreDonationViewController);

using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace HMUI;
using namespace TMPro;
using namespace BSML::Lite;

namespace PinkCore::UI
{
	void PinkCoreDonationViewController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
	{
		if (firstActivation)
		{
			UIUtils::AddHeader(get_transform(), "Donations", Color(0.94f, 0.11f, 0.5f, 1.0f));

			GameObject* container = CreateScrollableSettingsContainer(get_transform());

			auto components = container->GetComponent<BSML::ExternalComponents*>();
			RectTransform* rect = components->Get<RectTransform*>();
			rect->set_sizeDelta({0.0f, 0.0f});

			auto* backgroundable = container->GetComponent<BSML::Backgroundable*>();
			if (!backgroundable) backgroundable = container->AddComponent<BSML::Backgroundable*>();
			backgroundable->ApplyBackground("round-rect-panel");
			backgroundable->ApplyAlpha(0.5f);

			auto* layoutgroup = container->GetComponent<VerticalLayoutGroup*>();
			RectOffset* offset = RectOffset::New_ctor(2, 2, 0, 0);
			layoutgroup->set_padding(offset);


			HorizontalLayoutGroup* horizon = CreateHorizontalLayoutGroup(container->get_transform());
			VerticalLayoutGroup* layout = CreateVerticalLayoutGroup(horizon->get_transform());

			auto text = CreateText(layout->get_transform(), DonationText::get_text());
			text->set_fontSize(text->get_fontSize() * 0.65f);
			text->set_alignment(TextAlignmentOptions::MidlineLeft);
			text->set_enableWordWrapping(true);

			LayoutElement* noticelayout = text->get_gameObject()->AddComponent<LayoutElement*>();
			layout->set_childControlHeight(true);
			layout->set_childForceExpandHeight(true);
			layout->set_childControlWidth(true);
			layout->set_childForceExpandWidth(true);

			horizon->set_childControlWidth(true);
			horizon->set_childForceExpandWidth(true);

			noticelayout->set_preferredWidth(80.0f);
		}
	}
}
