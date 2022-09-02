#include "Utils/RequirementUtils.hpp"
#include "Utils/SongUtils.hpp"
#include "Utils/ContributorUtils.hpp"
#include "beatsaber-hook/shared/utils/typedefs-wrappers.hpp"
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/document.h"
#include "GlobalNamespace/DifficultyBeatmapSet.hpp"
#include "GlobalNamespace/IBeatmapLevel.hpp"
namespace PinkCore::API
{

    void GetNewMapData(rapidjson::GenericDocument<rapidjson::UTF16<char16_t>>& in, PinkCore::API::LevelDetails& levelDetail, GlobalNamespace::IDifficultyBeatmap* difficultyBeatmap){
      return SongUtils::SongInfo::SetMapData(in, levelDetail, difficultyBeatmap);
    }


    void GetNewMapData(rapidjson::GenericDocument<rapidjson::UTF16<char16_t>>& in, PinkCore::API::LevelDetails& levelDetail, GlobalNamespace::BeatmapDifficulty difficulty, GlobalNamespace::BeatmapCharacteristicSO* characteristic, GlobalNamespace::IPreviewBeatmapLevel* level){
      return SongUtils::SongInfo::SetMapData(in, levelDetail, level, difficulty, characteristic);
	}

    LevelDetails& GetCurrentMapData()
    {
        return SongUtils::SongInfo::get_mapData();
    }

}

