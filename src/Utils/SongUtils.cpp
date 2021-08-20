#include "Utils/SongUtils.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "GlobalNamespace/CustomPreviewBeatmapLevel.hpp"
#include "GlobalNamespace/ColorScheme.hpp"

#include "CustomTypes/CustomLevelInfoSaveData.hpp"
static std::string toLower(std::string in)
{
	std::string output = "";
	for (auto& c : in)
	{
		output += tolower(c);
	}

	return output;
}

static std::string removeSpaces(std::string input)
{
	std::string output = "";
	for (auto c : input)
	{
		if (c == ' ') continue;
		output += c;
	}
	return output;
}

extern Logger& getLogger();

namespace SongUtils
{
	LoadedInfoEvent onLoadedInfoEvent;
	std::shared_ptr<rapidjson::Document> currentInfoDat;

	bool currentInfoDatValid = false;

	std::string lastPhysicallySelectedCharacteristic = "";
	std::string lastPhysicallySelectedDifficulty = "";
	std::string currentLevelPath = "";
	
	bool currrentlySelectedIsCustom = false;
	bool currrentlySelectedIsWIP = false;

	LoadedInfoEvent& onLoadedInfo()
	{
		return onLoadedInfoEvent;
	}

	std::shared_ptr<rapidjson::Document>& GetCurrentInfoDatPtr()
	{
		return currentInfoDat;
	}

	rapidjson::Document& GetCurrentInfoDat()
	{
		return *currentInfoDat;
	}

	const std::string& GetCurrentSongPath()
	{
		return currentLevelPath;
	}
	
	static inline std::optional<std::shared_ptr<rapidjson::Document>> getOptional(bool value) 
	{ 
		if (value) return currentInfoDat;
		else return std::nullopt; 
	}

	std::string GetDiffFromNumber(GlobalNamespace::BeatmapDifficulty selectedDifficulty)
	{
		switch (selectedDifficulty.value)
		{
			case 0:
				return "Easy";
				break;
			case 1:
				return "Normal";
				break;
			case 2:
				return "Hard";
				break;
			case 3:
				return "Expert";
				break;
			case 4:
				return "ExpertPlus";
				break;
			default:
				return "Unknown";
				break;
		}
	}

	GlobalNamespace::BeatmapDifficulty GetNumberFromDiff(std::string difficulty)
	{
		// another way of doing it with a map
		/*
		static std::map<std::string, GlobalNamespace::BeatmapDifficulty> stringToDiff = {
			{ "Easy", GlobalNamespace::BeatmapDifficulty::Easy },
			{ "Normal", GlobalNamespace::BeatmapDifficulty::Normal },
			{ "Hard", GlobalNamespace::BeatmapDifficulty::Hard },
			{ "Expert", GlobalNamespace::BeatmapDifficulty::Expert },
			{ "ExpertPlus", GlobalNamespace::BeatmapDifficulty::ExpertPlus }
		};

		auto itr = stringToDiff.find(difficulty);
		if (itr != stringToDiff.end()) return itr->second;
		else return GlobalNamespace::BeatmapDifficulty::ExpertPlus;
		return stringToDiff[difficulty];		
		*/
		if (difficulty == "Easy") {
			return GlobalNamespace::BeatmapDifficulty::Easy;
		}
		else if (difficulty == "Normal") {
			return GlobalNamespace::BeatmapDifficulty::Normal;
		}
		else if (difficulty == "Hard") {
			return GlobalNamespace::BeatmapDifficulty::Hard;
		}
		else if (difficulty == "Expert") {
			return GlobalNamespace::BeatmapDifficulty::Expert;
		}
		else if (difficulty == "ExpertPlus") {
			return GlobalNamespace::BeatmapDifficulty::ExpertPlus;
		}
		else {
			return GlobalNamespace::BeatmapDifficulty::ExpertPlus;
		}
	}

	namespace CustomData
	{
		
