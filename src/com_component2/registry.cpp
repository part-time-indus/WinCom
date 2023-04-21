#include<stdio.h>
#include<objbase.h>
#include<assert.h>


void CLSIDtoChar(const CLSID&, char*);
BOOL CreateKey(const char*, const char*, const char*);
LONG RecursiveDeleteKey(HKEY, const char*);
//Add the component to the windows registry
//Remove the component from the windows registry


//Helper Functions to register and unregister server
void CLSIDtoChar(const CLSID& clsid, char* szCLSID){
    // assert(length >= 39);
    HRESULT hr;
    LPOLESTR wszCLSID = NULL;
    hr = StringFromCLSID(clsid, &wszCLSID);
    assert(SUCCEEDED(hr));
    size_t converted;
    errno_t wsz_to_sz = wcstombs_s(&converted, szCLSID, sizeof(szCLSID)/sizeof(char), (const wchar_t*) &wszCLSID, sizeof(wszCLSID)/sizeof(char));
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
            sizeof(szValue)/sizeof(BYTE)
        );
    }
    
    RegCloseKey(hKey);
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
    RegDeleteKeyA(parentKey, childKey);
}


//  3. Delete a key and all its components
//RegEnumKeyEx enumerates subkeys of a key
//RegDeleteKeyEx deletes key and its values

