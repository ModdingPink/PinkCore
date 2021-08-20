#pragma once
#include <string>
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/document.h"

namespace PinkCore
{
	struct Contributor
	{
		std::string name;
		std::string role;
		std::string iconPath;

		Contributor(rapidjson::Value& val)
		{
			name = val.HasMember("_name") ? val["_name"].GetString() : "";
			role = val.HasMember("_role") ? val["_role"].GetString() : "";
			iconPath = val.HasMember("_iconPath") ? val["_iconPath"].GetString() : "";
		}
			
		bool operator==(Contributor& other)
		{
			return (name == other.name && role == other.role);
		}
	};
}