		bool GetInfoJson(GlobalNamespace::IPreviewBeatmapLevel* level, std::shared_ptr<rapidjson::Document>& doc)
		{
			// if level nullptr or not custom it fails
			if (!level || !SongInfo::isCustom(level)) return false;

			// cast to custom level
			GlobalNamespace::CustomPreviewBeatmapLevel* customLevel = reinterpret_cast<GlobalNamespace::CustomPreviewBeatmapLevel*>(level);

			Il2CppString* levelPathCS = customLevel->get_customLevelPath();
			std::string songPath = to_utf8(csstrtostr(levelPathCS));
			currentLevelPath = songPath;

			auto infoDataOpt = il2cpp_utils::try_cast<CustomJSONData::CustomLevelInfoSaveData>(customLevel->get_standardLevelInfoSaveData());
			// if an info.dat already exists on the given level, don't read the file again
			if (infoDataOpt)
			{
				getLogger().info("Found custom json data on level");
				auto infoData = infoDataOpt.value();
				doc = infoData->doc;
				return true;
			}
			else
			{
				songPath += "/info.dat";
				// get the path
				getLogger().info("Getting info.dat for %s", songPath.c_str());
				// if the file doesnt exist, fail
				if (!fileexists(songPath)) return false;

				getLogger().info("Reading file");
				// read file
				std::string info = readfile(songPath);

				// parse into doc
				doc->Parse(info.c_str());
				// return true if it read the file right, return false if there was a parse error
				return !doc->GetParseError();
			}
		}
		
		/*
		bool GetInfoJson(GlobalNamespace::IPreviewBeatmapLevel* level, rapidjson::Document& doc)
		{
			// if level nullptr or not custom it fails
			if (!level || !SongInfo::isCustom(level)) return false;

			// cast to custom level
			GlobalNamespace::CustomPreviewBeatmapLevel* customLevel = reinterpret_cast<GlobalNamespace::CustomPreviewBeatmapLevel*>(level);

			Il2CppString* levelPathCS = customLevel->get_customLevelPath();
			std::string songPath = to_utf8(csstrtostr(levelPathCS));
			currentLevelPath = songPath;

			auto infoDataOpt = il2cpp_utils::try_cast<CustomJSONData::CustomLevelInfoSaveData>(customLevel->get_standardLevelInfoSaveData());
			// if an info.dat already exists on the given level, don't read the file again
			if (infoDataOpt)
			{
				getLogger().info("Found custom json data on level");
				auto infoData = infoDataOpt.value();
				doc.CopyFrom(*infoData->doc, doc.GetAllocator());
				return true;
			}
			else
			{
				songPath += "/info.dat";
				// get the path
				getLogger().info("Getting info.dat for %s", songPath.c_str());
				// if the file doesnt exist, fail
				if (!fileexists(songPath)) return false;

				getLogger().info("Reading file");
				// read file
				std::string info = readfile(songPath);

				// parse into doc
				doc.Parse(info.c_str());
				// return true if it read the file right, return false if there was a parse error
				return !doc.GetParseError();
			}
		}*/

		bool GetCurrentCustomData(rapidjson::Document& in, rapidjson::Value& out)
		{
			return GetCurrentCustomData(in, out, GetNumberFromDiff(SongUtils::SongInfo::get_lastPhysicallySelectedDifficulty()));
		}

		bool GetCurrentCustomData(rapidjson::Document& in, rapidjson::Value& out, GlobalNamespace::BeatmapDifficulty difficulty)
		{
			bool hasCustomData = false;
			auto& lastPhysicallySelectedCharacteristic = SongUtils::SongInfo::get_lastPhysicallySelectedCharacteristic();
			std::string difficultyToFind = SongUtils::GetDiffFromNumber(difficulty);
			getLogger().info("Looking for characteristic: %s", difficultyToFind.c_str());
			getLogger().info("Looking for diff: %s", difficultyToFind.c_str());

			auto difficultyBeatmapSetsitr = in.FindMember("_difficultyBeatmapSets");
			// if we find the sets iterator
			if (difficultyBeatmapSetsitr != in.MemberEnd())
			{
				auto setArr = difficultyBeatmapSetsitr->value.GetArray();
				for (auto& beatmapCharacteristicItr : setArr)
				{
					std::string beatmapCharacteristicName = beatmapCharacteristicItr.FindMember("_beatmapCharacteristicName")->value.GetString();
					getLogger().info("Found CharacteristicName: %s", beatmapCharacteristicName.c_str());
					// if the last selected beatmap characteristic is this specific one
					if (beatmapCharacteristicName == lastPhysicallySelectedCharacteristic)
					{
						auto difficultyBeatmaps = beatmapCharacteristicItr.GetObject().FindMember("_difficultyBeatmaps");
						auto beatmaps = difficultyBeatmaps->value.GetArray();
						for (auto& beatmap : beatmaps)
						{
							auto beatmapDiffNameItr = beatmap.GetObject().FindMember("_difficulty");
							std::string diffString = beatmapDiffNameItr->value.GetString();
							getLogger().info("Found diffstring: %s", diffString.c_str());
							// if the last selected difficulty is this specific one
							if (difficultyToFind == diffString)
							{
								auto customData = beatmap.GetObject().FindMember("_customData");
								if (customData != beatmap.MemberEnd())
								{
									hasCustomData = true;
									out = customData->value;
								}
							}
						}
					}
				}
			}
			return hasCustomData;
		}

