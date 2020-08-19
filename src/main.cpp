#include <dlfcn.h>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <algorithm>

#include "../extern/beatsaber-hook/rapidjson/include/rapidjson/document.h"
#include "../extern/beatsaber-hook/shared/utils/utils.h"
#include "../extern/beatsaber-hook/shared/utils/logging.hpp"
#include "../extern/beatsaber-hook/include/modloader.hpp"
#include "../extern/beatsaber-hook/shared/utils/typedefs.h"
#include "../extern/beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "../extern/beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "../extern/beatsaber-hook/shared/config/config-utils.hpp"

#define PATH "/sdcard/Android/data/com.beatgames.beatsaber/files/logdump-"
#define EXT ".txt"

bool burnMarkTrailsEnabled = true;
//bool debugMode = true;
bool enableCustomLevelsTab = true;
bool enableCharacteristicsColours = true;
bool enableRequirementDetails = true;
bool enableExtraSongDetails = true;
bool enableCustomSongColours = true;
//bool disable360EnvironmentOnNon360Levels = true;
bool enableCustomDiffNames = true;
bool alwaysOpenToCustomLevels = true;
bool enableChromaLite = true;
bool currentlyPlayingLevel = false;
bool enableNoticeBoard = true;
std::string Easy = "";
std::string Normal = "";
std::string Hard = "";
std::string Expert = "";
std::string ExpertPlus = "";
std::string lastPhysicallySelectedCharacteristic = "";

Il2CppObject *textPageScrollView = nullptr;
Il2CppObject *WebRequestAPI = nullptr;
Il2CppObject *DownloadHandler = nullptr;
Il2CppObject *WebRequestAsyncOp = nullptr;
Il2CppString *JsonStr = nullptr;
std::string modVersion = "1.2.1";
Il2CppObject *releaseInfoViewController = nullptr;

bool currentlySelectedIsCustomSong = true;

static rapidjson::Document d;

static ModInfo modInfo;


static Configuration& getConfig() {
    static Configuration config(modInfo);
    return config;
}

static const Logger& getLogger() {
    static const Logger logger(modInfo);
    return logger;
}

void write_info(FILE* fp, std::string str) {
    getLogger().debug("%s", str.data());
    fwrite((str + "\n").data(), str.length() + 1, 1, fp);
}

std::string getHash(std::string levelId) {
	if (levelId.find("_") == 6) {
		return levelId.erase(0, 13);
	}
	if (levelId.find_first_of("_") == 45) {
		return levelId.erase(0, 45);
	}
	std::string error = "error";
	return error;
}

float getValue(int rgb, int shift) {
	int value = (rgb >> shift) & 0x0ff; //Shitfs the bits 'shift' amounts of times to the right and only looks at the last two bytes. eg a number which maxes to 255 
	float floatValue = (value / 255.0); //make a float which a colour can use, a number between 0 and 1 hense we devide by 255.0
	return (floatValue);
}



std::string text = "<size=6><mspace=3.5><align=\"center\">NO INTERNET</align></mspace></size><br><color=#88202088>Error</color><br>-<indent=3>The NoticeBoard Part Of PinkUtils Reqires an Internet Connection to Function</indent>";

void CompletedWebRequest()
{
	DownloadHandler = RET_V_UNLESS(il2cpp_utils::RunMethod(WebRequestAPI, "get_downloadHandler"));
	JsonStr = RET_V_UNLESS(il2cpp_utils::RunMethod<Il2CppString*>(DownloadHandler, "GetText"));
	std::string requestCompleteText = to_utf8(csstrtostr(JsonStr));
	text = requestCompleteText;
	if (text.empty()) {
		text = "<size=6><mspace=3.5><align=\"center\">NO INTERNET</align></mspace></size><br><color=#88202088>Error</color><br>-<indent=3>The NoticeBoard Part Of PinkUtils Reqires an Internet Connection to Function</indent>";
	}
	if (!textPageScrollView) {
		Il2CppString *changeStr = il2cpp_utils::createcsstr(text.c_str());
		il2cpp_utils::RunMethod(textPageScrollView, "SetText", changeStr);
	}
}

void WebRequest() {
	Il2CppString *urlPath = il2cpp_utils::createcsstr("http://www.questboard.xyz/vrnews.php/" + modVersion);
	WebRequestAPI = RET_V_UNLESS(il2cpp_utils::RunMethod("UnityEngine.Networking", "UnityWebRequest", "Get", urlPath));
	RET_V_UNLESS(il2cpp_utils::RunMethod(WebRequestAPI, "SetRequestHeader", il2cpp_utils::createcsstr("User-Agent"), il2cpp_utils::createcsstr("PPViewer/")));
	WebRequestAsyncOp = RET_V_UNLESS(il2cpp_utils::RunMethod(WebRequestAPI, "SendWebRequest"));
	auto* methodInfo = RET_V_UNLESS(il2cpp_utils::FindMethodUnsafe(WebRequestAsyncOp, "add_completed", 1));
	auto action = RET_V_UNLESS(il2cpp_utils::MakeAction(methodInfo, 0, nullptr, CompletedWebRequest));
	RET_V_UNLESS(il2cpp_utils::RunMethod(WebRequestAsyncOp, methodInfo, action));
}

MAKE_HOOK_OFFSETLESS(FlowCoordinator_ProvideInitialViewControllers, void, Il2CppObject *self, Il2CppObject *mainViewController, Il2CppObject *leftScreenViewController, Il2CppObject *rightScreenViewController, Il2CppObject *bottomScreenViewController, Il2CppObject *topScreenViewController) {
	if (!enableNoticeBoard) {
		FlowCoordinator_ProvideInitialViewControllers(self, mainViewController, leftScreenViewController, rightScreenViewController, bottomScreenViewController, topScreenViewController);
		return;
	}
	
	auto* mainViewControllerName = RET_V_UNLESS(il2cpp_utils::RunMethod<Il2CppString*>(mainViewController, "get_name"));
	if (strcmp(to_utf8(csstrtostr(mainViewControllerName)).c_str(), "MainMenuViewController") == 0) {
		releaseInfoViewController = RET_V_UNLESS(il2cpp_utils::GetFieldValue(self, "_releaseInfoViewController"));
		FlowCoordinator_ProvideInitialViewControllers(self, mainViewController, releaseInfoViewController, rightScreenViewController, bottomScreenViewController, topScreenViewController);
	}
	else {
		FlowCoordinator_ProvideInitialViewControllers(self, mainViewController, leftScreenViewController, rightScreenViewController, bottomScreenViewController, topScreenViewController);
	}
}

