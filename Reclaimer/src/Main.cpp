#include <iostream>
#include <fstream>
#include <filesystem>

#include "BaseIOSocketStream.h"
#include "HTTPSNetwork.h"
#include "JSONParser.h"
#include "JSONArrayWrapper.h"
#include "HTTPParser.h"
#include "Exceptions/CantFindValueException.h"

#include "Constants.h"
#include "Requests.h"

#pragma comment (lib, "Networks.lib")

using namespace std;

void extractZip(const json::JSONParser& settings, const string& assetName);

int main(int argc, char** argv)
{
	if (!filesystem::exists(configuration::settings))
	{
		cout << format(R"(Can't find "{}")"sv, configuration::settings) << endl;

		return 0;
	}

	try
	{
		json::JSONParser settings = ifstream(configuration::settings.data());

		if (!filesystem::exists(settings.getString("outputPath")))
		{
			filesystem::create_directories(settings.getString("outputPath"));
		}

		streams::IOSocketStream stream(make_unique<buffers::IOSocketBuffer>(make_unique<web::HTTPSNetwork>("api.github.com", "443")));
		string response;

		stream << requests::getLastVersion();

		stream >> response;

		web::HTTPParser repositoryHTTP(response);

		if (repositoryHTTP.getResponseCode() == web::responseCodes::ok)
		{
			json::JSONParser repositoryJSON(repositoryHTTP.getBody());
			json::utility::JSONArrayWrapper assets(repositoryJSON.getArray("assets"));

			if (assets.size())
			{
				filesystem::directory_iterator it(settings.getString("outputPath"));

				for (const auto& i : it)
				{
					if (i.path().filename() == "Reclaimer.exe" || i.path().filename() == "settings.json")
					{
						continue;
					}

					filesystem::remove_all(i);
				}
			}

			for (size_t i = 0; i < assets.size(); i++)
			{
				auto& asset = get<json::utility::jsonObject>(assets.getObject(i).data.front().second);
				const string& assetName = asset.getString("name");

				stream << requests::getLoadUrl(asset.getString("url"));

				stream >> response;

				web::HTTPParser parser(response);

				if (parser.getResponseCode() == web::responseCodes::ok)
				{
					cout << format(R"(Start loading "{}")"sv, assetName) << endl;

					ofstream(filesystem::path(settings.getString("outputPath")) /= assetName, ios::binary) << parser.getBody();

					extractZip(settings, assetName);

					cout << format(R"(Finish loading "{}")"sv, assetName) << endl;
				}
				else if (parser.getResponseCode() == web::responseCodes::found)
				{
					cout << format(R"(Start loading "{}")"sv, assetName) << endl;

					ofstream(filesystem::path(settings.getString("outputPath")) /= assetName, ios::binary) << requests::getAsset(parser);

					extractZip(settings, assetName);

					cout << format(R"(Finish loading "{}")"sv, assetName) << endl;
				}
				else
				{
					cout << format(R"(Failed to load {})"sv, assetName) << endl;
				}
			}
		}
		else
		{
			cout << repositoryHTTP.getResponseMessage() << endl;
		}
	}
	catch (const exception& e)
	{
		cout << e.what() << endl;
	}

	return 0;
}

void extractZip(const json::JSONParser& settings, const string& assetName)
{
	if (assetName.find(".zip") == string::npos)
	{
		return;
	}

	filesystem::path outputPath = settings.getString("outputPath");

	system(format(R"(powershell.exe Expand-Archive -Path "{}" -DestinationPath "{}")", (outputPath / assetName).string(), outputPath.string()).data());

	try
	{
		if (settings.getBool("removeAfterExtract"))
		{
			filesystem::remove(outputPath / assetName);
		}
	}
	catch (const json::exceptions::CantFindValueException&)
	{

	}
}
