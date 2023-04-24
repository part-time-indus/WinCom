#include<objbase.h>
#include<iostream>
#include "Iface.h"

errno_t ConvertCLSID(CLSID&, char*, int);

void trace(const char* msg){
    std::cout << msg << std::endl;
}

errno_t ConvertCLSID(const CLSID& clsid, char* szCLSID, int length){
    HRESULT hr;
    LPOLESTR wszCLSID = NULL;
    errno_t wsz_to_sz;
    hr = StringFromCLSID(clsid, &wszCLSID);
    // assert(SUCCEEDED(hr));
    if(SUCCEEDED(hr)){
        size_t converted;
        wsz_to_sz = wcstombs_s(&converted, szCLSID, length, wszCLSID, length);
        std::cout <<"Bytes in szCLSID:\t" << sizeof(szCLSID) << std::endl;
        std::cout << "Wide char string\t" << wszCLSID << std::endl;
        std::cout << "Size of wide\t" << wcslen(wszCLSID) << std::endl;
        std::cout<< "Number of characters converted:\t" << converted << std::endl;
    }

    CoTaskMemFree(wszCLSID);
    return wsz_to_sz;

}
int main(){
    char szClsid[39];
    errno_t  err = ConvertCLSID(CLSID_Component1, szClsid, 39);
    std::cout << szClsid << std::endl;

}