		GlobalNamespace::ColorScheme* getCustomSongColour(GlobalNamespace::ColorScheme* defaultColorScheme, bool hasOverride) {

			auto& d = SongUtils::GetCurrentInfoDat();
			rapidjson::Value customData;
			if (SongUtils::CustomData::GetCurrentCustomData(d, customData)) {

				bool mapHasCustomColours = false;

				Il2CppString* colorSchemeId = il2cpp_utils::createcsstr("PinkCoreMapColorScheme");
				Il2CppString* colorSchemeNameLocalizationKey = il2cpp_utils::createcsstr("PinkCore Map Color Scheme");
				bool useNonLocalizedName = true;
				Il2CppString* nonLocalizedName = colorSchemeNameLocalizationKey;
				bool isEditable = false;
				UnityEngine::Color colorLeft = defaultColorScheme->saberAColor;
				UnityEngine::Color colorRight = defaultColorScheme->saberBColor;
				UnityEngine::Color envColorLeft = defaultColorScheme->environmentColor0;
				UnityEngine::Color envColorRight = defaultColorScheme->environmentColor1;
				bool supportsEnvironmentColorBoost = defaultColorScheme->supportsEnvironmentColorBoost;
				UnityEngine::Color envColorLeftBoost = defaultColorScheme->environmentColor0Boost;
				UnityEngine::Color envColorRightBoost = defaultColorScheme->environmentColor1Boost;
				UnityEngine::Color obstacleColor;

				bool customBoostColours = false;

				auto colorLeftItr = customData.FindMember("_colorLeft");
				if (colorLeftItr != customData.MemberEnd()) {
					colorLeft = { colorLeftItr->value["r"].GetFloat() , colorLeftItr->value["g"].GetFloat() , colorLeftItr->value["b"].GetFloat(), 1.0f };
					mapHasCustomColours = true;
				}

				auto colorRightItr = customData.FindMember("_colorRight");
				if (colorRightItr != customData.MemberEnd()) {
					colorRight = { colorRightItr->value["r"].GetFloat() , colorRightItr->value["g"].GetFloat() , colorRightItr->value["b"].GetFloat(), 1.0f };
					mapHasCustomColours = true;
				}

				auto envColorLeftItr = customData.FindMember("_envColorLeft");
				if (envColorLeftItr != customData.MemberEnd()) {
					envColorLeft = { envColorLeftItr->value["r"].GetFloat() , envColorLeftItr->value["g"].GetFloat() , envColorLeftItr->value["b"].GetFloat(), 1.0f };
					mapHasCustomColours = true;
				}

				auto envColorRightItr = customData.FindMember("_envColorRight");
				if (envColorRightItr != customData.MemberEnd()) {
					envColorRight = { envColorRightItr->value["r"].GetFloat() , envColorRightItr->value["g"].GetFloat() , envColorRightItr->value["b"].GetFloat(), 1.0f };
					mapHasCustomColours = true;
				}

				auto envColorLeftBoostItr = customData.FindMember("_envColorLeftBoost");
				if (envColorLeftBoostItr != customData.MemberEnd()) {
					envColorLeftBoost = { envColorLeftBoostItr->value["r"].GetFloat() , envColorLeftBoostItr->value["g"].GetFloat() , envColorLeftBoostItr->value["b"].GetFloat(), 1.0f };
					mapHasCustomColours = true;
					customBoostColours = true;
				}

				auto envColorRightBoostItr = customData.FindMember("_envColorRightBoost");
				if (envColorRightBoostItr != customData.MemberEnd()) {
					envColorRightBoost = { envColorRightBoostItr->value["r"].GetFloat() , envColorRightBoostItr->value["g"].GetFloat() , envColorRightBoostItr->value["b"].GetFloat(), 1.0f };
					mapHasCustomColours = true;
					customBoostColours = true;
				}

				auto obstacleColorItr = customData.FindMember("_obstacleColor");
				if (obstacleColorItr != customData.MemberEnd()) {
					obstacleColor = { obstacleColorItr->value["r"].GetFloat() , obstacleColorItr->value["g"].GetFloat() , obstacleColorItr->value["b"].GetFloat(), 1.0f };
					mapHasCustomColours = true;
				}

				if (mapHasCustomColours) {
					//if an override is not set then this is null
					if (hasOverride) {
						colorLeft = defaultColorScheme->saberAColor;
						colorRight = defaultColorScheme->saberBColor;
					}

					if (!customBoostColours && !supportsEnvironmentColorBoost) {
						envColorLeftBoost = envColorLeft;
						envColorRightBoost = envColorRight;
					}

					auto newColorScheme = *il2cpp_utils::New<GlobalNamespace::ColorScheme*>(colorSchemeId, colorSchemeNameLocalizationKey, useNonLocalizedName, nonLocalizedName, isEditable, colorLeft, colorRight, envColorLeft, envColorRight, supportsEnvironmentColorBoost, envColorLeftBoost, envColorRightBoost, obstacleColor);

					return newColorScheme;

				}
				else {
					return nullptr;
				}
			}
			else {
				return nullptr;
			}
		}

