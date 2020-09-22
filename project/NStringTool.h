#ifndef NSTRINGTOOL_H
#define NSTRINGTOOL_H

#include <sstream>
#include <string>
#include <vector>

#ifndef stringvector
typedef std::vector<std::string> stringvector;
#endif

namespace NStringTool
{
	int Split(std::string InputString, stringvector* ResultVector, char SplitChar);
	int Split(std::string InputString, stringvector* ResultVector, char SplitChar, const char b0, const char b1);
  bool Split(std::string InputString, std::string& s1, std::string& s2, char SplitChar);


	int ReadLine(std::ifstream& ifile, std::string* Line);

	std::string Cut(const std::string& InStr, char CutChar);
	bool Compare(const std::string& str1, const std::string& str2, bool IgnoreCase=false);
	std::string Trim(std::string str, const char* trimChars);
	std::string TrimValid(std::string InStr, const char* validChars);
	std::string FilterUmlaut(const std::string& InStr);
	std::string RemoveExtension(const std::string& InStr);
	bool Strcmpx(const char* s1, const char* s2);
	std::string Shorten(const std::string& InString, const std::string Punkte, int Limit);
	std::string ShortenUtf8(const std::string& Utf8String, const std::string Punkte, int Limit);
	void RemoveUmlaut(char* c1, bool ToUpper);
	std::string Last(const std::string& InStr, unsigned int n);
	std::string First(const std::string& InStr, unsigned int n);
	std::string RemoveBlanks(std::string& InputString);

	void Trim(stringvector& sv, const char* trimChars);
	void TrimValid(stringvector& sv, const char* validChars);

	template <typename T>
  std::string toString(T i)
	{
		std::stringstream istr;
		istr <<  i;
		return istr.str();
	}


	template <typename T>
	T Cast(const std::string& str)
	{
		T Value;
		std::stringstream istr(str);
		istr >> Value;
		return Value;
	}

	template <typename T>
	T Cast(const std::string& str, int Precision)
	{
		T Value;
		std::stringstream istr(str);
		istr.precision(Precision);
		istr >> Value;
		return Value;
	}
};


#endif
