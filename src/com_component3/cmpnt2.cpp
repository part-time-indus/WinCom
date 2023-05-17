#include<iostream>
#include<objbase.h>
#include "iface.h"
#include "registry.h"

#define global static;

global long g_bComponents = 0;
global long g_bServerLocks = 0;
global HMODULE g_bModule;
const char g_szbFriendlyName[] = "Inside COM, Chapter 8 Example 2";
const char g_szbVerIndProgId[] = "InsideCom.Ex2.Ch08";
const char g_szbProgId[] = "InsideCom.Ex2.Ch08.1";

void trace(char* pMsg){
    std::cout << pMsg << std::endl;
}

interface INonDelegatingUnknown{
    virtual HRESULT __stdcall INonDelegatingQueryInterface(const IID& riid, void** ppv) = 0;
    virtual ULONG __stdcall INonDelegatingAddRef() = 0;
    virtual ULONG __stdcall INonDelegatingRelease() = 0;
};


class BA: public IZ, public INonDelegatingUnknown
{
    public:
        BA(IUnknown* pUnkOuter);
        ~BA();
        virtual HRESULT __stdcall QueryInterface(const IID& iid, void** ppv);
        virtual ULONG __stdcall AddRef();
        virtual ULONG __stdcall Release();

        virtual HRESULT __stdcall INonDelegatingQueryInterface(const IID& riid, void** ppv);
        virtual ULONG __stdcall INonDelegatingAddRef();
        virtual ULONG __stdcall INonDelegatingRelease();
        virtual void Fz();
    private:
        long m_cRef;
        IUnknown* m_pUnkOuter;
};

BA::BA(IUnknown* pUnkOuter):m_cRef(1)
{
    trace("Inner Component:\t\tCreating IUnknown Object");
    InterlockedIncrement(&g_bComponents);
    if(pUnkOuter == NULL){
        trace("Inner Component (Not aggregating):\t\tDelegate to NonDelegating IUnknown");
        m_pUnkOuter = reinterpret_cast<IUnknown*>(static_cast<INonDelegatingUnknown*>(this));
    }else{
        trace("Inner Component (Aggregating):\t\tDelegate to Outer Unknown");
        m_pUnkOuter = pUnkOuter; 
    }

}

BA::~BA(){
    trace("Inner Component:\t\tDestroying Itself");
    InterlockedDecrement(&g_bComponents);
}
HRESULT __stdcall BA::INonDelegatingQueryInterface(const IID& riid, void** ppv){
    trace("Inner Component - INonDelegatingQueryInterface:\t\tQuerying Interface");
    *ppv = NULL;
    if(riid == IID_IUnknown){
        trace("Inner Component - INonDelegatingQueryInterface:\t\tGetting pointer to Interface INonDelegatingUnknown");
       *ppv = static_cast<INonDelegatingUnknown*>(this);
    }else if(riid == IID_IZ){
        trace("Inner Component - INonDelegatingQueryInterface:\t\tGetting pointer to interface IZ");
        *ppv = static_cast<IZ*>(this);
    }else{
        trace("Inner Component - INonDelegatingQueryInterface:\t\tInterface not found");
        return E_NOINTERFACE;
    }
    reinterpret_cast<IUnknown*>(*ppv)->AddRef();
    return S_OK;
}

ULONG __stdcall BA::INonDelegatingAddRef(){
    trace("Inner Component - INonDelegatingAddRef:\t\tIncrementing INonDelegatingUnknown reference count");

    return InterlockedIncrement(&m_cRef);
}

ULONG __stdcall BA::INonDelegatingRelease(){
    trace("Inner Component - INonDelegatingRelease:\t\tDecrementing INonDelegatingUnknown reference count");
    InterlockedDecrement(&m_cRef);
    if(m_cRef == 0){
        delete this;
        return 0;
    }
    return m_cRef;
}




HRESULT __stdcall BA::QueryInterface(const IID& riid, void** ppv){
    trace("Inner Component - QueryInterface: Delegating to QueryInterface of outer component");
    return m_pUnkOuter->QueryInterface(riid, ppv);
}

ULONG __stdcall BA::AddRef(){
    trace("Inner Component - AddRef:\t\tDelegating to AddRef of outer component.");
    return m_pUnkOuter->AddRef();
}

