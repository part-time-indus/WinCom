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
    int sz_converted; 
    errno_t err = wctomb_s(&sz_converted, szCLSID, length, *wszCLSID);
    assert(err == 0);
}