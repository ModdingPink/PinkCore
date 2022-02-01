#include "Utils/NoticeBoardText.hpp"
#include "Utils/WebUtils.hpp"
#include "static-defines.h"
#include "logging.hpp"

namespace PinkCore
{
	std::string NoticeBoardText::text = "";
	void NoticeBoardText::Download()
	{
		// safety to prevent from downloading multiple times
		if (text != "") return;
		
		INFO("Getting text from URL %s", noticeboardURL);

		WebUtils::GetAsync(noticeboardURL, [&](long code, std::string result){
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

	const std::string& NoticeBoardText::get_text()
	{
		return text;
	}
}