MAKE_HOOK_OFFSETLESS(FlowCoordinator_TopViewControllerWillChange, void, Il2CppObject *self, Il2CppObject *oldViewController, Il2CppObject *newViewController, bool immediately) {
	FlowCoordinator_TopViewControllerWillChange(self, oldViewController, newViewController, immediately);
	if (!enableNoticeBoard) {
		return;
	}
	auto* newViewControllerName = RET_V_UNLESS(il2cpp_utils::RunMethod<Il2CppString*>(newViewController, "get_name"));
	if (strcmp(to_utf8(csstrtostr(newViewControllerName)).c_str(), "MainMenuViewController") == 0)
	{
		releaseInfoViewController = RET_V_UNLESS(il2cpp_utils::GetFieldValue(self, "_releaseInfoViewController"));
		RET_V_UNLESS(il2cpp_utils::RunMethod(self, "SetLeftScreenViewController", releaseInfoViewController, immediately));
	}

}

MAKE_HOOK_OFFSETLESS(ReleaseInfoViewController_TextChange, void, Il2CppObject *self, bool firstActivation, Il2CppObject *ViewController) {
	
	if (!enableNoticeBoard) {
		return;
	}
	if (textPageScrollView == nullptr) {
		WebRequest();
	}

	textPageScrollView = RET_V_UNLESS(il2cpp_utils::GetFieldValue(self, "_textPageScrollView"));

	if (text.empty()) {
		text = "<size=6><mspace=3.5><align=\"center\">NO INTERNET</align></mspace></size><br><color=#88202088>Error</color><br>-<indent=3>This Mod Reqires an Internet Connection to Function</indent>";
	}
	//WebRequest();
	Il2CppString *changeStr = il2cpp_utils::createcsstr(text.c_str());
	RET_V_UNLESS(il2cpp_utils::RunMethod(textPageScrollView, "SetText", changeStr));
}

MAKE_HOOK_OFFSETLESS(HealthWarningSceneStart_Start, void, Il2CppObject *self) {

	if (enableNoticeBoard) {
		WebRequest();
	}
	HealthWarningSceneStart_Start(self);
}



MAKE_HOOK_OFFSETLESS(HandleBeatmapObject, void, Il2CppObject* self, Il2CppObject* beatmapEventData) {
	if (!enableChromaLite) {
		HandleBeatmapObject(self, beatmapEventData);
		return;
	}
	int value = CRASH_UNLESS(il2cpp_utils::GetFieldValue<int>(beatmapEventData, "value")); //Chroma works of the beatmaps value, so we get this value first
	if (value > 1999999999) { //Chroma Events only occur with values of 2000000000 and above 
		//log(DEBUG, "Chroma Event!");
		value = value - 2000000000;
		float red = getValue(value, 16); //gets the Red Value, the first byte of the number 'value' by shifting the bits 16 across
		float green = getValue(value, 8); //gets the Green Value, the second byte of the number 'value'
		float blue = getValue(value, 0); //gets the Blue Value, the last byte of the number 'value' 
		float alpha = 1.0; //Opaque
		//log(DEBUG, "Red :%f", red);
		//log(DEBUG, "Green :%f", green);
		//log(DEBUG, "Blue :%f", blue);
		Color newColor = { red, green, blue, alpha }; //create a new color with values created from the beatmaps value
		Il2CppObject* newColorSO = CRASH_UNLESS(il2cpp_utils::New(il2cpp_utils::GetClassFromName("", "SimpleColorSO")));
		CRASH_UNLESS(il2cpp_utils::RunMethod(newColorSO, "SetColor", newColor));
		CRASH_UNLESS(il2cpp_utils::SetFieldValue(self, "_lightColor0", newColorSO));
		CRASH_UNLESS(il2cpp_utils::SetFieldValue(self, "_lightColor1", newColorSO));
		CRASH_UNLESS(il2cpp_utils::SetFieldValue(self, "_highlightColor0", newColorSO));
		CRASH_UNLESS(il2cpp_utils::SetFieldValue(self, "_highlightColor1", newColorSO));
	}
	HandleBeatmapObject(self, beatmapEventData);
	
	

	//Run the games code
}

bool isCustomSong(std::string_view songHash)
{
	if (songHash.rfind("custom_level_", 0) == 0) {
		getLogger().info("%s is custom song!", songHash.data());
		return true;
	}
	else if (songHash.find_first_of("_") == 45) {
		getLogger().info("Failsafe _ check, %s is custom song!", songHash.data());
		return true;
	}
	return false;
}

std::string GetInfoJson(std::string songID)
{
	std::string songPath = "/sdcard/BMBFData/CustomSongs/" + songID + "/info.dat";
	std::string myText;
	std::ifstream SongFile(songPath);
	std::string finalstr = "";
	while (getline(SongFile, myText)) {
		finalstr += myText;
	}
	if (finalstr == "") {
		finalstr = "invalid";
	}
	SongFile.close();
	return finalstr;
}

void getPastCharacteristicFromFile() {
	std::string PlayerDataPath = "/sdcard/Android/data/com.beatgames.beatsaber/files/PlayerData.dat";
	std::string myText;
	std::ifstream PlayerDataFile(PlayerDataPath);
	std::string finalstr = "";
	while (getline(PlayerDataFile, myText)) {
		finalstr += myText;
	}
	if (finalstr == "") {
		lastPhysicallySelectedCharacteristic = "Standard";
	}
	else {
		d.Parse(finalstr.c_str());
		lastPhysicallySelectedCharacteristic = d["localPlayers"][0]["lastSelectedBeatmapCharacteristicName"].GetString();
	}
	PlayerDataFile.close();
}


void resetDiffNames() {
	Easy = "";
	Normal = "";
	Hard = "";
	Expert = "";
	ExpertPlus = "";
	std::string InvalidJsonData = "{}";
	d.Parse(InvalidJsonData.c_str());
}

void Setdiffs(std::string CharacteristicName)
{

	if (!currentlySelectedIsCustomSong || !enableCustomDiffNames) {
		resetDiffNames();
		return;
	}
	bool setEasy = false;
	bool setNormal = false;
	bool setHard = false;
	bool setExpert = false;
	bool setExpertPlus = false;
	bool characteristicfound = false;
	auto firstarr = d["_difficultyBeatmapSets"].GetArray();
	for (auto i = firstarr.Begin(); i != firstarr.End(); ++i) {
		auto beatmapCharacteristicNameItr = i->GetObject().FindMember("_beatmapCharacteristicName");
		std::string beatmapCharacteristicName = beatmapCharacteristicNameItr->value.GetString();
		if (CharacteristicName == beatmapCharacteristicName) {
			characteristicfound = true;
			auto difficultyBeatmaps = i->GetObject().FindMember("_difficultyBeatmaps");
			auto arr = difficultyBeatmaps->value.GetArray();
			for (auto itr = arr.Begin(); itr != arr.End(); ++itr) {
				std::string diffString = itr->GetObject()["_difficulty"].GetString();
				auto customData = itr->GetObject().FindMember("_customData");
				if (customData != itr->GetObject().MemberEnd()) {
					auto customDiffItr = customData->value.FindMember("_difficultyLabel");
					if (customDiffItr != customData->value.MemberEnd()) {
						std::string difflabelstring = customDiffItr->value.GetString();
						if (diffString == "Easy") {
							Easy = difflabelstring;
							setEasy = true;
						}
						else if (diffString == "Normal") {
							Normal = difflabelstring;
							setNormal = true;
						}
						else if (diffString == "Hard") {
							Hard = difflabelstring;
							setHard = true;
						}
						else if (diffString == "Expert") {
							Expert = difflabelstring;
							setExpert = true;
						}
						else if (diffString == "ExpertPlus") {
							ExpertPlus = difflabelstring;
							setExpertPlus = true;
						}
					}
				}
			}
			break;
		}
	}
	if (!characteristicfound) {
		std::string fallbackCharacteristic = d["_difficultyBeatmapSets"][0]["_beatmapCharacteristicName"].GetString();
		if (CharacteristicName != fallbackCharacteristic) {
			Setdiffs(fallbackCharacteristic);
		}
		else {
			resetDiffNames();
		}
		return;
	}
	if (!setEasy) { Easy = ""; }
	if (!setNormal) { Normal = ""; }
	if (!setHard) { Hard = ""; }
	if (!setExpert) { Expert = ""; }
	if (!setExpertPlus) { ExpertPlus = ""; }
}

