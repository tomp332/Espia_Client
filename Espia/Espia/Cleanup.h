#pragma once
#include <Windows.h>
#include <iostream>
#include "Utils.hpp"

namespace Tool
{
	class Cleanup{
	public:
		static void CleanupTool();
		static void CleanAll(std::string& sMainProductsDir);
	};
}