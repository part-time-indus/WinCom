#include<iostream>
#include<objbase.h>
#include "iface.h"

#define global static

global long g_cComponents = 0;
global long g_cServerLocks = 0;

void trace(char* pMsg){
    std::cout << pMsg << std::endl;
}

class CA: public IX, public IY{
    public:
        CA();
        ~CA();
        virtual HRESULT __stdcall QueryInterface(const IID& iid, void** ppv);
        virtual ULONG __stdcall AddRef();
        virtual ULONG __stdcall Release();
        virtual void Fx();
        virtual void Fy();
    private: 
        long m_cRef;
};

CA::CA(): m_cRef(1){
    InterlockedIncrement(&g_cComponents);

}

CA::~CA(){
    trace("Component:\tDestroying itself");
    InterlockedDecrement(&g_cComponents);
}
HRESULT __stdcall CA::QueryInterface(const IID& iid, void** ppv){
    trace("QueryInterface: Creating Interface");
    *ppv = NULL;
    if(iid == IID_IUnknown){
        *ppv = static_cast<IX*>(this);
    }else if(iid == IID_IX){
        trace("Component:\tReturn pointer to IX");
        *ppv = static_cast<IX*>(this);
    }else if(iid == IID_IY){
        trace("Component:\tReturn to pointer IY");
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



class CFactory: public IClassFactory{
    public:
        CFactory();
        ~CFactory();
        virtual HRESULT __stdcall QueryInterface(const IID& iid, void** ppv);
        virtual ULONG __stdcall AddRef();
        virtual ULONG __stdcall Release();
        virtual HRESULT __stdcall CreateInstance(IUnknown* pUnkOuter, const IID& iid, void** ppv);
        virtual HRESULT __stdcall LockServer(BOOL bLock);
    private:
        long m_cRef;
};

CFactory::CFactory():m_cRef(1){

}

CFactory::~CFactory(){
    trace("Class factory:\t\tDestroy self.");
}

HRESULT __stdcall CFactory::QueryInterface(const IID& iid, void** ppv){
    *ppv = NULL;
    if(iid == IID_IUnknown || iid == IID_IClassFactory){
        *ppv = static_cast<IClassFactory*>(this);
    }else{
        return E_NOINTERFACE;
    }
    static_cast<IUnknown*>(this)->AddRef();
}

ULONG __stdcall CFactory::AddRef(){
    return InterlockedIncrement(&m_cRef);
}

ULONG __stdcall CFactory::Release(){
    InterlockedDecrement(&m_cRef);
    if(m_cRef == 0){
        delete this;
        m_cRef = 0;
    }
    return m_cRef;
}

HRESULT __stdcall CFactory::CreateInstance(IUnknown* pUnkOuter, const IID& iid, void** ppv){
    trace("Class Factory:\t\tCreate Component");
    if(pUnkOuter != NULL){
        return CLASS_E_NOAGGREGATION;
    }
    CA* ca = new CA;
    if(ca == NULL){
        return E_OUTOFMEMORY;

    }
    HRESULT hr = ca->QueryInterface(iid, ppv);
    ca->Release();
    return hr;
}

HRESULT __stdcall CFactory::LockServer(BOOL bLock){
    if(bLock){
        InterlockedIncrement(&g_cServerLocks);
    }else{
        InterlockedDecrement(&g_cServerLocks);
    }
    return S_OK;
}
HRESULT DllGetClassObject(const CLSID& rclsid, const IID& riid, void** ppv){
    *ppv = NULL;
    if(rclsid != CLSID_COMPONENT1){
        return CLASS_E_CLASSNOTAVAILABLE;
    }
    CFactory* cF = new CFactory;
    if(cF == NULL){
        return E_OUTOFMEMORY;
    }
    HRESULT hr = cF->QueryInterface(IID_IClassFactory, ppv);
    cF->Release();
    return hr;
}