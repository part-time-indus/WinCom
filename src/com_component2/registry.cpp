#include<assert.h>
#include "Registry.h"




const int CLSID_STRING_SIZE = 39 ;

void CLSIDtoChar(const CLSID&, char*, int);
BOOL CreateKey(const char*, const char*, const char*);
LONG RecursiveDeleteKey(HKEY key, const char*);
void ErrorExit(LPTSTR);


HRESULT RegisterServer(HMODULE hModule, const CLSID& clsid, const char* szKeyFriendlyName, const char* szVerIndProgId, const char* szProgId){

    //Get server location
    char szModule[512];
    DWORD nSize = 512;
    DWORD lResult = GetModuleFileNameA(hModule,szModule,nSize);

    assert(lResult != 0);

    //Convert clsid to char

    char szCLSID[CLSID_STRING_SIZE];
    CLSIDtoChar(clsid, szCLSID, CLSID_STRING_SIZE);

    //Build the key "CLSID\\"
    char szKey[64];
    size_t szBuf = 64;
    errno_t sResult = strcpy_s(szKey,szBuf,"CLSID\\");

    assert(sResult == 0);
    sResult = strcat_s(szKey,szBuf,szCLSID);
    assert(sResult == 0);

    //Add CLSID to the registry
    CreateKey(szKey, NULL, szKeyFriendlyName);

    //Add the server filename subkey under CLSID
    CreateKey(szKey, "InprocServer32", szModule);

    //Add ProgId subkey under CLSID
    CreateKey(szKey, "ProgId", szProgId);

    //Add Version Independent ProgId subkey under CLSID
    CreateKey(szKey, "VersionIndependentProgId", szVerIndProgId);

    //Add version Independent ProgId subkey under HKEY_CLASSES_ROOT
    CreateKey(szVerIndProgId, NULL, szKeyFriendlyName);
    CreateKey(szVerIndProgId, "CLSID", szCLSID);
    CreateKey(szVerIndProgId, "CurVer", szProgId);

    //Add versioned ProgId subkey under HKEY_CLASSES_ROOT
    CreateKey(szProgId,NULL, szKeyFriendlyName);
    CreateKey(szProgId,"CLSID", szCLSID);
    return S_OK;
}

LONG UnregisterServer(const CLSID& clsid, const char* szVerIndProgId, const char* szProgId){
    char szCLSID[CLSID_STRING_SIZE];
    char szKey[64];
    long szBuf = 64;
    HKEY parentKey = HKEY_CLASSES_ROOT;
    CLSIDtoChar(clsid, szCLSID, CLSID_STRING_SIZE);
    errno_t sResult = strcpy_s(szKey,szBuf,"CLSID\\");
    assert(sResult == 0);
    sResult = strcat_s(szKey,szBuf,szCLSID);
    assert(sResult == 0);

    LONG lResult = RecursiveDeleteKey(parentKey, szCLSID);
    assert(lResult == ERROR_SUCCESS || lResult == ERROR_FILE_NOT_FOUND);
    RecursiveDeleteKey(parentKey, szVerIndProgId);
    assert(lResult == ERROR_SUCCESS || lResult == ERROR_FILE_NOT_FOUND);
    RecursiveDeleteKey(parentKey, szProgId);
    assert(lResult == ERROR_SUCCESS || lResult == ERROR_FILE_NOT_FOUND);
    return S_OK;

}




//Helper Functions to register and unregister server
void CLSIDtoChar(const CLSID& clsid, char* szCLSID, int length){
    assert(length >= 39);
    HRESULT hr;
    LPOLESTR wszCLSID = NULL;
    hr = StringFromCLSID(clsid, &wszCLSID);
    assert(SUCCEEDED(hr));
    size_t converted;
    //sizeof only works for char[] and not char*
    //Can strlen() be used for char*?
    errno_t wsz_to_sz = wcstombs_s(&converted, szCLSID, length, wszCLSID, length);
    assert(wsz_to_sz==0);
    CoTaskMemFree(wszCLSID);

}


BOOL CreateKey(const char* szKey, const char* szSubKey, const char* szValue){
    HKEY hKey;
    char szKeyBuf[1024];
    HKEY parent = HKEY_CLASSES_ROOT;
    errno_t cpyResult = strcpy_s(szKeyBuf, sizeof(szKeyBuf)/sizeof(char), szKey);
    assert(cpyResult == 0);
    if(szSubKey != NULL){
        errno_t catResult;
        catResult = strcat_s(szKeyBuf, sizeof(szKeyBuf)/sizeof(char), "\\");
        assert(catResult == 0);
        catResult = strcat_s(szKeyBuf, sizeof(szKeyBuf)/sizeof(char), szSubKey);
        assert(catResult == 0);
    }
    LSTATUS lResult = RegCreateKeyExA(
        parent,
        szKeyBuf,
        0,
        NULL,
        REG_OPTION_NON_VOLATILE,
        KEY_ALL_ACCESS,
        NULL,
        &hKey,
        NULL
    );
    assert(lResult == ERROR_SUCCESS);

    if(lResult != ERROR_SUCCESS){
        return false;
    }

    if(szValue != NULL){
        RegSetValueExA(
            hKey,
            NULL,
            0,
            REG_SZ,
            (BYTE*) szValue,
            strlen(szValue) + 1
        );
    }
    
    RegCloseKey(hKey);
    return true;
}
 
LONG RecursiveDeleteKey(HKEY parentKey, const char* childKey){
    DWORD subkeyBufSize = 256;
    char subKey[256];
    FILETIME time;
    HKEY hKey;

    LSTATUS lResult = RegOpenKeyExA(parentKey,childKey,0,KEY_ALL_ACCESS,&hKey);
    if(lResult != ERROR_SUCCESS){
        return lResult;
    }
    while(RegEnumKeyExA(hKey,0,subKey,&subkeyBufSize,NULL,NULL,NULL,&time) == ERROR_SUCCESS){
        lResult = RecursiveDeleteKey(hKey, subKey);
        if(lResult != ERROR_SUCCESS){
            RegCloseKey(hKey);
            return lResult;
        }
        RegDeleteKeyA(hKey, subKey);
        subkeyBufSize = 256;

    }
    RegCloseKey(hKey);
    return RegDeleteKeyA(parentKey, childKey);
}


