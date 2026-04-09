#pragma once

#include <string>

inline std::string WideToUtf8(const WCHAR* wstr)
{
	if (!wstr)
	{
		return {};
	}

	int size = WideCharToMultiByte(
		CP_UTF8,
		0,
		wstr,
		-1,
		nullptr,
		0,
		nullptr,
		nullptr);

	if (size <= 0)
	{
		return {};
	}

	std::string ustr(size - 1, '\0');

	WideCharToMultiByte(
		CP_UTF8,
		0,
		wstr,
		-1,
		&ustr[0],
		size - 1,
		nullptr,
		nullptr);

	return ustr;
}

inline std::wstring Utf8ToWide(const std::string& utf8)
{
	if (utf8.empty())
	{
		return {};
	}

	int size = MultiByteToWideChar(
		CP_UTF8,
		0,
		utf8.c_str(),
		-1,
		nullptr,
		0);

	if (size <= 0)
	{
		return {};
	}

	std::wstring wstr(size - 1, L'\0');

	MultiByteToWideChar(
		CP_UTF8,
		0,
		utf8.c_str(),
		-1,
		&wstr[0],
		size - 1);

	return wstr;
}