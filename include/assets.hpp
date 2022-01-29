#pragma once
#define DECLARE_FILE(name, prefix) extern "C" uint8_t _binary_##name##_start[]; extern "C" uint8_t _binary_##name##_end[]; struct prefix##name { static size_t getLength() { return _binary_##name##_end - _binary_##name##_start; } static uint8_t* getData() { return _binary_##name##_start; } };
DECLARE_FILE(Donation_png,)
DECLARE_FILE(DonationActive_png,)
DECLARE_FILE(Mango_png,)
DECLARE_FILE(MangoActive_png,)
DECLARE_FILE(MissingSprite_png,)
DECLARE_FILE(RequirementFound_png,)
DECLARE_FILE(RequirementMissing_png,)
DECLARE_FILE(SuggestionFound_png,)
DECLARE_FILE(SuggestionMissing_png,)
DECLARE_FILE(WIP_png,)
