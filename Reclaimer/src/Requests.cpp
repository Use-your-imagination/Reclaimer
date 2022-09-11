#include "Requests.h"

#include "HTTPBuilder.h"
#include "BaseIOSocketStream.h"
#include "HTTPSNetwork.h"

#include "Constants.h"

using namespace std;

namespace requests
{
	string getLastVersion()
	{
		return web::HTTPBuilder().getRequest().parameters("repos/Use-your-imagination/LostConnection/releases/latest").headers
		(
			"Host", "api.github.com",
			"User-Agent", "Reclaimer",
			"Authorization", "token " + jsonSettings->getString("token"),
			"Accept", "application/vnd.github.v3+json"
		).build();
	}

	string getLoadUrl(const string& url)
	{
		return web::HTTPBuilder().getRequest().parameters(url.substr(url.find(".com") + 5)).headers
		(
			"Host", "api.github.com",
			"User-Agent", "Reclaimer",
			"Authorization", "token " + jsonSettings->getString("token"),
			"Accept", "application/octet-stream"
		).build();
	}

	string getAsset(const web::HTTPParser& parser)
	{
		static constexpr string_view https = "https://";
		static constexpr string_view com = ".com";

		const string& location = parser.getHeaders().at("Location");
		string host = string(location.begin() + location.find(https) + https.size(), location.begin() + location.find(com) + com.size());
		streams::IOSocketStream stream(make_unique<buffers::IOSocketBuffer>(make_unique<web::HTTPSNetwork>(host, "443")));
		string response;

		stream << web::HTTPBuilder().getRequest().parameters(location.substr(location.find(".com") + 5)).headers
		(
			"Host", host,
			"User-Agent", "Reclaimer",
			"Authorization", "token " + jsonSettings->getString("token"),
			"Accept", "application/octet-stream"
		).build();

		stream >> response;

		return web::HTTPParser(response).getBody();
	}
}
