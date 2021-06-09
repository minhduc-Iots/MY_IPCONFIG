#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <stdio.h>
#include <iphlpapi.h>
#include <iostream>
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "IPHLPAPI.lib")
using namespace std;

/*khai bao bien*/
IP_ADAPTER_INFO  *pAdapterInfo; // con tro luu du lieu
ULONG            ulOutBufLen;   // bien luu tru do dai du lieu
DWORD            dwRetVal;		// bien thong bao loi
/*Funtion*/
void call_ip_config() {
	if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) != ERROR_SUCCESS) {
		printf("GetAdaptersInfo call failed with %d\n", dwRetVal);
	} else {
		PIP_ADAPTER_INFO pAdapter = pAdapterInfo;
		printf("Windows IP Configuration\n\n\n");
		while (pAdapter) {
			printf("Adapter Name: \t%s\n", pAdapter->AdapterName);
			printf("Adapter Desc: \t%s\n", pAdapter->Description);
			printf("Adapter Addr: \t");
			for (UINT i = 0; i < pAdapter->AddressLength; i++) {
				if (i == (pAdapter->AddressLength - 1))
					printf("%.2X\n",(int)pAdapter->Address[i]);
				else
					printf("%.2X-",(int)pAdapter->Address[i]);
			}
			printf("%s\n\n", pAdapter->Description);
			printf("\tConnection-specific DNS Suffix  .: %s\n", pAdapter->DhcpServer.IpAddress.String);
			printf("\tIPv4 Address. . . . . . . . . . . : %s\n", pAdapter->IpAddressList.IpAddress.String);
			printf("\tSubnet Mask . . . . . . . . . . . : %s\n", pAdapter->IpAddressList.IpMask.String);
			printf("\tDefault Gateway . . . . . . . . . : %s\n\n\n", pAdapter->GatewayList.IpAddress.String);

			pAdapter = pAdapter->Next;
		}
	}
	if (pAdapterInfo)
		free(pAdapterInfo);
}
void call_ip_config_all() {
	if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) != ERROR_SUCCESS) {
		printf("GetAdaptersInfo call failed with %d\n", dwRetVal);
	} else {
		PIP_ADAPTER_INFO pAdapter = pAdapterInfo;
		printf("Windows IP Configuration\n\n\n");
		while (pAdapter) {
			printf("%s\n\n", pAdapter->Description);
			printf("\tConnection-specific DNS Suffix  .: %s\n", pAdapter->DhcpServer.IpAddress.String);
			printf("\tDescription . . . . . . . . . . . : %s\n", pAdapter->Description);
			printf("\tPhysical Address. . . . . . . . . : ");
			for (UINT i = 0; i < pAdapter->AddressLength; i++) {
				if (i == (pAdapter->AddressLength - 1))
					printf("%.2X\n", (int)pAdapter->Address[i]);
				else
					printf("%.2X-", (int)pAdapter->Address[i]);
			}
			printf("\tDHCP Enabled. . . . . . . . . . . : %s\n", pAdapter->DhcpEnabled ? "YES" : "NO");
			printf("\tAutoconfiguration Enabled . . . . : \n");
			printf("\tLink-local IPv6 Address . . . . . : \n");
			printf("\tIPv4 Address. . . . . . . . . . . : %s\n", pAdapter->IpAddressList.IpAddress.String);
			printf("\tSubnet Mask . . . . . . . . . . . : %s\n", pAdapter->IpAddressList.IpMask.String);
			printf("\tDefault Gateway . . . . . . . . . : %s\n", pAdapter->GatewayList.IpAddress.String);
			printf("\tDHCP Server . . . . . . . . . . . : %s\n\n", pAdapter->DhcpServer.IpAddress.String);

			pAdapter = pAdapter->Next;
		}
	}
}
void call_renew() {
	ULONG ulOutBufLen = 0;
	DWORD dwRetVal = 0;
	PIP_INTERFACE_INFO pInfo;
	pInfo = (IP_INTERFACE_INFO *)malloc(sizeof(IP_INTERFACE_INFO));
// goi GetInterfaceInfo de lay kich thuoc can thiet ulOutBufLen
	if (GetInterfaceInfo(pInfo, &ulOutBufLen) == ERROR_INSUFFICIENT_BUFFER) {
		free(pInfo);
		pInfo = (IP_INTERFACE_INFO *)malloc(ulOutBufLen);
	}
//goi lan 2 de lay gia tri mong muon
	if ((dwRetVal = GetInterfaceInfo(pInfo, &ulOutBufLen)) == NO_ERROR) {

		printf("\nCall ipconfig /renew...\n");
	} else if (dwRetVal == ERROR_NO_DATA) {
		printf("There are no network adapters with IPv4 enabled on the local system\n");
		return;
	} else {
		LPVOID lpMsgBuf;
		printf("GetInterfaceInfo failed.\n");

		if (FormatMessage(
		            FORMAT_MESSAGE_ALLOCATE_BUFFER |
		            FORMAT_MESSAGE_FROM_SYSTEM |
		            FORMAT_MESSAGE_IGNORE_INSERTS,
		            NULL,
		            dwRetVal,
		            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		            (LPTSTR)&lpMsgBuf,
		            0,
		            NULL)) {
			printf("\tError: %s", lpMsgBuf);
		}
		LocalFree(lpMsgBuf);
		return;
	}
// Goi IpReleaseAddress và IpRenewAddress de renew va release
// tra ve dia chi IP dau tien tren network bang su dung lenh GetInterfaceInfo.

	PIP_INTERFACE_INFO pAdapter = pInfo;
	int isSucces = 0;
	for (int i = 0; i < pAdapter->NumAdapters; i++) {

		if ((dwRetVal = IpRenewAddress(&pInfo->Adapter[i])) == NO_ERROR) {
			isSucces = 1;
		}
	}
	call_ip_config();
	if (isSucces == 1) {
		printf("\n\nipconfig /renew succeeded.\n");
	} else {
		printf("\n\nipconfig /renew failed.\n");
	}
	if (pInfo != NULL) {
		free(pInfo);
	}
	return;
}
void call_release() {
	ULONG ulOutBufLen = 0;
	DWORD dwRetVal = 0;
	PIP_INTERFACE_INFO pInfo;
	pInfo = (IP_INTERFACE_INFO *)malloc(sizeof(IP_INTERFACE_INFO));
	if (GetInterfaceInfo(pInfo, &ulOutBufLen) == ERROR_INSUFFICIENT_BUFFER) {
		free(pInfo);
		pInfo = (IP_INTERFACE_INFO *)malloc(ulOutBufLen);
	}
	if ((dwRetVal = GetInterfaceInfo(pInfo, &ulOutBufLen)) == NO_ERROR) {
		printf("\nCall ipconfig /release...\n");
	} else if (dwRetVal == ERROR_NO_DATA) {
		printf("There are no network adapters with IPv4 enabled on the local system\n");
		return;
	} else {
		LPVOID lpMsgBuf;
		printf("GetInterfaceInfo failed.\n");

		if (FormatMessage(
		            FORMAT_MESSAGE_ALLOCATE_BUFFER |
		            FORMAT_MESSAGE_FROM_SYSTEM |
		            FORMAT_MESSAGE_IGNORE_INSERTS,
		            NULL,
		            dwRetVal,
		            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		            (LPTSTR)&lpMsgBuf,
		            0,
		            NULL)) {
			printf("\tError: %s", lpMsgBuf);
		}
		LocalFree(lpMsgBuf);
		return;
	}

	PIP_INTERFACE_INFO pAdapter = pInfo;
	int isSucces = 0;
	for (int i = 0; i < pAdapter->NumAdapters; i++) {
		if ((dwRetVal = IpReleaseAddress(&pInfo->Adapter[i])) == NO_ERROR) {
			isSucces = 1;
		} else {
		}
	}
	call_ip_config();
	if (isSucces == 1) {
		printf("\n\nipconfig /release succeeded.\n");
	} else {
		printf("\n\nipconfig /release failed.\n");
	}
	if (pInfo != NULL) {
		free(pInfo);
	}
	return;
}
int main() {
	//string menu = "";
	int chon_menu = 0;
	pAdapterInfo = (IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO));
	ulOutBufLen = sizeof(IP_ADAPTER_INFO);

	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) != ERROR_SUCCESS) {
		free(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO *)malloc(ulOutBufLen);
	}
	do {
		cout << "\t\t\t\t\t\t Write by Duc and Ngoc"<<endl;
		cout << "Menu: " << endl;
		cout << "\tOption 1: ipconfig" << endl;
		cout << "\tOption 2: ipconfig /all" << endl;
		cout << "\tOption 3: ipconfig /renew" << endl;
		cout << "\tOption 4: ipconfig /release" << endl;
		cout << "\tOPtion 5: Exit"<<endl;
		cout << "Option: ";
		cin >> chon_menu ;
		switch (chon_menu) {
			case 1:
				call_ip_config();
				break;
			case 2:
				call_ip_config_all();
				break;
			case 3:
				call_renew();
				break;
			case 4:
				call_release();
				break;
			case 5:
				chon_menu = 0;
				cout << "Thank you for using my ipconfig"<<endl;
				cout << "\t\t\t\t\t\t Write by Duc and Ngoc"<<endl;
				break;
			default:
				break;
		}
	} while (chon_menu != 0);
	getchar();
}
