#ifndef CONVERT_IMPL_NAROWCHAR_WIDECHAR__HPP
#define CONVERT_IMPL_NAROWCHAR_WIDECHAR__HPP

#include "ConvertFunction.hpp"

namespace akasha
{
template<>
	inline std::string convertType(const std::wstring& s)
{
	const size_t len = s.length() * MB_CUR_MAX + 1;
	char* mbs = new char[len];
	wcstombs(mbs, s.c_str(),len);
	std::string result(mbs);
	delete[] mbs;
	return result;
}

	template<>
inline std::wstring convertType(const std::string& s)
{
	const size_t len = s.length() + 1;
	wchar_t* wcs = new wchar_t[len];
	mbstowcs(wcs, s.c_str(), len);
	std::wstring result(wcs);
	delete[] wcs;
	return result;
}


}
#endif
