#include<iostream>
#include<objbase.h>
#include "Iface.h"

using namespace std;

void trace(const char* pMsg){
    cout << pMsg << endl;
}

class CA: public IX, public IY{
    virtual HRESULT __stdcall QueryInterface(const IID& iid, void** ppv);
    virtual ULONG __stdcall AddRef();
    virtual ULONG __stdcall Release();
    virtual void Fx();
    virtual void Fy();
    public:
        CA();
        ~CA();
    private:
        long m_CRef;
};


CA::CA(): m_CRef(0){}

CA::~CA(){
    trace("Destroy Self.");
}

HRESULT __stdcall CA::QueryInterface(const IID& iid, void** ppv){
    if(iid == IID_IUnknown){
        trace("QueryInterface:  Return pointer to IUnknown");
        *ppv = static_cast<IX*>(this);

    }else if(iid == IID_IX){
        trace("QueryInterface:  Return pointer to IX");
        *ppv = static_cast<IX*>(this);

    }else if(iid == IID_IY){
        trace("QueryInterface:  Return pointer to IY");
        *ppv = static_cast<IY*>(this);
    }else{
        trace("QueryInterface:  Not supported");
        *ppv = NULL;
        return E_NOINTERFACE;
    }

    trace("QueryInterface:  Incrementing Reference Count");
    reinterpret_cast<IUnknown*>(*ppv)->AddRef();
    return S_OK;
}

ULONG __stdcall CA::AddRef(){
    return InterlockedIncrement(&m_CRef);
}

ULONG __stdcall CA::Release(){
    if(InterlockedDecrement(&m_CRef) == 0){
        delete this;
        return 0;
    }
    return m_CRef;
}

void CA::Fx(){
    cout << "Fx Called.\n";
}

void CA::Fy(){
    cout << "Fy called.\n";
}

extern "C" IUnknown* CreateInstance()
{
    IUnknown* i_unkn = static_cast<IX*>(new CA());
    i_unkn->AddRef();
    return i_unkn;
}