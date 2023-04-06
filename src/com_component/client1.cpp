#include<iostream>
#include<objbase.h>

#include"create.h"
#include"Iface.h"


using namespace std;

void trace(const char* pMsg){
    cout << pMsg << endl;
}

int main(){

    HRESULT hr;
    char name[40];
    cout << "Enter the filename of a component to use [cmpnt?.dll]\n";
    cin >> name;
    cout << endl;

    trace("Client:  Get an IUnknown pointer.");
    IUnknown* pIUnknown = CallCreateInstance(name);
    if(pIUnknown == NULL){
        trace("CallCreateInstance Failed.");
        return 1;
    }
    trace("Client:  Getting interface IX");
    IX* pIX = NULL;
    hr = pIUnknown->QueryInterface(IID_IX, (void**)&pIX);
    if(SUCCEEDED(hr)){
        trace("Client:  Succeeded getting IX");
        pIX->Fx();
        pIX->Release();
    }else{
        trace("Client:  Could not get interface IX");
    }

    trace("Client:  Release IUnknown interface.");
    pIUnknown->Release();
    return(0);


}


