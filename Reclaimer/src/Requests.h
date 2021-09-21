#pragma once

#include <string>

#include "HTTPParser.h"

namespace requests
{
	std::string getLastVersion();

	std::string getLoadUrl(const std::string& url);

	std::string getAsset(const web::HTTPParser& parser);
}
