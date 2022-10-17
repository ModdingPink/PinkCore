#pragma once
#include <string_view>
#include "beatsaber-hook/shared/utils/typedefs.h"
struct IncludedAsset {
    IncludedAsset(uint8_t* start, uint8_t* end) : array(reinterpret_cast<Array<uint8_t>*>(start)) {
        array->klass = nullptr;
        array->monitor = nullptr;
        array->bounds = nullptr;
        array->max_length = end - start - 33;
        *(end - 1)= '\0';
    }
    
    operator ArrayW<uint8_t>() const {
        init();
        return array;
    }
    operator std::string_view() const {
        return { reinterpret_cast<char*>(array->values), array->Length() };
    }
    
    operator std::span<uint8_t>() const {
        return { array->values, array->Length() };
    }
    void init() const {
        if(!array->klass)
            array->klass = classof(Array<uint8_t>*);
    }
    private:
        Array<uint8_t>* array;
};
#define DECLARE_FILE(name)                         \
    extern "C" uint8_t _binary_##name##_start[];  \
    extern "C" uint8_t _binary_##name##_end[];    \
    const IncludedAsset name { _binary_##name##_start, _binary_##name##_end};
namespace IncludedAssets {

	DECLARE_FILE(Colors_png)
	DECLARE_FILE(Donation_png)
	DECLARE_FILE(DonationActive_png)
	DECLARE_FILE(Info_png)
	DECLARE_FILE(MainMenuIcon_png)
	DECLARE_FILE(MainMenuIconEmpty_png)
	DECLARE_FILE(MainMenuIconEmptyHighlight_png)
	DECLARE_FILE(MainMenuIconFlashing_png)
	DECLARE_FILE(MainMenuIconHighlight_png)
	DECLARE_FILE(MainMenuIconHighlightFlashing_png)
	DECLARE_FILE(MissingSprite_png)
	DECLARE_FILE(NewsIcon_png)
	DECLARE_FILE(NewsIconActive_png)
	DECLARE_FILE(RequirementFound_png)
	DECLARE_FILE(RequirementMissing_png)
	DECLARE_FILE(SuggestionFound_png)
	DECLARE_FILE(SuggestionMissing_png)
	DECLARE_FILE(WIP_png)

}
