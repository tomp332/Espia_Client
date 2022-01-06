#pragma once
#include <Windows.h>
#include <netlistmgr.h>
#include "Utils.hpp"

namespace Tool
{
	namespace Connection
	{
		enum class INTERNET_STATUS
		{
			CONNECTED,
			DISCONNECTED,
			CONNECTED_TO_LOCAL,
			CONNECTION_ERROR
		};

		class ConnectionFuncs {
		public:
			static INTERNET_STATUS InternetAccess();
			static bool GetConnectionState();
			static void WaitForConnection();
		};
	}
}
