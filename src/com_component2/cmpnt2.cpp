#include<iostream>
#include "registry.h"
#include "Iface.h"

static HMODULE g_hModule = NULL;
static long g_cComponents = 0 ; 
static long g_cServerlock = 0;


const char g_szFriendlyName[] = "Inside COM, Chapter 7 Example";
const char g_szVerIndProgId[] = "InsideCom.Ch07";
const char g_szProgId[] = "InsideCom.Ch07.1";


void trace(const char* msg){
    std::cout << msg << std::endl;
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

//m_cRef initialized to 1 instead of 0
//If QueryInterface call fails, AddReference call never occurs
//Release in CreateInterface will decrement m_cRef to zero 
//At the end of CreateInterface Object is still alive if QueryInterface doesnt fail

CA::CA():m_cRef(1){
    InterlockedIncrement(&g_cComponents);
}
CA::~CA(){
    trace("Component:\tDestroying Itself");
    InterlockedDecrement(&g_cComponents);
}

HRESULT __stdcall CA::QueryInterface(const IID& iid, void** ppv){

    if(iid == IID_IUnknown){
        *ppv = static_cast<IX*>(this);
    }else if(iid == IID_IX){
        trace("Component:\tReturn pointer to IX");
        *ppv = static_cast<IX*>(this);
    }else if(iid == IID_IY){
        trace("Component:\tReturn pointer to IY");
        *ppv = static_cast<IY*>(this);
    }else{
        *ppv = NULL;
        return E_NOINTERFACE;
    }
    static_cast<IUnknown*>(*ppv)->AddRef();
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
    std::cout << "Fx Called" << std::endl;
}

void CA::Fy(){
    std::cout << "Fy Called" << std::endl;
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

CFactory::CFactory():m_cRef(1){}
CFactory::~CFactory(){
    trace("Class factory:\t\tDestroy self.");
}

HRESULT __stdcall CFactory::QueryInterface(const IID& iid, void** ppv){
    if(iid == IID_IUnknown){
        *ppv = static_cast<IClassFactory*>(this);
    }else if(iid == IID_IClassFactory){
        *ppv = static_cast<IClassFactory*>(this);
    }else{
        *ppv = NULL;
        return E_NOINTERFACE;
    }
    static_cast<IUnknown*>(*ppv)->AddRef();
    return S_OK;
}

ULONG __stdcall CFactory::AddRef(){
    return InterlockedIncrement(&m_cRef);
}

ULONG __stdcall CFactory::Release(){
    InterlockedDecrement(&m_cRef);
    if(m_cRef == 0){
        delete this;
        return 0;
    }
    return m_cRef;
}

HRESULT __stdcall CFactory::CreateInstance(IUnknown* pUnkOuter, const IID& iid, void** ppv){
    trace("Class Factory:\t\tCreate Component");
    if(pUnkOuter != NULL){
        return CLASS_E_NOAGGREGATION;
    }
    CA* pA = new CA;
    if(pA == NULL){
        return E_OUTOFMEMORY;
    }
    HRESULT hr = pA->QueryInterface(iid, ppv);
    pA->Release();
    return hr;
}

HRESULT __stdcall CFactory::LockServer(BOOL bLock){
    if(bLock){
        InterlockedIncrement(&g_cServerlock);
    }else{
        InterlockedDecrement(&g_cServerlock);
    }

    return S_OK;

}

HRESULT DllCanUnloadNow(){
    if(g_cServerlock == 0 && g_cComponents == 0){
        return S_OK;
    }else{
        return S_FALSE;
    }
}

HRESULT DllGetClassObject(CLSID rclsid, IID riid, void** ppv){
    if(rclsid != CLSID_Component1){
        *ppv = NULL;
        return CLASS_E_CLASSNOTAVAILABLE;
    }
    HRESULT hr;
    CFactory* Cfactory = new CFactory;
    if(Cfactory == NULL){
        return E_OUTOFMEMORY;
    }   
    hr = Cfactory->QueryInterface(IID_IClassFactory, ppv);
    Cfactory->Release();
    return hr;
}


STDAPI DllRegisterServer(){
    return RegisterServer(g_hModule, CLSID_Component1, g_szFriendlyName, g_szVerIndProgId, g_szProgId);
}

STDAPI DllUnregisterServer(){
    //Its they value(filepath of component dll) for the inprocserver32 subkey for component CLSID
    return UnregisterServer(CLSID_Component1, g_szVerIndProgId, g_szProgId);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved){
    if(fdwReason == DLL_PROCESS_ATTACH){
        g_hModule = hinstDLL;
    }
    return true;
}