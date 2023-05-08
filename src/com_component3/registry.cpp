#include<iostream>
#include<objbase.h>
#include<assert.h>

void CLSIDtoChar(const CLSID&, char*, int);
BOOL CreateKey(const char*, const char*, const char*);
LONG RecursiveDeleteKey(HKEY key, const char*);

const int CLSID_STRLEN = 39;

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
    LPOLESTR wszCLSID;
    HRESULT hr = StringFromCLSID(clsid,&wszCLSID);
    assert(hr == S_OK);
    size_t sz_converted; 
    errno_t err = wcstombs_s(&sz_converted, szCLSID, length, wszCLSID, length);
    assert(err == 0);
    CoTaskMemFree(wszCLSID);
}

BOOL CreateKey(const char* key, const char* subkey, const char* value){
    char szKey[1024];
    size_t dwSize = 1024;
    errno_t eResult = strcpy_s(szKey, dwSize, key);
    assert(eResult == 0);
    eResult = strcat_s(szKey, dwSize, "\\");
    assert(eResult == 0);
    if(subkey != NULL){
        eResult = strcat_s(szKey, 1024, subkey);
        assert(eResult == 0);

    }
    HKEY hKey;
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
    if(lResult != ERROR_SUCCESS){
        return false;
    }

    if(value != NULL){
        LSTATUS lResult = RegSetValueExA(hKey,
                                        NULL,
                                        0,
                                        REG_SZ,
                                        (BYTE*) value,
                                        strlen(value) + 1
                                        );
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