ULONG __stdcall BA::Release(){
    trace("Inner Component - Release:\t\tDelegating to Release of the outer component.");
    return m_pUnkOuter->Release();
}

void BA::Fz(){
    std::cout << "Fz called" << std::endl;
}


class BFactory: public IClassFactory
{
    public:
        BFactory();
        ~BFactory();
        virtual HRESULT __stdcall QueryInterface(const IID& riid, void** ppv);
        virtual ULONG __stdcall AddRef();
        virtual ULONG __stdcall Release();
        virtual HRESULT __stdcall CreateInstance(IUnknown* pUnkOuter, const IID& riid, void** ppv);
        virtual HRESULT __stdcall LockServer(BOOL bLock);
    private:
        long m_cRef;
};


BFactory::BFactory(): m_cRef(1)
{
        trace("Inner Component Factory:\t\tCreating IClassFactory Object");

}

BFactory::~BFactory()
{
    trace("Inner Component Factory:\t\tDestroying self");
}

HRESULT __stdcall BFactory::QueryInterface(const IID& riid, void** ppv){
    trace("Inner Component Factory - QueryInterface:\tQuerying Interface");
    *ppv = NULL;
    if(riid == IID_IUnknown || riid == IID_IClassFactory){
        trace("Inner Component Factory - QueryInterface:\tGetting pointer to interface IID_IClassFactory");
        *ppv = static_cast<IClassFactory*>(this);
    }else{
        trace("Inner Component Factory - QueryInterface:\t\tInterface not found");
        return E_NOINTERFACE;
    }
    reinterpret_cast<IUnknown*>(*ppv)->AddRef();
    return S_OK;
}

ULONG __stdcall BFactory::AddRef(){
    trace("Inner Component Factory - AddRef:\t\tIncrementing factory reference count");
    return InterlockedIncrement(&m_cRef);
}

ULONG __stdcall BFactory::Release(){
    trace("Inner Component Factory - Release:\t\tDecrementing factory reference count");
    InterlockedDecrement(&m_cRef);
    if(m_cRef == 0){
        delete this;
        return 0;
    }
    return m_cRef;
}

HRESULT __stdcall BFactory::CreateInstance(IUnknown* pUnkOuter, const IID& riid, void** ppv){
    trace("Inner Component Factory - CreateInstance:\t\tCreating instance of the component");
    if(pUnkOuter != NULL && riid != IID_IUnknown){
        return CLASS_E_NOAGGREGATION;
    }
    BA* ba = new BA(pUnkOuter);
    if(ba == NULL){
        return E_OUTOFMEMORY;
    }
    HRESULT hr = ba->INonDelegatingQueryInterface(riid, ppv);
    ba->INonDelegatingRelease();
    return hr;
}

HRESULT __stdcall BFactory::LockServer(BOOL bLock){
    if(bLock){
        InterlockedIncrement(&g_bServerLocks);

    }else{
        InterlockedDecrement(&g_bServerLocks);
    }
    return S_OK;
}


HRESULT __stdcall DllGetClassObject(const CLSID& rclsid, const IID& riid, void** ppv){
    if(rclsid != CLSID_COMPONENT2){
        return CLASS_E_CLASSNOTAVAILABLE;
    }
    BFactory* bFactory = new BFactory;

    if(bFactory == NULL){
        return E_OUTOFMEMORY;
    }

    HRESULT hr = bFactory->QueryInterface(riid, ppv);
    bFactory->Release();
    return hr;
}

HRESULT __stdcall DllCanUnloadNow(){
    if(g_bComponents == 0 && g_bServerLocks == 0){
        return S_OK;
    }
    return S_FALSE;
}

HRESULT DllRegisterServer(){
    return RegisterDll(g_bModule, CLSID_COMPONENT2, g_szbFriendlyName, g_szbVerIndProgId, g_szbProgId);
}

HRESULT DllUnregisterServer(){
    return UnregisterDll(CLSID_COMPONENT2, g_szbProgId, g_szbVerIndProgId);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved){
    if(fdwReason == DLL_PROCESS_ATTACH){
        g_bModule = hinstDLL;
    }
    return true;
}