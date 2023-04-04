#include<iostream>
#include<objbase.h>

using namespace std;


//Interfaces
void trace(const char* pMsg){
    cout << pMsg << endl;
}

interface IX: public IUnknown{
    virtual void __stdcall Fx() = 0;
};

interface IY: public IUnknown{
    virtual void __stdcall Fy() = 0;
};

interface IZ: public IUnknown{
    virtual void __stdcall Fz() = 0; 
};

//GUID

extern const IID IID_IX;
extern const IID IID_IY;
extern const IID IID_IZ;

//Component

class CA: public IX, public IY
{
    virtual HRESULT __stdcall QueryInterface(const IID& iid, void** ppv);
    virtual ULONG __stdcall AddRef() { return 0; }
    virtual ULONG __stdcall Release() { return 0; }

    virtual void Fx() {cout << "Fx" << endl;}
    virtual void Fy() {cout << "Fy" << endl;}
};

HRESULT __stdcall CA::QueryInterface(const IID& iid, void** ppv){
    if(iid == IID_IUnknown){
        trace("Query Interface: Return pointer to IUnknown");
        *ppv = static_cast<IX*>(this);
    }else if(iid == IID_IX){
        trace("Query Interface: Return pointer to IX");
        *ppv = static_cast<IX*>(this);
    }else if(iid == IID_IY){
        trace("Query Interface: Return pointer to IY");
        *ppv = static_cast<IY*>(this); 
    }else{
        trace("Query interface: Not supported");
        *ppv = NULL;
        return E_NOINTERFACE;
    }
    static_cast<IUnknown*>(*ppv)->AddRef();
    return S_OK;
}

IUnknown* CreateInstance(){
    IUnknown* pI = static_cast<IX*>(new CA());
    pI->AddRef();
    return pI;

}

static const IID IID_IX = {0x32bb8320, 0xb41b, 0x11cf, 
                        {0xa6, 0xbb, 0x0, 0x80, 0xc7, 0xb2, 0xd6, 0x82}};
static const IID IID_IY = {0x32bb8321, 0xb41b, 0x11cf, 
                        {0xa6, 0xbb, 0x0, 0x80, 0xc7, 0xb2, 0xd6, 0x82}};
static const IID IID_IZ = {0x32bb8322, 0xb41b, 0x11cf, 
                        {0xa6, 0xbb, 0x0, 0x80, 0xc7, 0xb2, 0xd6, 0x82}};                        

//Client

int main(){
    HRESULT hr;
    trace("Client:  Get an IUnknown pointer.");
    IUnknown* pIUnknown = CreateInstance();

    trace("Client:  Get interface IX");

    IX* pIX = NULL;
    hr = pIUnknown->QueryInterface(IID_IX, (void**) &pIX);
    if(SUCCEEDED(hr)){
        trace("Client:  Succeeded getting IX.");
        pIX->Fx();
    }
    trace("Client:  Get interface IY"); 
    IY* pIY = NULL;
    hr = pIUnknown->QueryInterface(IID_IY, (void**) &pIY);
    if(SUCCEEDED(hr)){
        trace("Client: Succeeded getting IY.");
        pIY->Fy();
    }

    trace("Client:  Asked for unsupported interface");
    IZ* pIZ= NULL;
    hr = pIUnknown->QueryInterface(IID_IZ, (void**) &pIZ);
    if(SUCCEEDED(hr)){
        trace("Client:  Succeeded in getting interface IZ");
        pIZ->Fz();
    }else{
        trace("Client:  Could not get interface IZ.");
    }

    trace("Client:  Interface IY from interface IX");

    IY* pIYfromIX = NULL;
    hr = pIX->QueryInterface(IID_IY, (void**) &pIYfromIX);
    if(SUCCEEDED(hr)){
        trace("Client:  Succeded in getting interface IY from Interface IX");
        pIYfromIX->Fy();
    }

    trace("Client: Get interface IUnknown from IY");

    IUnknown* pIUnknownFromIY = NULL;

    hr = pIY->QueryInterface(IID_IUnknown, (void**) &pIUnknownFromIY);
    if(SUCCEEDED(hr)){
        cout << "Are the IUnknown pointer equal? " << endl;
        if(pIUnknownFromIY == pIUnknown){
            cout << "pIUnknownFromIY:   " << pIUnknownFromIY << endl;
            cout << "pIUnknown:    " << pIUnknown << endl;
            cout << "Yes, pIUnknownFromIY == pIUnknown." << endl;
        }else{
            cout << "No. pIUnknownFromIY != pIUnknown." << endl; 
        }
    }
    delete pIUnknown;
    return(0);
}