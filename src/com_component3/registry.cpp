#include<iostream>
#include<objbase.h>
#include<assert.h>
#include<strsafe.h>
#include<wchar.h>
#include<winuser.h>
#include<winbase.h>
#pragma comment(lib,"user32.lib")
void CLSIDtoChar(const CLSID&, char*, int);
BOOL CreateKey(const char*, const char*, const char*);
LONG RecursiveDeleteKey(HKEY key, const char*);
void ErrorExit(LPTSTR);

const int CLSID_STRLEN = 39;
LSTATUS error_code;

void ErrorExit(LPTSTR lpszFunction) 
{ 

    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError(); 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    // Display the error message and exit the process

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
        (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR)); 
    char intStr[64];
    _itoa(error_code, intStr, 10);
    char msg[128];
    strcpy_s(msg, 128, "ERROR: ");
    strcat_s(msg, 128, intStr);
    StringCchPrintf((LPTSTR)lpDisplayBuf, 
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"), 
        lpszFunction, dw, lpMsgBuf); 
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, (LPCTSTR)msg, MB_OK); 
    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
    ExitProcess(dw); 
}


extern "C" HRESULT RegisterDll(HMODULE hModule, const CLSID& clsid, const char* szFriendlyName, const char* szVerIndProgName, const char* szProgName){
    char szCLSID[CLSID_STRLEN];
    char szKey[64];
    char szFile[512];
    CLSIDtoChar(clsid, szCLSID, CLSID_STRLEN);

    errno_t eRes = strcpy_s(szKey,64,"CLSID\\");
    assert(eRes == 0);
    eRes = strcat_s(szKey, 64, szCLSID);
    assert(eRes == 0);
    DWORD hRes = GetModuleFileNameA(hModule, szFile, 512);
    assert(hRes != 0);
    
    BOOL bRes = CreateKey(szKey, NULL, szFriendlyName);
    assert(bRes == TRUE);
    bRes = CreateKey(szKey, "InProcServer32", szFile);
    assert(bRes == TRUE);
    bRes = CreateKey(szKey, "ProgId", szProgName);
    assert(bRes == TRUE);
    
    bRes = CreateKey(szProgName, NULL, szFriendlyName);
    assert(bRes == TRUE);
    bRes = CreateKey(szProgName, "CLSID", szCLSID);
    assert(bRes == TRUE);

    bRes = CreateKey(szVerIndProgName, NULL, szFriendlyName);
    assert(bRes == TRUE);
    bRes = CreateKey(szVerIndProgName, "CLSID", szCLSID);
    assert(bRes == TRUE);
    bRes = CreateKey(szVerIndProgName, "CurrVer", szProgName);
    assert(bRes == TRUE);
    return S_OK;

}

extern "C" HRESULT UnregisterDll(const CLSID& clsid, const char* szProgName, const char* szVerIndProgName){
    char szKey[64];
    char szCLSID[CLSID_STRLEN];
    CLSIDtoChar(clsid, szCLSID, CLSID_STRLEN);
    errno_t eRes = strcpy_s(szKey, 64, "CLSID\\");
    assert(eRes == 0);
    eRes = strcat_s(szKey, 64, szCLSID);
    assert(eRes == 0);
    LONG res = RecursiveDeleteKey(HKEY_CLASSES_ROOT, szKey);
    assert(res == ERROR_SUCCESS || res == ERROR_FILE_NOT_FOUND);
    RecursiveDeleteKey(HKEY_CLASSES_ROOT, szProgName);
    assert(res == ERROR_SUCCESS || res == ERROR_FILE_NOT_FOUND);
    RecursiveDeleteKey(HKEY_CLASSES_ROOT, szVerIndProgName);
    assert(res == ERROR_SUCCESS || res == ERROR_FILE_NOT_FOUND);
    return S_OK;

}




void CLSIDtoChar(const CLSID& clsid, char* szCLSID, int length){
    //0xbb, 0x0, 0x80, 0xc7, 0xb2, 0xd6, 0x82});
    LPOLESTR wszCLSID;
    HRESULT hr = StringFromCLSID(clsid,&wszCLSID);
    assert(SUCCEEDED(hr));
    size_t sz_converted; 
    errno_t err = wcstombs_s(&sz_converted, szCLSID, length, wszCLSID, length);
    assert(err == 0);
    CoTaskMemFree(wszCLSID);
}

BOOL CreateKey(const char* key, const char* subkey, const char* value){
    char szKey[1024];
    size_t dwSize = 1024;
    HKEY hKey;
    errno_t eResult = strcpy_s(szKey, dwSize, key);
    assert(eResult == 0);
    if(subkey != NULL){
        eResult = strcat_s(szKey, sizeof(szKey)/sizeof(char), "\\");
        assert(eResult == 0);
        eResult = strcat_s(szKey, sizeof(szKey)/sizeof(char), subkey);
        assert(eResult == 0);

    }
    LSTATUS lResult = RegCreateKeyExA(
        HKEY_CLASSES_ROOT,
        szKey,
        0,
        NULL,
        REG_OPTION_NON_VOLATILE,
        KEY_ALL_ACCESS,
        NULL,
        &hKey,
        NULL
    );  
    assert(lResult != 5);
    if(lResult != ERROR_SUCCESS){
        error_code = lResult;
        // char intStr[64];
        // _itoa(lResult, intStr, 10);
        ErrorExit(TEXT("RegCreateKeyExA"));
        return false;
    }

    if(value != NULL){
        lResult = RegSetValueExA(hKey,
                                        NULL,
                                        0,
                                        REG_SZ,
                                        (BYTE*) value,
                                        strlen(value) + 1
                                        );
        assert(lResult == ERROR_SUCCESS);
    }
    RegCloseKey(hKey);
    return true;
}


LONG RecursiveDeleteKey(HKEY hKey, const char* subkey){
    HKEY szKey;
    LSTATUS lResult = RegOpenKeyExA(hKey, subkey, 0, KEY_ALL_ACCESS, &szKey);
    if(lResult != ERROR_SUCCESS){
        return lResult;
    }
    char szSubkey[256];
    DWORD dwSize = 256;
    FILETIME* lpLastWriteTime = NULL;
    while(RegEnumKeyExA(szKey, 0, szSubkey,&dwSize,NULL,NULL,NULL,lpLastWriteTime) == ERROR_SUCCESS){
        lResult = RecursiveDeleteKey(szKey, szSubkey);
        if(lResult != ERROR_SUCCESS){
            RegCloseKey(szKey);
            return lResult;
        }
        dwSize = 256; 
    }
    RegCloseKey(szKey);
    return RegDeleteKeyA(hKey, subkey);
} 