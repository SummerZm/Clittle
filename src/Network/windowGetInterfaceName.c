
// 参数网卡名字
extern int isFactoryDefaultIp(char* adapterName){
	HKEY  hkey;
	LPTSTR szPtr;
	TCHAR strKeyName[256];
	TCHAR szTemp[256];
	TCHAR adapterNameW[64];
	DWORD dwType;
	DWORD dwSize = sizeof(szTemp);
	LONG regRet;
	int changeStatus;
	char ipaddr[128];
	int ret = 0;

	if(NULL == adapterName){
		return 0;
	}
	
	// 宽字节与多字节的转换
	changeStatus = SS_Win_mByteToWChar(adapterName, adapterNameW);

	if(changeStatus == 0)	return 0;
	memset(strKeyName, 0, 256);
	swprintf(strKeyName,L"Comm\\%s\\Parms\\TCPIP", adapterNameW);
	
	regRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE,strKeyName,0,KEY_READ,&hkey);
	if(regRet != ERROR_SUCCESS){
		SS_LOG(SS_DEBUG, "Open reg error %lu\n", regRet);
		return 0;
	}

	regRet = RegQueryValueEx (hkey, TEXT("IpAddress"), 0, &dwType, (LPBYTE)szTemp,&dwSize); 
	if ((regRet == ERROR_SUCCESS) && ((dwType == REG_SZ) || (dwType == REG_MULTI_SZ))) {
		//SS_LOG(SS_DEBUG, "get SUCCESS\n");
		if(szTemp[0]){
			memset(ipaddr, 0, 128);
			changeStatus = SS_Win_wCharToMByte(szTemp, ipaddr);
			if(changeStatus == 0){
				//SS_LOG(SS_DEBUG, "wcharToChar fail\n");
				return 0;
			}
			//SS_LOG(SS_DEBUG, "Ip %s\n", ipaddr);
			if(strcmp(ipaddr, "192.168.1.168") == 0){
				ret = 1;
			}
		}
	}
	else{
		SS_LOG(SS_DEBUG, "Get RegTable Ip failed error: %lu\n", regRet);
	}

	if(ERROR_SUCCESS != RegCloseKey(hkey)){
		SS_LOG(SS_DEBUG, "close reg Failed\n");
	}
	//SS_LOG(SS_DEBUG, "close reg OK\n");
	return ret;
}