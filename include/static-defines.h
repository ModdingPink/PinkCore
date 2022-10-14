#pragma once

#ifndef VERSION
#define VERSION "0.0.1";
#endif

#ifndef MOD_ID
#define MOD_ID "PinkCore"
#endif

#define DATAPATH "/sdcard/ModData/com.beatgames.beatsaber/Mods/PinkCore/"

constexpr const char* donationURL = "https://questmodding.com/donation.php";
constexpr const char* noticeboardURL = "https://questmodding.com/vrnews.php/" VERSION;
constexpr const char* useragent = "PinkCore/" VERSION;

constexpr const char* serverUnavailable = "Unfortunately, the server couldn't be reached,\nand thus the noticeboard could not display any information";
constexpr const char* noTextReceived = "Unfortunately, the server returned an empty message,\nthus the noticeboard could not display any information";