MAKE_HOOK_OFFSETLESS(BeatmapDifficultyMethods_Name, Il2CppString*, int diffint) {
	std::string diffLabel = "";
	if (currentlyPlayingLevel) {
		return BeatmapDifficultyMethods_Name(diffint);
	}
	switch (diffint) {
	case 0:
		diffLabel = Easy;
		break;
	case 1:
		diffLabel = Normal;
		break;
	case 2:
		diffLabel = Hard;
		break;
	case 3:
		diffLabel = Expert;
		break;
	case 4:
		diffLabel = ExpertPlus;
		break;
	default:
		return BeatmapDifficultyMethods_Name(diffint);
		break;
	}
	if (diffLabel != "") {
		return il2cpp_utils::createcsstr(diffLabel);
	}
	else {
		return BeatmapDifficultyMethods_Name(diffint);
	}
}


MAKE_HOOK_OFFSETLESS(BeatmapLevelsModel, void, Il2CppObject *self) {
	if (enableCustomLevelsTab) { //if were enabling custom levels
		 //get the amount of ost & extras packs

		auto OstPackList = getConfig().config["OstPackList"].GetArray();
		int ostarraysize = OstPackList.Size();
		Il2CppObject* BeatmapLevelPackCollection = CRASH_UNLESS(il2cpp_utils::GetFieldValue(self, "_ostAndExtrasPackCollection")); //grab the current ost and extras
		Array<Il2CppObject*>* ostarray = CRASH_UNLESS(il2cpp_utils::GetPropertyValue<Array<Il2CppObject*>*>(BeatmapLevelPackCollection, "beatmapLevelPacks")); //store the current OST pack array
		auto customSongArray = reinterpret_cast<Array<Il2CppObject*>*>(il2cpp_functions::array_new(il2cpp_utils::GetClassFromName("", "IBeatmapLevelPack"), ostarray->Length() - ostarraysize)); //create a new array to house the Custom Song packs
		int customsongcounti = 0;
		for (int i = 0; i < ostarray->Length(); i++) {//for all packs in the OST and Extras Pack
			Il2CppString* packName = CRASH_UNLESS(il2cpp_utils::RunMethod<Il2CppString*>(ostarray->values[i], "get_packID")); 
			std::string packNameString = to_utf8(csstrtostr(packName)); 
			bool inList = false;
			for (int j = 0; j < ostarraysize; j++) {
				if (packNameString == OstPackList[j].GetString()) { //check if its in the list
					inList = true;
					break;
				}
			}
			if (!inList) { //if its not in the list
				customSongArray->values[customsongcounti] = ostarray->values[i]; //add it into our own custom levels pack collection
				customsongcounti++;
			}
		}
		auto* pack = CRASH_UNLESS(il2cpp_utils::New("", "BeatmapLevelPackCollection", (Il2CppArray*)customSongArray));
		CRASH_UNLESS(il2cpp_utils::SetFieldValue(self, "_customLevelPackCollection", pack));
	}
	BeatmapLevelsModel(self);
}

MAKE_HOOK_OFFSETLESS(MainMenuViewController, void, Il2CppObject *self, int menuButton) {
	resetDiffNames(); //100% Makes sure everything resets
	MainMenuViewController(self, menuButton);
}


MAKE_HOOK_OFFSETLESS(MainSettingsModelSO, void, Il2CppObject* self, bool  forced) {
	Il2CppObject* burnMark = CRASH_UNLESS(il2cpp_utils::GetFieldValue(self, "burnMarkTrailsEnabled"));
	CRASH_UNLESS(il2cpp_utils::RunMethod(burnMark, "set_value", burnMarkTrailsEnabled));
	MainSettingsModelSO(self, forced);
}


MAKE_HOOK_OFFSETLESS(LevelFilteringNavigationController, void, Il2CppObject *self, bool hideIfOneOrNoPacks, bool enableCustomLevels, Il2CppObject *annotatedBeatmapLevelCollectionToBeSelectedAfterPresent) {
	LevelFilteringNavigationController(self, hideIfOneOrNoPacks, enableCustomLevelsTab, annotatedBeatmapLevelCollectionToBeSelectedAfterPresent);
	if (annotatedBeatmapLevelCollectionToBeSelectedAfterPresent == nullptr && enableCustomLevelsTab && alwaysOpenToCustomLevels) {
		Il2CppObject* tabBarViewController = CRASH_UNLESS(il2cpp_utils::GetFieldValue(self, "_tabBarViewController"));
		CRASH_UNLESS(il2cpp_utils::RunMethod(tabBarViewController, "SelectItem", 3)); //select the 3rd segment (Custom Levels) visually
		Il2CppObject* segmentedControll = CRASH_UNLESS(il2cpp_utils::GetFieldValue(tabBarViewController, "_segmentedControll"));
		CRASH_UNLESS(il2cpp_utils::RunMethod(tabBarViewController, "HandleDidSelectCell", segmentedControll, 3)); //then actually select it

	}
}


MAKE_HOOK_OFFSETLESS(HandleBeatmapCharacteristic, void, Il2CppObject* self, Il2CppObject* controller, Il2CppObject* beatmapCharacteristic) {
	if (beatmapCharacteristic != nullptr) {
		Il2CppString * serializedName = CRASH_UNLESS(il2cpp_utils::RunMethod<Il2CppString*>(beatmapCharacteristic, "get_serializedName"));
		std::string serializedNameString = to_utf8(csstrtostr(serializedName));
		lastPhysicallySelectedCharacteristic = serializedNameString;
		if (currentlySelectedIsCustomSong) {
			Setdiffs(lastPhysicallySelectedCharacteristic);
		}
	}
	HandleBeatmapCharacteristic(self, controller, beatmapCharacteristic);
}


