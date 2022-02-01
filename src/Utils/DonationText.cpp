#include "Utils/DonationText.hpp"
#include "Utils/WebUtils.hpp"
#include "static-defines.h"
#include "logging.hpp"

namespace PinkCore
{
	std::string DonationText::text = "";
	void DonationText::Download()
	{
		// safety to prevent from downloading multiple times
		if (text != "") return;
		
		INFO("Getting text from URL %s", donationURL);

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