#pragma once

#include <string>

namespace Hazel {

	class FileDialogs
	{
	public:
		// These return empty strings if cancelled
		//返回选中的地址
		static std::string OpenFile(const char* filter);
		//
		static std::string SaveFile(const char* filter);
	};

}