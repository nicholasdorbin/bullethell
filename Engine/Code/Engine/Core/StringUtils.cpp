#include "Engine/Core/StringUtils.hpp"
#include <stdarg.h>
#include <algorithm>
#include "Engine/Math/Rgba.hpp"
#include "Engine/Math/Vector2.hpp"


//-----------------------------------------------------------------------------------------------
const int STRINGF_STACK_LOCAL_TEMP_LENGTH = 2048;


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const char* format, ... )
{
	char textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, STRINGF_STACK_LOCAL_TEMP_LENGTH, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	return std::string( textLiteral );
}


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const int maxLength, const char* format, ... )
{
	char textLiteralSmall[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	char* textLiteral = textLiteralSmall;
	if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		textLiteral = new char[ maxLength ];

	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, maxLength, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ maxLength - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	std::string returnValue( textLiteral );
	if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		delete[] textLiteral;

	return returnValue;
}

std::string ExtractToken(const std::string& inputString, const std::string& stringBeforeToken, const std::string& delimiter)
{
	std::string result;
	
	int tokenPos = inputString.find(stringBeforeToken);
	int tokenLength = stringBeforeToken.length();
	int delimPos = inputString.find(delimiter, tokenPos);

	int substrStart = tokenPos + tokenLength;
	int substrLength = delimPos - (tokenPos + tokenLength);
	result = inputString.substr(substrStart, substrLength);

	return result;
}


std::vector<std::string> TokenizeStringOnDelimiter(std::string inputString, const std::string& delimiter)
{

	size_t found = 1;
	std::string curString = inputString;
	std::vector<std::string> outStrings;
	
	while (found != std::string::npos)
	{
		std::string subStr;
		found = curString.find(delimiter);
		if (found == std::string::npos)
		{
			subStr = curString;
		}
		else
		{
			subStr = curString.substr(0, found);
			curString.erase(0, found + 1);
		}
			

		
		if (subStr != "")
		{
			outStrings.push_back(subStr);
		}
	}

	return outStrings;
}


void ReplaceCharsInString(std::string& inputString, const char delimiter, const char replacement)
{
	std::replace(inputString.begin(), inputString.end(), delimiter, replacement);
}

//-----------------------------------------------------------------------------------------------
void SetTypeFromString(int& outValue, const std::string& valueAsString)
{
	outValue = atoi(valueAsString.c_str());
}


void SetTypeFromString(float& outValue, const std::string& valueAsString)
{
	outValue = (float)atof(valueAsString.c_str());
}


//-----------------------------------------------------------------------------------------------
void SetTypeFromString(char& outValue, const std::string& valueAsString)
{
	outValue = valueAsString[0];
}


//-----------------------------------------------------------------------------------------------
void SetTypeFromString(unsigned char& outValue, const std::string& valueAsString)
{
	outValue = (unsigned char)atoi(valueAsString.c_str());
}

void SetTypeFromString(std::string& outValue, const std::string& valueAsString)
{
	outValue = valueAsString;
}
void SetTypeFromString(Rgba& outValue, const std::string& valueAsString)
{
	if (!valueAsString.empty())
		outValue = Rgba(valueAsString);
	else
	{
		outValue = Rgba();
	}
}

void SetTypeFromString(Vector2& outValue, const std::string& valueAsString)
{

	if (valueAsString.empty())
	{ 
		outValue = Vector2::ZERO;
		return;
	}


	std::string strCopy = valueAsString;

	size_t pos = strCopy.find(",");
	std::string xStr = strCopy.substr(0, pos);
	strCopy.erase(0, pos + 1);


	std::string yStr = strCopy;

	outValue = Vector2(stof(xStr), stof(yStr));
}

void SetTypeFromString(bool& outValue, const std::string& valueAsString)
{
	if (valueAsString == "true" || valueAsString == "1" || valueAsString == "yes")
		outValue = true;
	else
		outValue = false;
}

size_t StringHash(const std::string& str)
{
	return std::hash<std::string>{}(str);
}
