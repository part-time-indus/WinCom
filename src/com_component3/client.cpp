#include<iostream>
#include "registry.h";
int main(){
    const int szCLSIDlen = 39;
    char szCLSID[szCLSIDlen];
    CLSIDtoChar(CLSID_COMPONENT1, szCLSID, szCLSIDlen);
    std::cout << "CLSID char:\t" << szCLSID << std::endl; 
}