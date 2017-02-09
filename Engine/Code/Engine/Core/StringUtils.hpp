#pragma once
//-----------------------------------------------------------------------------------------------
#include <string>
#include <vector>
class Rgba;
class Vector2;


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const char* format, ... );
const std::string Stringf( const int maxLength, const char* format, ... );

std::string ExtractToken(const std::string& inputString, const std::string& stringBeforeToken, const std::string& delimiter = "\n");

std::vector<std::string> TokenizeStringOnDelimiter(std::string inputString, const std::string& delimiter = ",");

void ReplaceCharsInString(std::string& inputString, const char delimiter, const char replacement);

void SetTypeFromString(int& outValue, const std::string& valueAsString);
void SetTypeFromString(float& outValue, const std::string& valueAsString);
void SetTypeFromString(char& outValue, const std::string& valueAsString);
void SetTypeFromString(unsigned char& outValue, const std::string& valueAsString);
void SetTypeFromString(std::string& outValue, const std::string& valueAsString);
void SetTypeFromString(Rgba& outValue, const std::string& valueAsString);
void SetTypeFromString(bool& outValue, const std::string& valueAsString);
void SetTypeFromString(Vector2& outValue, const std::string& valueAsString);

size_t StringHash(const std::string& str);