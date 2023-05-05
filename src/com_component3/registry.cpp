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
    size_t key_len = 1024;
    errno_t eResult = strcpy_s(szKey, key_len, key);
    assert(eResult == 0);
    eResult = strcat_s(szKey, key_len, "\\");
    assert(eResult == 0);
    if(subkey != NULL){
        eResult = strcat_s(szKey, 64, subkey);
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
