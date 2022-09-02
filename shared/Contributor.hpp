#pragma once
#include <string>
#include <codecvt>
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/document.h"


namespace PinkCore
{
	struct Contributor
	{
		std::u16string name = u"";
		std::u16string role = u"";
		std::u16string iconPath = u"";

		Contributor(rapidjson::GenericValue<rapidjson::UTF16<char16_t>>& val)
		{
			auto nameItr = val.FindMember(u"_name");
			auto roleItr = val.FindMember(u"_role");
			auto iconPathItr = val.FindMember(u"_iconPath");

			if (nameItr != val.MemberEnd()) {
				name = std::u16string(reinterpret_cast<const char16_t*>(nameItr->value.GetString()), nameItr->value.GetStringLength());
			}			
			if (roleItr != val.MemberEnd()) {
				role = std::u16string(reinterpret_cast<const char16_t*>(roleItr->value.GetString()), roleItr->value.GetStringLength());
			}			
			if (iconPathItr != val.MemberEnd()) {
				iconPath = std::u16string(reinterpret_cast<const char16_t*>(iconPathItr->value.GetString()), iconPathItr->value.GetStringLength());
			}
		}
			
		bool operator==(Contributor& other)
		{
			return (name == other.name && role == other.role);
		}
	};
}
#undef pinkcore_id