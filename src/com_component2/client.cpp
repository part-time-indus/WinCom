#include<objbase.h>
#include<iostream>
#include "Iface.h"
#include "assert.h"
using namespace std;

void trace(const char* msg){
    std::cout << msg << std::endl;
}

int main(){
    CoInitialize(NULL);
    HRESULT hr;
    IX* pIX = NULL;
    hr = CoCreateInstance(CLSID_Component1, NULL, CLSCTX_INPROC_SERVER, IID_IX, (void**) &pIX);
    if(SUCCEEDED(hr)){
        trace("Created object");
        pIX->Fx();
        pIX->Release();
    }
    CoUninitialize();
    return(0);

}