MAKE_HOOK_OFFSETLESS(LevelSelectionNavigationController, void, Il2CppObject* self, Il2CppObject* viewController, Il2CppObject* level) {
	//log(DEBUG, "Hooked onto Standard View");

	Il2CppString *levelID = CRASH_UNLESS(il2cpp_utils::RunMethod<Il2CppString*>(level, "get_levelID"));
	std::string LevelData = to_utf8(csstrtostr(levelID));
	if (isCustomSong(LevelData)) {
		currentlySelectedIsCustomSong = true;
		std::string JsonData = GetInfoJson(LevelData).c_str();
		if (JsonData == "invalid") {
			resetDiffNames();
		}
		else {
			if (lastPhysicallySelectedCharacteristic == "") {
				getPastCharacteristicFromFile();
			}
			d.Parse(JsonData.c_str());
			Setdiffs(lastPhysicallySelectedCharacteristic);
		}
	}
	else {
		currentlySelectedIsCustomSong = false;
		resetDiffNames();
	}		
	LevelSelectionNavigationController(self, viewController, level);
}



MAKE_HOOK_OFFSETLESS(LevelFilteringNavigationController_UpdateCustomSongs, void, Il2CppObject *self) {
	if (enableCustomLevelsTab) {
		Il2CppObject* beatmapLevelsModel = CRASH_UNLESS(il2cpp_utils::GetFieldValue(self, "_beatmapLevelsModel"));
		Il2CppObject* customLevelPackCollection = CRASH_UNLESS(il2cpp_utils::GetFieldValue(beatmapLevelsModel, "_customLevelPackCollection"));
		Il2CppObject* beatmapLevelPacksArray = CRASH_UNLESS(il2cpp_utils::GetPropertyValue(customLevelPackCollection, "beatmapLevelPacks")); //store the current OST pack array
		Il2CppObject* customLevelsTabBarData = CRASH_UNLESS(il2cpp_utils::GetFieldValue(self, "_customLevelsTabBarData"));
		CRASH_UNLESS(il2cpp_utils::SetFieldValue(customLevelsTabBarData, "annotatedBeatmapLevelCollections", beatmapLevelPacksArray));
		CRASH_UNLESS(il2cpp_utils::RunMethod(self, "UpdatePlaylistsData"));
		CRASH_UNLESS(il2cpp_utils::RunMethod(self, "TabBarDidSwitch"));
	}
	else {
		LevelFilteringNavigationController_UpdateCustomSongs(self);
	}
}


MAKE_HOOK_OFFSETLESS(StandardLevelScenesTransitionSetupDataSO, void, Il2CppObject* self, Il2CppObject* difficultyBeatmap, Il2CppObject* overrideEnvironmentSettings, Il2CppObject* overrideColorScheme, Il2CppObject* gameplayModifiers, Il2CppObject* playerSpecificSettings, Il2CppObject* practiceSettings, Il2CppObject* backButtonText, bool useTestNoteCutSoundEffects) {
	currentlyPlayingLevel = true;
	if (!enableCustomSongColours) {
		StandardLevelScenesTransitionSetupDataSO(self, difficultyBeatmap, overrideEnvironmentSettings, overrideColorScheme, gameplayModifiers, playerSpecificSettings, practiceSettings, backButtonText, useTestNoteCutSoundEffects);
		return;
	}

	if (overrideColorScheme == nullptr) {
		if (d.HasMember("_difficultyBeatmapSets") && currentlySelectedIsCustomSong) {
			Il2CppObject * DifficultyBeatmapSet = CRASH_UNLESS(il2cpp_utils::RunMethod(difficultyBeatmap, "get_parentDifficultyBeatmapSet"));
			Il2CppObject * beatmapCharacteristic = CRASH_UNLESS(il2cpp_utils::RunMethod(DifficultyBeatmapSet, "get_beatmapCharacteristic"));
			Il2CppString * serializedName = CRASH_UNLESS(il2cpp_utils::RunMethod<Il2CppString*>(beatmapCharacteristic, "get_serializedName"));
			std::string serializedNameString = to_utf8(csstrtostr(serializedName));

			int diff = CRASH_UNLESS(il2cpp_utils::RunMethod<int>(difficultyBeatmap, "get_difficulty"));

			std::string diffToSearchForString = "";


			switch (diff) {
			case 0:
				diffToSearchForString = "Easy";
				break;
			case 1:
				diffToSearchForString = "Normal";
				break;
			case 2:
				diffToSearchForString = "Hard";
				break;
			case 3:
				diffToSearchForString = "Expert";
				break;
			case 4:
				diffToSearchForString = "ExpertPlus";
				break;
			default:
				diffToSearchForString = "Unknown";
				break;
			}

			Color saberAColor = { 1,0.18823529411,0.18823529411, 1.0 };
			Color saberBColor = { 0.18823529411, 0.59607843137, 1.0, 1.0 };
			Color environmentColor0 = { 0.7529411911964417,0.1882352977991104,0.1882352977991104, 1.0 };
			Color environmentColor1 = { 0.1882352977991104,0.5960784554481506,1.0, 1.0 };
			Color obstaclesColor = { 1.0,0.1882353127002716,0.1882353127002716,1.0 }; //The First Colour Scheme

			Color environmentleftBoost = { 0.0,0.0,0.0, 0.0 };
			Color environmentrightBoost = { 0.0,0.0,0.0, 0.0 };

			bool colorLeftFound = false;
			bool colorRightFound = false;
			bool envColorLeftFound = false;
			bool envColorRightFound = false;
			bool obstacleColorFound = false;
			bool envBoostLeftFound = false;
			bool envBoostRightFound = false;
			auto firstarr = d["_difficultyBeatmapSets"].GetArray();
			for (auto i = firstarr.Begin(); i != firstarr.End(); ++i) {
				auto beatmapCharacteristicNameItr = i->GetObject().FindMember("_beatmapCharacteristicName");
				std::string beatmapCharacteristicName = beatmapCharacteristicNameItr->value.GetString();
				if (serializedNameString == beatmapCharacteristicName) {
					auto difficultyBeatmaps = i->GetObject().FindMember("_difficultyBeatmaps");
					auto arr = difficultyBeatmaps->value.GetArray();
					for (auto itr = arr.Begin(); itr != arr.End(); ++itr) {
						std::string diffString = itr->GetObject()["_difficulty"].GetString();
						if (diffToSearchForString == diffString) {
							auto customData = itr->GetObject().FindMember("_customData");

							if (customData != itr->GetObject().MemberEnd()) {
								if (customData->value.HasMember("_colorLeft")) {
									colorLeftFound = true;
									auto colorLeft = customData->value.FindMember("_colorLeft");
									saberAColor = { colorLeft->value["r"].GetFloat(),colorLeft->value["g"].GetFloat(),colorLeft->value["b"].GetFloat(), 1.0 };
								}
								if (customData->value.HasMember("_colorRight")) {
									colorRightFound = true;
									auto colorRight = customData->value.FindMember("_colorRight");
									saberBColor = { colorRight->value["r"].GetFloat(),colorRight->value["g"].GetFloat(),colorRight->value["b"].GetFloat(), 1.0 };
								}
								if (customData->value.HasMember("_envColorLeft")) {
									envColorLeftFound = true;
									auto envColorLeft = customData->value.FindMember("_envColorLeft");
									environmentColor0 = { envColorLeft->value["r"].GetFloat(),envColorLeft->value["g"].GetFloat(),envColorLeft->value["b"].GetFloat(), 1.0 };
								}
								if (customData->value.HasMember("_envColorRight")) {
									envColorRightFound = true;
									auto envColorRight = customData->value.FindMember("_envColorRight");
									environmentColor1 = { envColorRight->value["r"].GetFloat(),envColorRight->value["g"].GetFloat(),envColorRight->value["b"].GetFloat(), 1.0 };
								}
								if (customData->value.HasMember("_envColorRightBoost")) {
									auto envColorRightBoost = customData->value.FindMember("_envColorRightBoost");
									environmentrightBoost = { envColorRightBoost->value["r"].GetFloat(),envColorRightBoost->value["g"].GetFloat(),envColorRightBoost->value["b"].GetFloat(), 1.0 };
									envBoostRightFound = true;
								}
								if (customData->value.HasMember("_envColorLeftBoost")) {
									auto envColorLeftBoost = customData->value.FindMember("_envColorLeftBoost");
									environmentleftBoost = { envColorLeftBoost->value["r"].GetFloat(),envColorLeftBoost->value["g"].GetFloat(),envColorLeftBoost->value["b"].GetFloat(), 1.0 };
									envBoostLeftFound = true;
								}
								if (customData->value.HasMember("_obstacleColor")) {
									obstacleColorFound = true;
									auto obstacleColor = customData->value.FindMember("_obstacleColor");
									obstaclesColor = { obstacleColor->value["r"].GetFloat(),obstacleColor->value["g"].GetFloat(),obstacleColor->value["b"].GetFloat(), 1.0 };
								}
							}
							break;
						}
					}
					break;
				}
			}
			if ((!colorLeftFound) && (!colorRightFound) && (!envColorLeftFound) && (!envColorRightFound) && (!obstacleColorFound) && (!envBoostLeftFound) && (!envBoostRightFound)) {
				StandardLevelScenesTransitionSetupDataSO(self, difficultyBeatmap, overrideEnvironmentSettings, overrideColorScheme, gameplayModifiers, playerSpecificSettings, practiceSettings, backButtonText, useTestNoteCutSoundEffects);
			}
			else {
				if (!envColorLeftFound) {
					environmentColor0 = saberAColor;
				}
				if (!envColorRightFound) {
					environmentColor1 = saberBColor;
				}

				if (!envBoostLeftFound) {
					environmentleftBoost = environmentColor0;
				}
				if (!envBoostRightFound) {
					environmentrightBoost = environmentColor1;
				}
				
				Il2CppString * colorSchemeId = il2cpp_utils::createcsstr("PinkUtilsCustomColours");
				Il2CppString * colorSchemeName = il2cpp_utils::createcsstr("Custom Song Colours");
				bool isEditable = false;
				Il2CppObject* newColorScheme = CRASH_UNLESS(il2cpp_utils::New(il2cpp_utils::GetClassFromName("", "ColorScheme"), colorSchemeId, colorSchemeName, isEditable, saberAColor, saberBColor, environmentColor0, environmentColor1, environmentleftBoost, environmentrightBoost, obstaclesColor));
				StandardLevelScenesTransitionSetupDataSO(self, difficultyBeatmap, overrideEnvironmentSettings, newColorScheme, gameplayModifiers, playerSpecificSettings, practiceSettings, backButtonText, useTestNoteCutSoundEffects);
			}

		}
		else {
			StandardLevelScenesTransitionSetupDataSO(self, difficultyBeatmap, overrideEnvironmentSettings, overrideColorScheme, gameplayModifiers, playerSpecificSettings, practiceSettings, backButtonText, useTestNoteCutSoundEffects);
		}
	}
	else {
		StandardLevelScenesTransitionSetupDataSO(self, difficultyBeatmap, overrideEnvironmentSettings, overrideColorScheme, gameplayModifiers, playerSpecificSettings, practiceSettings, backButtonText, useTestNoteCutSoundEffects);
	}

	//  

}
MAKE_HOOK_OFFSETLESS(StandardLevelScenesTransitionSetupDataSO_Finish, void, Il2CppObject* self, Il2CppObject* levelCompletionResults) {
	StandardLevelScenesTransitionSetupDataSO_Finish(self, levelCompletionResults);
	currentlyPlayingLevel = false;
}

