#include<iostream>
#include<objbase.h>
#include<assert.h>


void trace(char* pMsg){
    std::cout << pMsg << std::endl;
}


extern "C" void CLSIDtoChar(const CLSID& clsid, char* szCLSID, int length){
    LPOLESTR wszCLSID;
    HRESULT hr = StringFromCLSID(clsid,&wszCLSID);
    assert(hr == S_OK);
    size_t sz_converted; 
    errno_t err = wcstombs_s(&sz_converted, szCLSID, length, wszCLSID, length);
    assert(err == 0);
    CoTaskMemFree(wszCLSID);
}

extern "C" BOOL CreateKey(const char* key, const char* subkey, const char* value){
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


extern "C" LONG RecursiveDeleteKey(HKEY hKey, const char* subkey){
    HKEY szKey;
    LSTATUS lResult = RegOpenKeyExA(hKey, subkey, 0, KEY_ALL_ACCESS, &szKey);
    if(lResult != ERROR_SUCCESS){
        return lResult;
    }
    char szSubkey[256];
    DWORD dwSize = 256;
    FILETIME* lpLastWriteTime;
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