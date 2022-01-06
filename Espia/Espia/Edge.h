#pragma once
#include <Windows.h>
#include <iostream>
#include "Chromium.h"
#include "Configuration.h"

namespace Tool
{
	namespace Browsers
	{
		class Edge: public Chromium {
		public:
			Edge(Tool::Configuration::config_t MainContext_t);			
		};
	}
}