static rapidjson::Document tablelistjson;


MAKE_HOOK_OFFSETLESS(LevelListTableCell_SetDataFromLevelAsync, void, Il2CppObject* self, Il2CppObject*  mainlevel, bool isFavorite) {
	LevelListTableCell_SetDataFromLevelAsync(self, mainlevel, isFavorite);
	Il2CppString *levelID = CRASH_UNLESS(il2cpp_utils::RunMethod<Il2CppString*>(mainlevel, "get_levelID"));
	std::string levelidstring = to_utf8(csstrtostr(levelID));


	if ((!enableExtraSongDetails) && (!enableCharacteristicsColours) && (!enableRequirementDetails)) {
		return;
	}

	if (isCustomSong(levelidstring)) {
		bool hasCustomData = false;
		bool hasRequirementData = false;
		std::string extraData = "";
		//std::transform(songHash.begin(), songHash.end(), songHash.begin(), ::toupper);
		Color saberAColor = { 1.0,1.0,1.0, 0.7 };
		bool colorFound = false;
		std::string infojson = GetInfoJson(levelidstring);
		if (infojson != "invalid") {
			tablelistjson.Parse(infojson.c_str());
			auto firstarr = tablelistjson["_difficultyBeatmapSets"].GetArray();
			for (auto i = firstarr.Begin(); i != firstarr.End(); ++i) {
				auto difficultyBeatmaps = i->GetObject().FindMember("_difficultyBeatmaps");
				auto arr = difficultyBeatmaps->value.GetArray();
				for (auto itr = arr.Begin(); itr != arr.End(); ++itr) {
					std::string diffString = itr->GetObject()["_difficulty"].GetString();
					auto customData = itr->GetObject().FindMember("_customData");
					if (customData != itr->GetObject().MemberEnd()) {

						if ((!colorFound) && enableCharacteristicsColours) {
							if (customData->value.HasMember("_colorLeft")) {
								colorFound = true;
								auto colorLeft = customData->value.FindMember("_colorLeft");
								saberAColor = { colorLeft->value["r"].GetFloat(),colorLeft->value["g"].GetFloat(),colorLeft->value["b"].GetFloat(), 1.0 };
							}
							else if (customData->value.HasMember("_colorRight")) {
								colorFound = true;
								auto colorRight = customData->value.FindMember("_colorRight");
								saberAColor = { colorRight->value["r"].GetFloat(),colorRight->value["g"].GetFloat(),colorRight->value["b"].GetFloat(), 1.0 };
							}
							else if (customData->value.HasMember("_envColorLeft")) {
								colorFound = true;
								auto envColorLeft = customData->value.FindMember("_envColorLeft");
								saberAColor = { envColorLeft->value["r"].GetFloat(),envColorLeft->value["g"].GetFloat(),envColorLeft->value["b"].GetFloat(), 1.0 };
							}
							else if (customData->value.HasMember("_envColorRightBoost")) {
								colorFound = true;
								auto envColorRight = customData->value.FindMember("_envColorRightBoost");
								saberAColor = { envColorRight->value["r"].GetFloat(),envColorRight->value["g"].GetFloat(),envColorRight->value["b"].GetFloat(), 1.0 };
							}
							else if (customData->value.HasMember("_envColorLeftBoost")) {
								colorFound = true;
								auto envColorLeft = customData->value.FindMember("_envColorLeftBoost");
								saberAColor = { envColorLeft->value["r"].GetFloat(),envColorLeft->value["g"].GetFloat(),envColorLeft->value["b"].GetFloat(), 1.0 };
							}
							else if (customData->value.HasMember("_envColorRight")) {
								colorFound = true;
								auto envColorRight = customData->value.FindMember("_envColorRight");
								saberAColor = { envColorRight->value["r"].GetFloat(),envColorRight->value["g"].GetFloat(),envColorRight->value["b"].GetFloat(), 1.0 };
							}
							else if (customData->value.HasMember("_obstacleColor")) {
								colorFound = true;
								auto obstacleColor = customData->value.FindMember("_obstacleColor");
								saberAColor = { obstacleColor->value["r"].GetFloat(),obstacleColor->value["g"].GetFloat(),obstacleColor->value["b"].GetFloat(), 1.0 };
							}
						}
						hasCustomData = true;
						if (enableRequirementDetails) {
							auto requirements = customData->value.FindMember("_requirements");
							if (requirements != itr->GetObject().MemberEnd()) {
								auto requirementsarray = requirements->value.GetArray();
								if (requirementsarray.Size() > 0 && customData->value.HasMember("_requirements")) {
									hasRequirementData = true;
									for (int it = 0; it != requirementsarray.Size(); it++) {
										std::string ReqShortText = "";
										std::string currentReqInFile = requirementsarray[it].GetString();

										if (currentReqInFile == "Mapping Extensions") {
											ReqShortText = "<color=#e0bd2f>ME</color>";
										}
										else if (currentReqInFile == "Noodle Extensions") {
											ReqShortText = "<color=#e0bd2f>NE</color>";
										}
										else if (currentReqInFile == "Chroma") {
											ReqShortText = "<color=#e0bd2f>Chroma</color>";
										}
										else {
											ReqShortText = currentReqInFile;
										}

										if (extraData != "") {
											extraData = extraData + ", " + ReqShortText;
										}
										else {
											extraData = "Requires: " + ReqShortText;
										}

									}
								}
							}
							else {
								hasRequirementData = false;
							}
						}
						else {
							hasRequirementData = false;
						}
					}
					if (hasCustomData && (hasRequirementData || (!enableRequirementDetails))) {
						break;
					}
				}
				if (hasCustomData && (hasRequirementData || (!enableRequirementDetails))) {
					break;
				}
			}

			if (enableExtraSongDetails) {
				Il2CppString * overallsongNameString;
				Il2CppString * creatorNameString;

				Il2CppObject* authortext = CRASH_UNLESS(il2cpp_utils::GetFieldValue(self, "_authorText"));
				Il2CppObject* songNameText = CRASH_UNLESS(il2cpp_utils::GetFieldValue(self, "_songNameText"));

				Il2CppString * authorName = CRASH_UNLESS(il2cpp_utils::RunMethod<Il2CppString*>(mainlevel, "get_songAuthorName"));
				Il2CppString * levelAuthorName = CRASH_UNLESS(il2cpp_utils::RunMethod<Il2CppString*>(mainlevel, "get_levelAuthorName"));
				Il2CppString * songSubName = CRASH_UNLESS(il2cpp_utils::RunMethod<Il2CppString*>(mainlevel, "get_songSubName"));
				Il2CppString * songName = CRASH_UNLESS(il2cpp_utils::RunMethod<Il2CppString*>(mainlevel, "get_songName"));

				std::string authorNameString = to_utf8(csstrtostr(authorName));
				std::string levelCreatorName = to_utf8(csstrtostr(levelAuthorName));
				std::string songSubNameString = to_utf8(csstrtostr(songSubName));
				std::string songNameString = to_utf8(csstrtostr(songName));


				if (hasRequirementData) {
					overallsongNameString = il2cpp_utils::createcsstr(songNameString + " <size=80%>" + songSubNameString + "</size>");
					creatorNameString = il2cpp_utils::createcsstr(authorNameString + " <size=80%>[" + levelCreatorName + "]</size>" + " <size=75%>[" + extraData + "]</size>");
				}
				else {
					overallsongNameString = il2cpp_utils::createcsstr(songNameString + " <size=80%>" + songSubNameString + "</size>");
					creatorNameString = il2cpp_utils::createcsstr(authorNameString + " <size=80%>[" + levelCreatorName + "]</size>");
				}
				CRASH_UNLESS(il2cpp_utils::RunMethod(authortext, "set_text", creatorNameString));
				CRASH_UNLESS(il2cpp_utils::RunMethod(songNameText, "set_text", overallsongNameString));
			}
			else if (hasRequirementData) {
				Il2CppObject* authortext = CRASH_UNLESS(il2cpp_utils::GetFieldValue(self, "_authorText"));
				Il2CppString * authorName = CRASH_UNLESS(il2cpp_utils::RunMethod<Il2CppString*>(mainlevel, "get_songAuthorName"));
				std::string authorNameString = to_utf8(csstrtostr(authorName));
				Il2CppString * creatorNameString = il2cpp_utils::createcsstr(authorNameString + " <size=75%>[" + extraData + "]</size>");
				CRASH_UNLESS(il2cpp_utils::RunMethod(authortext, "set_text", creatorNameString));
			}
		}
		else {
			if (enableExtraSongDetails) {
				Il2CppObject* authortext = CRASH_UNLESS(il2cpp_utils::GetFieldValue(self, "_authorText"));
				Il2CppString * authorName = CRASH_UNLESS(il2cpp_utils::RunMethod<Il2CppString*>(mainlevel, "get_songAuthorName"));
				std::string authorNameString = to_utf8(csstrtostr(authorName));
				Il2CppString * creatorNameString = il2cpp_utils::createcsstr(authorNameString + " <size=80%><color=#FF0300FF>Invalid Song Path</color></size>");
				CRASH_UNLESS(il2cpp_utils::RunMethod(authortext, "set_text", creatorNameString));
			}
		}
		if (enableCharacteristicsColours) {
			CRASH_UNLESS(il2cpp_utils::SetFieldValue(self, "_beatmapCharacteristicImagesNormalColor", saberAColor));

		}
		//Color newColour = { 1.0,0.2705,0.703,1.0 };
		//CRASH_UNLESS(il2cpp_utils::SetFieldValue(self, "_selectedHighlightElementsColor", newColour));
	}
	else {
		if (enableCharacteristicsColours) {
			Color saberAColor = { 1.0,1.0,1.0, 0.7 };
			CRASH_UNLESS(il2cpp_utils::SetFieldValue(self, "_beatmapCharacteristicImagesNormalColor", saberAColor));
		}
	}
}