		void ExtractRequirements(const rapidjson::Value& requirementsArray, std::vector<std::string>& output)
		{
			auto actualArray = requirementsArray.GetArray();
			for (auto& requirement : actualArray)
			{
				std::string requirementName = requirement.GetString();
				std::string requirementNameWithoutSpaces = removeSpaces(requirementName);

				auto it = std::find(output.begin(), output.end(), requirementName);

				if (it == output.end())
					output.push_back(requirementName);
			}

			std::sort(output.begin(), output.end());
		}
	}

	namespace SongInfo
	{
		bool isCustom(GlobalNamespace::IPreviewBeatmapLevel* level)
		{
			if (!level) return false;
			return il2cpp_functions::class_is_assignable_from(classof(GlobalNamespace::CustomPreviewBeatmapLevel*), il2cpp_functions::object_get_class(reinterpret_cast<Il2CppObject*>(level)));
			// the above check should suffice, but this code will remain as backup
			/*
			Il2CppString* levelidCS = level->get_levelID();
			static Il2CppString* custom_level_ = il2cpp_utils::newcsstr<il2cpp_utils::CreationType::Manual>("custom_level_");
			// does level ID contain custom_level_ ?
			if (levelidCS->Contains(custom_level_)) return true;
			// fallback to _ on pos 45
			else if (static_cast<char>(levelidCS->get_Chars(45)) == '_') return true;
			return false;
			*/
		}
		/*-------------------------------------------------*/

		bool get_currentlySelectedIsCustom()
		{
			return currrentlySelectedIsCustom;
		}

		void set_currentlySelectedIsCustom(bool val)
		{
			currrentlySelectedIsCustom = val;
		}
		/*-------------------------------------------------*/

		bool get_currentlySelectedIsWIP()
		{
			return currrentlySelectedIsWIP;
		}

		void set_currentlySelectedIsWIP(bool val)
		{
			currrentlySelectedIsWIP = val;
		}
		/*-------------------------------------------------*/

		bool get_currentInfoDatValid()
		{
			return currentInfoDatValid;
		}

		void set_currentInfoDatValid(bool value)
		{
			currentInfoDatValid = value;
			onLoadedInfoEvent.invoke(getOptional(value));
		}
		/*-------------------------------------------------*/

		const std::string& get_lastPhysicallySelectedCharacteristic()
		{
			return lastPhysicallySelectedCharacteristic;
		}

		void set_lastPhysicallySelectedCharacteristic(std::string value)
		{
			lastPhysicallySelectedCharacteristic = value;
		}
		/*-------------------------------------------------*/

		const std::string& get_lastPhysicallySelectedDifficulty()
		{
			return lastPhysicallySelectedDifficulty;
		}

		void set_lastPhysicallySelectedDifficulty(std::string value)
		{
			lastPhysicallySelectedDifficulty = value;
		}
	}
}