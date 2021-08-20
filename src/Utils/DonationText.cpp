#include "Utils/DonationText.hpp"
#include "Utils/WebUtils.hpp"
#include "static-defines.h"
#include "beatsaber-hook/shared/utils/logging.hpp"

extern Logger& getLogger();

namespace PinkCore
{
	std::string DonationText::text = "";
	void DonationText::Download()
	{
		// safety to prevent from downloading multiple times
		if (text != "") return;
		
		getLogger().info("Getting text from URL %s", donationURL);

		WebUtils::GetAsync(donationURL, [&](long code, std::string result){
			switch (code)
			{
				case 200:
					text = result;
					break;
				default:
					text = serverUnavailable;
					break;
				if (text == "") text = noTextReceived;
			}
		});
	}

	const std::string& DonationText::get_text()
	{
		return text;
	}
}