MAKE_HOOK_OFFSETLESS(AnnotatedBeatmapLevelCollectionsViewController, void, Il2CppObject* self, Array<Il2CppObject*>* annotatedBeatmapLevelCollections, int selectedItemIndex, bool hideIfOneOrNoPacks) {

	//this entire section is done due to us not being able to set the OST pack array ourselves, so I intercept the SetData of the array as its getting displayed
	//and removing the custom song packs

	//THIS SHOULD BE CHANGED IN FUTURE, ITS A MASSIVE BODGE AND IS VERY POORLY DONE, IF YOU ARE UPDATING THIS MOD, CHANGE THIS!

	if (!enableCustomLevelsTab) {
		AnnotatedBeatmapLevelCollectionsViewController(self, annotatedBeatmapLevelCollections, selectedItemIndex, hideIfOneOrNoPacks);
		return;
	}
	auto OstPackList = getConfig().config["OstPackList"].GetArray();
	int ostarraysize = OstPackList.Size();

	 //get the amount of songs in the OST playlist tab

	if (!il2cpp_utils::RunMethod<Il2CppString*>(annotatedBeatmapLevelCollections->values[0], "get_packID")) {
		AnnotatedBeatmapLevelCollectionsViewController(self, annotatedBeatmapLevelCollections, selectedItemIndex, hideIfOneOrNoPacks);
		return;
		//the Playlists Tab is different, and doesnt have a PackID which will cause the mod to crash
	}

	bool isTheOst = false;

	for (int i = 0; i < annotatedBeatmapLevelCollections->Length(); i++) { //for every item in the currently selected tab list
		Il2CppString* collectionName = CRASH_UNLESS(il2cpp_utils::RunMethod<Il2CppString*>(annotatedBeatmapLevelCollections->values[i], "get_packID"));
		std::string collectionNameString = to_utf8(csstrtostr(collectionName)); //get the collection name
		//getLogger().info(collectionNameString);
		if (collectionNameString == OstPackList[0].GetString()) { //and check if it has OST Vol 1
			isTheOst = true; //If it does, then its the OST pack as PinkUtils removes custom packs from the main OST pack earlier
			break;
		}
	}
	if (isTheOst) {
		//if the current selected tab is the OST tab
		auto ostSongArray = reinterpret_cast<Array<Il2CppObject*>*>(il2cpp_functions::array_new(il2cpp_utils::GetClassFromName("", "IBeatmapLevelPackCollection"), ostarraysize)); //create a new OST song array
		int ostsongcounti = 0; //int for looping later
		for (int i = 0; i < annotatedBeatmapLevelCollections->Length(); i++) {
			Il2CppString* collectionName = CRASH_UNLESS(il2cpp_utils::RunMethod<Il2CppString*>(annotatedBeatmapLevelCollections->values[i], "get_packID"));
			std::string collectionNameString = to_utf8(csstrtostr(collectionName));
			bool inList = false;
			for (int j = 0; j < ostarraysize; j++) {
				if (collectionNameString == OstPackList[j].GetString()) {
					
					inList = true;
					break; //If the currently selected path is a part of the OST pack name array, then we set inList to true to add it to the new array later
				}
			}
			if (inList) {
				ostSongArray->values[ostsongcounti] = annotatedBeatmapLevelCollections->values[i]; //add the current pack if it is an OST pack to the new pack we created
				ostsongcounti++; //adds 1 to this variable so we can add up seperately, so it doesnt matter on the collections order
			}
		}
		//there is then a bug which occurs if the packs are in a wacky order, where clicking between the 'Custom Levels' and 'OST & Extras' tab can make the game
		//still think that custom song packs are still in the OST and Extras tab, meaning they view visually on the level select screen but not in the
		//playlists tab... not a game breaking bug but still enough of an issue to require fixing
		Il2CppString* collectionName = CRASH_UNLESS(il2cpp_utils::RunMethod<Il2CppString*>(annotatedBeatmapLevelCollections->values[selectedItemIndex], "get_packID"));
		std::string collectionNameString = to_utf8(csstrtostr(collectionName)); //gets the collection name of the beat map which is going to be selected after data is set
		bool goingToBeSelectedIsOST = false;
		for (int j = 0; j < ostarraysize; j++) {
			if (collectionNameString == OstPackList[j].GetString()) {
				goingToBeSelectedIsOST = true;
				break; //checks if it is an OST pack
			}
		}
		if (goingToBeSelectedIsOST) { //if it is an OST pack, great run it like normal but add in our own pack collection that we made.
			AnnotatedBeatmapLevelCollectionsViewController(self, ostSongArray, selectedItemIndex, hideIfOneOrNoPacks);
		}
		else { //if its not, then we should auto select to the first pack in our list to fully reset and fix graphical issues
			AnnotatedBeatmapLevelCollectionsViewController(self, ostSongArray, 0, hideIfOneOrNoPacks); //selectedItemIndex is mainly the visual graphics on the first pack within the pack collection list
			Il2CppObject* annotatedBeatmapLevelCollectionsTableView = CRASH_UNLESS(il2cpp_utils::GetFieldValue(self, "_annotatedBeatmapLevelCollectionsTableView"));
			Il2CppObject* tableView = CRASH_UNLESS(il2cpp_utils::GetFieldValue(annotatedBeatmapLevelCollectionsTableView, "_tableView")); //grab the table view
			CRASH_UNLESS(il2cpp_utils::RunMethod(annotatedBeatmapLevelCollectionsTableView, "HandleDidSelectColumnEvent", tableView, 0)); //and force select it to the 0th (1st) item

		}

	}
	else { //if its not the OST tab, then just run the normal code
		AnnotatedBeatmapLevelCollectionsViewController(self, annotatedBeatmapLevelCollections, selectedItemIndex, hideIfOneOrNoPacks);
	}
}


