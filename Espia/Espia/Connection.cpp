#include "Connection.h"


bool Tool::Connection::ConnectionFuncs::GetConnectionState()
{
	INTERNET_STATUS connectedStatus = INTERNET_STATUS::CONNECTION_ERROR;
	connectedStatus = Connection::ConnectionFuncs::InternetAccess();
	switch (connectedStatus)
	{
	case INTERNET_STATUS::CONNECTED:
		return true;
		break;
	case INTERNET_STATUS::DISCONNECTED:
		Tool::Utils::print(STR("No internet access"));
		return false;
		break;
	default:
		Tool::Utils::print_error(STR("Unable to determine internet state"));
		return false;
	}
}

void Tool::Connection::ConnectionFuncs::WaitForConnection()
{
	while (!Connection::ConnectionFuncs::GetConnectionState())
		Sleep(4000);
}

Tool::Connection::INTERNET_STATUS Tool::Connection::ConnectionFuncs::InternetAccess() {
	INTERNET_STATUS iConnectionStatus = INTERNET_STATUS::CONNECTION_ERROR;
	HRESULT hr = S_FALSE;
	try
	{
		hr = CoInitialize(NULL);
		if (SUCCEEDED(hr))
		{
			INetworkListManager* pNetworkListManager;
			hr = CoCreateInstance(CLSID_NetworkListManager, NULL, CLSCTX_ALL, __uuidof(INetworkListManager), (LPVOID*)&pNetworkListManager);
			if (SUCCEEDED(hr))
			{
				NLM_CONNECTIVITY nlmConnectivity = NLM_CONNECTIVITY::NLM_CONNECTIVITY_DISCONNECTED;
				VARIANT_BOOL isConnected = VARIANT_FALSE;
				hr = pNetworkListManager->get_IsConnectedToInternet(&isConnected);
				if (SUCCEEDED(hr))
				{
					if (isConnected == VARIANT_TRUE)
						iConnectionStatus = INTERNET_STATUS::CONNECTED;
					else
						iConnectionStatus = INTERNET_STATUS::DISCONNECTED;
				}
				if (isConnected == VARIANT_FALSE && SUCCEEDED(pNetworkListManager->GetConnectivity(&nlmConnectivity)))
				{
					if (nlmConnectivity & (NLM_CONNECTIVITY_IPV4_LOCALNETWORK | NLM_CONNECTIVITY_IPV4_SUBNET | NLM_CONNECTIVITY_IPV6_LOCALNETWORK | NLM_CONNECTIVITY_IPV6_SUBNET))
					{
						iConnectionStatus = INTERNET_STATUS::CONNECTED_TO_LOCAL;
					}
				}
				pNetworkListManager->Release();
			}
		}
		CoUninitialize();
	}
	catch (...)
	{
		iConnectionStatus = INTERNET_STATUS::CONNECTION_ERROR;
	}

	return iConnectionStatus;

}
