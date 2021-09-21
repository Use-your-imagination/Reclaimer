#include "Requests.h"

#include "HTTPBuilder.h"
#include "BaseIOSocketStream.h"
#include "HTTPSNetwork.h"

using namespace std;

namespace requests
{
	string getLastVersion()
	{
		return web::HTTPBuilder().getRequest().parameters("repos/Use-your-imagination/LostConnection/releases/latest").headers
		(
			"Host", "api.github.com",
			"User-Agent", "Reclaimer",
			"Accept", "application/vnd.github.v3+json"
		).build();
	}

	string getLoadUrl(const string& url)
	{
		return web::HTTPBuilder().getRequest().parameters(url.substr(url.find(".com") + 5)).headers
		(
			"Host", "api.github.com",
			"User-Agent", "Reclaimer",
			"Accept", "application/octet-stream"
		).build();
	}

	string getAsset(const web::HTTPParser& parser)
	{
		const string& location = parser.getHeaders().at("Location");
		string host = string(location.begin() + location.find("github"), location.begin() + location.find(".com") + 4);
		streams::IOSocketStream stream(make_unique<buffers::IOSocketBuffer>(make_unique<web::HTTPSNetwork>(host, "443")));
		string response;

		stream << web::HTTPBuilder().getRequest().parameters(location.substr(location.find(".com") + 5)).headers
		(
			"Host", host,
			"User-Agent", "Reclaimer",
			"Accept", "application/octet-stream"
		).build();

		stream >> response;

		return web::HTTPParser(response).getBody();
	}
}

/*
https://github-releases.githubusercontent.com/379357672/7c186396-6045-4b6a-926d-3471efafaa44?X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Credential=AKIAIWNJYAX4CSVEH53A%2F20210921%2Fus-east-1%2Fs3%2Faws4_request&X-Amz-Date=20210921T210417Z&X-Amz-Expires=300&X-Amz-Signature=a8357f709b8548f7bd76eb29e2d8da3a9b7844a775a7a20bb932dc80e72d8543&X-Amz-SignedHeaders=host&actor_id=0&key_id=0&repo_id=379357672&response-content-disposition=attachment%3B%20filename%3DLostConnection.rar&response-content-type=application%2Foctet-stream
*/
