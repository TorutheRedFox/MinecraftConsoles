#include "stdafx.h"
#include "Language.h"

// 4J - TODO - properly implement

Language *Language::singleton = NULL;

Language::Language()
{
    BufferedReader* br = new BufferedReader(new InputStreamReader(InputStream::getResourceAsStream(L"Common\\res\\lang\\en_US.lang")));

    if (br)
    {
        wstring prevLine = L"null";
        wstring line = L"";
        int sameLines = 0;
        while (true)
        {
            line = br->readLine();
            line = trimString(line);
            if (line == prevLine)
            {
                if (sameLines++ > 16)
                    break;
            }
            else
                sameLines = 0;
            prevLine = line;
            if (line.length() > 0)
            {
                std::wstring tag = line.substr(0, line.find('='));
                if (localizationTable.count(tag) == 0)
                {
                    std::wstring string = line.substr(line.find('=') + 1);
                    localizationTable.insert({ tag, string });
                }
            }
        }

        br->close();
        delete br;
    }

    br = new BufferedReader(new InputStreamReader(InputStream::getResourceAsStream(L"Common\\res\\lang\\stats_US.lang")));

    if (br)
    {
        wstring prevLine = L"null";
        wstring line = L"";
        int sameLines = 0;
        while (true)
        {
            line = br->readLine();
            line = trimString(line);
            if (line == prevLine)
            {
                if (sameLines++ > 16)
                    break;
            }
            else
                sameLines = 0;
            prevLine = line;
            if (line.length() > 0)
            {
                std::wstring tag = line.substr(0, line.find('='));
                if (localizationTable.count(tag) == 0)
                {
                    std::wstring string = line.substr(line.find('=') + 1);
                    localizationTable.insert({ tag, string });
                }
            }
        }

        br->close();
        delete br;
    }
}

Language *Language::getInstance()
{
    if (!singleton)
        singleton = new Language(); // construct
	return singleton;
}

std::wstring Language::getElementName(const std::wstring& elementId)
{
    if (localizationTable.count(elementId + L".name") > 0)
        return localizationTable[elementId + L".name"];
    else
	    return elementId;
}

std::wstring Language::getElementDescription(const std::wstring& elementId)
{
    if (localizationTable.count(elementId + L".desc") > 0)
        return localizationTable[elementId + L".desc"];
    else
        return elementId;
}