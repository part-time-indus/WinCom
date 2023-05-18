#include<iostream>
#include<objbase.h>
#include "iface.h"
#include "registry.h"

#define global static

global long g_cComponents = 0;
global long g_cServerLocks = 0;
global HMODULE hModule;



const char g_szFriendlyName[] = "Inside COM, Chapter 8 Example";
const char g_szVerIndProgId[] = "InsideCom.Ch08";
const char g_szProgId[] = "InsideCom.Ch08.1";

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
        HRESULT Init();
    private: 
        long m_cRef;
        IUnknown* m_pUnkInner;
        IZ* m_pIZ;
};

CA::CA(): m_cRef(1), m_pUnkInner(NULL){
    trace("Outer Component:\t\tCreating IUnknown Object");
    InterlockedIncrement(&g_cComponents);

}

CA::~CA(){
    trace("Outer Component:\t\tDestroying itself");
    //m_cRef = 0;
    InterlockedDecrement(&g_cComponents);
    //NOTE: Prevent recursive destruction on next AddRef/Release pair.
    //NOTE: Counter the pUnkOuter->Release in the init method.
    //NOTE: Removing following two statements, m_cRef sets to -1;
    //1:    m_cRef = 1
    IUnknown* pUnkOuter = static_cast<IX*>(this);
    pUnkOuter->AddRef();
    if(m_pIZ != NULL){
        //NOTE: 2: calls outer Release, sets m_cRef = 0;
        //NOTE: calls delete this again, which again sets m_cRef = 1 as above at 1
        //NOTE: which again sets m_cRef = 0 as above at 2
        //NOTE: This leads to recursive AddRef/Release;
        m_pIZ->Release();
    }
    if(m_pUnkInner != NULL){
        m_pUnkInner->Release();
    }
}
HRESULT CA::Init(){
    IUnknown* pUnkOuter = static_cast<IX*>(this);
    trace("Outer Component - Init:\t\tCreate inner component.");
    HRESULT hr = CoCreateInstance(CLSID_COMPONENT2, pUnkOuter, CLSCTX_INPROC_SERVER, IID_IUnknown, (void**) &m_pUnkInner);
    if(FAILED(hr)){
        trace("Outer Component - Init:\t\tCould not create contained component.");
        return E_FAIL;
    }
    hr = m_pUnkInner->QueryInterface(IID_IZ, (void**)&m_pIZ);
    if(FAILED(hr)){
        trace("Outer Component - Init:\t\tInner component does not suport the interface");
        m_pUnkInner->Release();
        m_pUnkInner = NULL;
        m_pIZ = NULL;
        return E_FAIL;
    }
    pUnkOuter->Release();
    return S_OK;
}
HRESULT __stdcall CA::QueryInterface(const IID& iid, void** ppv){
    trace("Outer Component - QueryInterface:\t\tQuerying Interface");
    *ppv = NULL;
    if(iid == IID_IUnknown){
        *ppv = static_cast<IX*>(this);
    }else if(iid == IID_IX){
        trace("Outer Component - QueryInterface:\t\tReturn pointer to IX");
        *ppv = static_cast<IX*>(this);
    }else if(iid == IID_IY){
        trace("Outer Component - QueryInterface:\t\tReturn pointer to IY");
        *ppv = static_cast<IY*>(this);
    }else if(iid == IID_IZ){
        trace("Outer Component - QueryInterface:\t\tReturn pointer to IZ");
#if 1
        return m_pUnkInner->QueryInterface(IID_IZ, ppv);
#else
        *ppv = m_pIZ;
#endif

    }else{
        return E_NOINTERFACE;
    }
    reinterpret_cast<IUnknown*>(*ppv)->AddRef();
    return S_OK;
}

ULONG __stdcall CA::AddRef(){
    trace("Outer Component - AddRef:\t\tIncrementing Ref Count");
    return InterlockedIncrement(&m_cRef);
}

ULONG __stdcall CA::Release(){
    trace("Outer Component - Release:\t\tDecrementing Ref Count");
    InterlockedDecrement(&m_cRef);
    if(m_cRef == 0){
        delete this;
        return 0;
    }
    return m_cRef;
}

void CA::Fx(){
    std::cout << "Fx called" << std::endl;
}

void CA::Fy(){
    std::cout << "Fy called" << std::endl;
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
    trace("Outer Component Factory:\t\tCreating IClassFactory Object.");

}

CFactory::~CFactory(){
    trace("Outer Component Factory:\t\tDestroy self.");
}

HRESULT __stdcall CFactory::QueryInterface(const IID& iid, void** ppv){
    trace("Outer Component Factory - QueryInterface:\t\tQuerying Interface.");
    *ppv = NULL;
    if(iid == IID_IUnknown || iid == IID_IClassFactory){
        trace("Outer Component Factory - QueryInterface:\t\tGetting pointer to IID_IClassFactory.");
        *ppv = static_cast<IClassFactory*>(this);
    }else{
        trace("Outer Component Factory - QueryInterface:\t\tInterface not found.");
        return E_NOINTERFACE;
    }
    static_cast<IUnknown*>(*ppv)->AddRef();
    return S_OK;
}

ULONG __stdcall CFactory::AddRef(){
    trace("Outer Component Factory - AddRef:\t\tIncrementing Reference Count.");

    return InterlockedIncrement(&m_cRef);
}

ULONG __stdcall CFactory::Release(){
        trace("Outer Component Factory - Release:\t\tDecrementing Reference Count.");

    InterlockedDecrement(&m_cRef);
    if(m_cRef == 0){
        delete this;
        m_cRef = 0;
    }
    return m_cRef;
}

HRESULT __stdcall CFactory::CreateInstance(IUnknown* pUnkOuter, const IID& iid, void** ppv){
    trace("Outer Component Factory - CreateInstance:\t\tCreating instance of the Component");
    if(pUnkOuter != NULL){
        return CLASS_E_NOAGGREGATION;
    }
    CA* ca = new CA;
    if(ca == NULL){
        trace("Outer Component Factory - CreateInstance:\t\tOut Of Memory");
        return E_OUTOFMEMORY;

    }
    HRESULT hr = ca->Init();
    if(FAILED(hr)){
        //NOTE: This is where the component gets deleted => m_cRef = 0;
        trace("Outer Component Factory - CreateInstance:\t\tInit Failed");
        ca->Release();
        return hr;
    }
    hr = ca->QueryInterface(iid, ppv);
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
HRESULT __stdcall DllGetClassObject(const CLSID& rclsid, const IID& riid, void** ppv){
    *ppv = NULL;
    if(rclsid != CLSID_COMPONENT1){
        return CLASS_E_CLASSNOTAVAILABLE;
    }
    CFactory* cF = new CFactory;
    if(cF == NULL){
        return E_OUTOFMEMORY;
    }
    HRESULT hr = cF->QueryInterface(riid, ppv);
    cF->Release();
    return hr;
}

HRESULT __stdcall DllCanUnloadNow(){
    if(g_cComponents == 0 && g_cServerLocks == 0){
        return S_OK;
    }
    return S_FALSE;
}

HRESULT __stdcall DllRegisterServer(){
    return RegisterDll(hModule, CLSID_COMPONENT1, g_szFriendlyName, g_szVerIndProgId, g_szProgId);

}

HRESULT __stdcall DllUnregisterServer(){
    return UnregisterDll(CLSID_COMPONENT1, g_szProgId, g_szVerIndProgId);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved){
    if(fdwReason == DLL_PROCESS_ATTACH){
        hModule = hinstDLL;
    }
    return true;

}