void resetconfig() {

	getConfig().config.RemoveAllMembers();
	getConfig().config.SetObject();
	auto& allocator = getConfig().config.GetAllocator();

	getConfig().config.AddMember("ModVersionDontChange", "1.2.1", allocator);
	getConfig().config.AddMember("enablePCBurnMarks", burnMarkTrailsEnabled, allocator);
	getConfig().config.AddMember("enableCustomLevelsTab", enableCustomLevelsTab, allocator);
	getConfig().config.AddMember("enableCharacteristicsColours", enableCharacteristicsColours, allocator);
	getConfig().config.AddMember("enableRequirementDetails", enableRequirementDetails, allocator);
	getConfig().config.AddMember("enableExtraSongDetails", enableExtraSongDetails, allocator);
	getConfig().config.AddMember("enableCustomSongColours", enableCustomSongColours, allocator);
	getConfig().config.AddMember("alwaysOpenToCustomLevels", alwaysOpenToCustomLevels, allocator);
	getConfig().config.AddMember("enableCustomDiffNames", enableCustomDiffNames, allocator);
	getConfig().config.AddMember("enableChromaLite", enableChromaLite, allocator);
	getConfig().config.AddMember("enableNoticeBoard", enableNoticeBoard, allocator);


	auto arr = rapidjson::Value(rapidjson::kArrayType);
	arr.PushBack("OstVol1", allocator);
	arr.PushBack("OstVol2", allocator);
	arr.PushBack("OstVol3", allocator);
	arr.PushBack("Extras", allocator);
	arr.PushBack("Camellia", allocator);


	getConfig().config.AddMember("OstPackList", arr, allocator);
	getConfig().Write();
}

