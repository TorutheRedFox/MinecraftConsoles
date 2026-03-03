#pragma once

#include <string>

class Language
{
private:
	static Language *singleton;
    std::unordered_map<std::wstring, std::wstring> localizationTable;
public:
	Language();
    static Language *getInstance();
    template<typename...Args>
    inline std::wstring getElement(const std::wstring& elementId, Args...)
    {
        if (this)
        {
            if (localizationTable.count(elementId))
                return localizationTable[elementId];
            else
                return elementId;
        }
    }
    std::wstring getElementName(const std::wstring& elementId);
    std::wstring getElementDescription(const std::wstring& elementId);
};