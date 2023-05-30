
#ifndef __CUnknown_h__
#define __CUnknown_h__

#include<objbase.h>

interface INonDelegatingUnknown{
    virtual HRESULT STDMETHODCALLTYPE INonDeleglatingQueryInterface(const IID& riid, void** ppv) = 0;
    virtual ULONG STDMETHODCALLTYPE INonDeleatingAddRef();
    virtual ULONG STDMETHODCALLTYPE INonDelegatingRelease();
};

class CUnknown: public INonDelegatingUnknown
{
    public:
        virtual HRESULT STDMETHODCALLTYPE INonDelgatingQueryInterface(const IID& riid, void** ppv);
        virtual ULONG STDMETHODCALLTYPE INonDelegatingAddRef();
        virtual ULONG STDMETHODCALLTYPE INonDelegatingRelease();
        CUnknown(IUnknown* pUnkOuter);
        virtual ~CUnknown();
        virtual HRESULT Init(){
            return S_OK;
        }
        virtual void FinalRelease();
        static long ActiveComponents(){
            return m_ActiveComponents;
        }
        HRESULT FinishQI(IUnknown* pI, void** ppv);
    protected:
        IUnknown* GetOuterComponent(){
            return m_pUnkOuter;
        }
    private:
        long m_cRef;
        IUnknown* m_pUnkOuter;
        static long m_ActiveComponents;

};


#define DECLARE_IUNKNOWN                                                        \
virtual HRESULT STDMETHODCALLTYPE QueryInterface(const IID& riid, void** ppv)   \
{                                                                               \
    return GetOuterComponent()->QueryInterface(riid, ppv);                      \
};                                                                              \
virtual ULONG STDMETHODCALLTYPE AddRef()                                        \
{                                                                               \
    return GetOuterComponent()->AddRef();                                       \
};                                                                              \
virtual ULONG STDMETHODCALLTYPE Release()                                       \
{                                                                               \
    return GetOuterComponent()->ReleasE();                                      \
};                                                                               \    
            
#endif                                                                          