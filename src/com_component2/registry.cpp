#include<stdio.h>
#include<objbase.h>


void CLSIDtoChar(GUID, char*);
     //Add the component to the windows registry
//Remove the component from the windows registry


void CLSIDtoChar(GUID clsid, char* clsid_str){
    HRESULT hr;
    wchar_t clsid_wstr[39];
    hr = StringFromCLSID(clsid, (LPOLESTR* )clsid_wstr);
    if(SUCCEEDED(hr)){
        size_t converted;
        wcstombs_s(&converted, clsid_str, sizeof(clsid_str)/sizeof(char), (const wchar_t*) &clsid_wstr, sizeof(clsid_wstr)/sizeof(char));
    }
    CoTaskMemFree(clsid_wstr);

}

//  1. Convert CLSID to char string - CLSID is a struct
//This function will take a pointer to a char as an argument
//There are 38 characters in the resulting string + null terminator
//HRESULT StringFromCLSID(
//   [in]  REFCLSID rclsid,
//   [out] LPOLESTR *lplpsz
// );
// LPOESTR is wide char type, use wcstombs_s to convert to multibyte string UTF-8? 


//  2. Create a key and set its value
//RegCreateKeyEx function creates a key
//RegSetValue function sets key value


//  3. Delete a key and all its components
//RegEnumKeyEx enumerates subkeys of a key
//RegDeleteKeyEx deletes key and its values

