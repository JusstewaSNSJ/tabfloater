#include "windowhandler.h"
#include "../libs/loguru/src/loguru.hpp"
#include <iostream>
#include <string>
#include <regex>

#ifdef _WIN32
#include <fcntl.h>

int setBinaryMode(FILE* file)
{
	int result;

	result = _setmode(_fileno(file), _O_BINARY);
	if (result == -1)
	{
		perror("Cannot set mode");
		return result;
	}
	// set do not use buffer
	result = setvbuf(file, NULL, _IONBF, 0);	
	if (result != 0)
	{
		perror("Cannot set zero buffer");
		return result;
	}

	return 0;
}
#endif

unsigned int readFirstFourBytesFromStdIn()
{
    char buffer[4];

    std::cin.read(buffer, 4);
    if (std::cin.eof())
    {
        return 0;
    }

    return *reinterpret_cast<unsigned int *>(buffer);
}

std::string readStringFromStdIn(unsigned int length)
{
    char *buffer = new char[length];

    std::cin.read(buffer, length);
    std::string json(buffer);
    delete[] buffer;

    return json;
}

std::string getJsonValueByKey(std::string jsonContents, std::string key)
{
    // We are looking for the JSON value with a format like this: "key": "value"
    // This regex also allows the '\"' character in the value. It looks for the
    // closing '"' character and allows a comma at the end. The full value is the
    // first capture grop. This wouldn't work for nested JSONs, but TabFloater
    // only sends non-nested JSON.
    std::regex jsonRegex(R"(\")" + key + R"(\"\s*:\s*\"(([^\"]?(\\\")*)*)\",?)");
    std::smatch matches;

    if (std::regex_search(jsonContents, matches, jsonRegex) && matches.size() >= 1)
    {
        return matches[1].str();
    }

    return std::string();
}

void sendOkStatus()
{
    std::string okMessage = "{\"status\":\"ok\"}";
    unsigned int len = okMessage.length();

    std::cout << char(len >> 0)
              << char(len >> 8)
              << char(len >> 16)
              << char(len >> 24);

    std::cout << okMessage;
}

int main(int argc, char* argv[])
{
    loguru::init(argc, argv);
    loguru::add_file("everything.log", loguru::Append, loguru::Verbosity_MAX);
    LOG_F(INFO, "Starting");


    #ifdef _WIN32
    setBinaryMode(stdin);
    setBinaryMode(stdout);
    #endif

    while (1)
    {
        // See https://developer.chrome.com/extensions/nativeMessaging
        // As described in the Chrome native messaging protocol, the JSON message
        // is preceded with the message length in the first 4 bytes. We need to
        // read that, and then read that many characters that will make up the message.
        // If the message length is 0, Chrome has closed the message port and we can
        // stop reading.

        unsigned int messageLength = readFirstFourBytesFromStdIn();
        if (!messageLength)
        {
            break;
        }

        std::string json = readStringFromStdIn(messageLength);
        std::string action = getJsonValueByKey(json, "action");

        if (action.compare("ping") == 0)
        {
            sendOkStatus();
        }
        else if (action.compare("makepanel") == 0)
        {
            std::string title = getJsonValueByKey(json, "title");
            setWindowAlwaysOnTopAndSkipTaskbar(title);
            sendOkStatus();
        }
    }

    return 0;
}