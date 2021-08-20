#pragma once

#ifndef VERSION
#define VERSION "0.0.1";
#endif

#ifndef ID
#define ID "PinkCore"
#endif

#define DATAPATH "/sdcard/ModData/com.beatgames.beatsaber/Mods/PinkCore/"

constexpr const char* donationURL = "https://questmodding.com/donation.php";
constexpr const char* noticeboardURL = "https://questmodding.com/vrnews.php/" VERSION;
constexpr const char* useragent = "PinkCore/" VERSION;

constexpr const char* serverUnavailable = "Unfortunately, the server couldn't be reached,\nand thus the noticeboard could not display any information";
constexpr const char* noTextReceived = "Unfortunately, the server returned an empty message,\nthus the noticeboard could not display any information";

constexpr const char* wipImagePath = DATAPATH "UI/Requirements/WIP.png";
constexpr const char* missingSpritePath = DATAPATH "UI/Requirements/MissingSprite.png";

constexpr const char* mangoImagePath = DATAPATH "UI/Mango.png";
constexpr const char* activeMangoImagePath = DATAPATH "UI/MangoActive.png";
constexpr const char* donationImagePath = DATAPATH "UI/Donation.png";
constexpr const char* activeDonationImagePath = DATAPATH "UI/DonationActive.png";
constexpr const char* requirementsImagePath = DATAPATH "UI/requirements.png";
constexpr const char* activeRequirementsImagePath = DATAPATH "UI/requirementsActive.png";

constexpr const char* requirementImagePaths[4] = { 
	DATAPATH "UI/Requirements/RequirementFound.png", 
	DATAPATH "UI/Requirements/RequirementMissing.png", 
	DATAPATH "UI/Requirements/SuggestionFound.png", 
	DATAPATH "UI/Requirements/SuggestionMissing.png"
};
