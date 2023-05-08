#include<iostream>
#include<objbase.h>
#include "iface.h"

void trace(char* pMsg){
    std::cout << pMsg << std::endl;
}

class CA: public IX, public IY{
    public:
        CA();
        ~CA();
        virtual HRESULT __stdcall QueryInterface(const IID& iid, void** ppv) = 0;
        virtual ULONG __stdcall AddRef() = 0;
        virtual ULONG __stdcall Release() = 0;
        virtual void Fx() = 0;
        virtual void Fy() = 0;
    private: 
        long m_cRef;
};

CA::CA(): m_cRef(0){

}

CA::~CA(){
    trace("Destroying itself");
}
HRESULT __stdcall CA::QueryInterface(const IID& iid, void** ppv){
    trace("QueryInterface: Creating Interface");
    *ppv = NULL;
    if(iid == IID_IUnknown){
        *ppv = static_cast<IX*>(this);
    }else if(iid == IID_IX){
        *ppv = static_cast<IX*>(this);
    }else if(iid == IID_IY){
        *ppv = static_cast<IY*>(this);
    }else{
        return E_NOINTERFACE;
    }
    reinterpret_cast<IUnknown*>(this)->AddRef();
    return S_OK;
}

ULONG __stdcall CA::AddRef(){
    return InterlockedIncrement(&m_cRef);
}

ULONG __stdcall CA::Release(){
    InterlockedDecrement(&m_cRef);
    if(m_cRef == 0){
        delete this;
        return 0;
    }
    return m_cRef;
}

void CA::Fx(){
    std::cout << "Fx called";
}

void CA::Fy(){
    std::cout << "Fy called";
}