// This function is called before a mod is constructed for the first time.
// Perform one time setup, as well as specify the mod ID and version here.
extern "C" void setup(ModInfo& info) {
    // This should be the first thing done
    info.id = "PinkUtils";
    info.version = "1.1.0";
	modInfo = info;

    getLogger().info("Completed setup!");
    // We can even check information specific to the modloader!
    getLogger().info("Modloader name: %s", Modloader::getInfo().name.c_str());
    // Load config
    getConfig().Load();
    // Can use getConfig().config to modify the rapidjson document


	




	if (!getConfig().config.HasMember("enableCustomLevelsTab")) {
		resetconfig();
	}
	burnMarkTrailsEnabled = getConfig().config["enablePCBurnMarks"].GetBool();
	enableCustomLevelsTab = getConfig().config["enableCustomLevelsTab"].GetBool();
	enableCharacteristicsColours = getConfig().config["enableCharacteristicsColours"].GetBool();
	enableRequirementDetails = getConfig().config["enableRequirementDetails"].GetBool();
	enableExtraSongDetails = getConfig().config["enableExtraSongDetails"].GetBool();
	enableCustomSongColours = getConfig().config["enableCustomSongColours"].GetBool();
	alwaysOpenToCustomLevels = getConfig().config["alwaysOpenToCustomLevels"].GetBool();
	enableCustomDiffNames = getConfig().config["enableCustomDiffNames"].GetBool();
	enableChromaLite = getConfig().config["enableChromaLite"].GetBool();
	enableNoticeBoard = getConfig().config["enableNoticeBoard"].GetBool();


	if (getConfig().config.HasMember("ModVersionDontChange")) {
		if (getConfig().config["ModVersionDontChange"].GetString() != modVersion) {
			resetconfig();
		}
	}
	else {
		resetconfig();
	}


}

// This function is called when the mod is loaded for the first time, immediately after il2cpp_init.
extern "C" void load() {
    getLogger().debug("Installing PinkUtils!");
	INSTALL_HOOK_OFFSETLESS(StandardLevelScenesTransitionSetupDataSO, il2cpp_utils::FindMethodUnsafe("", "StandardLevelScenesTransitionSetupDataSO", "Init", 8));
	INSTALL_HOOK_OFFSETLESS(StandardLevelScenesTransitionSetupDataSO_Finish, il2cpp_utils::FindMethodUnsafe("", "StandardLevelScenesTransitionSetupDataSO", "Finish", 1));
	INSTALL_HOOK_OFFSETLESS(MainMenuViewController, il2cpp_utils::FindMethodUnsafe("", "MainMenuViewController", "HandleMenuButton", 1));
	INSTALL_HOOK_OFFSETLESS(HealthWarningSceneStart_Start, il2cpp_utils::FindMethodUnsafe("", "HealthWarningSceneStart", "Start", 0));

	INSTALL_HOOK_OFFSETLESS(BeatmapDifficultyMethods_Name, il2cpp_utils::FindMethodUnsafe("", "BeatmapDifficultyMethods", "Name", 1)); //song names
	
	INSTALL_HOOK_OFFSETLESS(LevelSelectionNavigationController, il2cpp_utils::FindMethodUnsafe("", "LevelSelectionNavigationController", "HandleLevelCollectionViewControllerDidSelectLevel", 2));
	INSTALL_HOOK_OFFSETLESS(HandleBeatmapCharacteristic, il2cpp_utils::FindMethodUnsafe("", "StandardLevelDetailView", "HandleBeatmapCharacteristicSegmentedControlControllerDidSelectBeatmapCharacteristic", 2));
	INSTALL_HOOK_OFFSETLESS(LevelListTableCell_SetDataFromLevelAsync, il2cpp_utils::FindMethodUnsafe("", "LevelListTableCell", "SetDataFromLevelAsync", 2));

	INSTALL_HOOK_OFFSETLESS(HandleBeatmapObject, il2cpp_utils::FindMethodUnsafe("", "LightSwitchEventEffect", "HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger", 1)); //Hook onto the lighting events

	INSTALL_HOOK_OFFSETLESS(LevelFilteringNavigationController_UpdateCustomSongs, il2cpp_utils::FindMethodUnsafe("", "LevelFilteringNavigationController", "UpdateCustomSongs", 0));
	INSTALL_HOOK_OFFSETLESS(AnnotatedBeatmapLevelCollectionsViewController, il2cpp_utils::FindMethodUnsafe("", "AnnotatedBeatmapLevelCollectionsViewController", "SetData", 3));

	INSTALL_HOOK_OFFSETLESS(BeatmapLevelsModel, il2cpp_utils::FindMethodUnsafe("", "BeatmapLevelsModel", "Start", 0)); //setting customlevels pack
	INSTALL_HOOK_OFFSETLESS(LevelFilteringNavigationController, il2cpp_utils::FindMethodUnsafe("", "LevelFilteringNavigationController", "Setup", 3)); //custom levels tab
	INSTALL_HOOK_OFFSETLESS(MainSettingsModelSO, il2cpp_utils::FindMethodUnsafe("", "MainSettingsModelSO", "Load", 1)); //burn marks
    
	
	INSTALL_HOOK_OFFSETLESS(ReleaseInfoViewController_TextChange, il2cpp_utils::FindMethodUnsafe("", "ReleaseInfoViewController", "DidActivate", 2));
	INSTALL_HOOK_OFFSETLESS(FlowCoordinator_ProvideInitialViewControllers, il2cpp_utils::FindMethodUnsafe("", "MainFlowCoordinator", "ProvideInitialViewControllers", 5));
	INSTALL_HOOK_OFFSETLESS(FlowCoordinator_TopViewControllerWillChange, il2cpp_utils::FindMethodUnsafe("", "MainFlowCoordinator", "TopViewControllerWillChange", 3));
	getLogger().debug("Installed PinkUtils!");
    getLogger().info("initialized: %s", il2cpp_functions::initialized ? "true" : "false");
}