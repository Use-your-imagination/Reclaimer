#pragma once

#include <string>

namespace configuration
{
	inline constexpr std::string_view settings = "settings.json";
}

inline const json::JSONParser* jsonSettings;
