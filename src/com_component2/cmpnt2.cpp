#include<iostream>
#include<objbase.h>
#include "guids.cpp"


static long g_cComponents = 0 ; 

void trace(const char* msg){
    std::cout << msg << std::endl;
}

interface IX: public IUnknown{
    virtual void Fx() = 0;
};

interface IY: public IUnknown{
    virtual void Fy() = 0;

};


class CA: public IX, public IY{
    public:
        CA();
        ~CA();
        virtual HRESULT __stdcall QueryInterface(IID& iid, void** ppv);
        virtual ULONG __stdcall AddRef();
        virtual ULONG __stdcall Release();
        virtual void Fx();
        virtual void Fy();
    private:
        long m_cRef;
};

CA::CA():m_cRef(0){
    InterlockedIncrement(&g_cComponents);
}
CA::~CA(){
    trace("Component:\tDestroying Itself");
    InterlockedDecrement(&g_cComponents);
}

HRESULT __stdcall CA::QueryInterface(IID& iid, void** ppv){

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
    AddRef();
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
    return m_cRef

}

void CA::Fx(){
    std::cout << "Fx Called";
}

void CA::Fy(){
    std::cout << "Fy Called";
}


