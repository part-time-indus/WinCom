
#ifndef __CUnknown_h__
#define __CUnknown_h__

#include<objbase.h>

interface IDelegatingUnknown{
    virtual HRESULT STDMETHODCALLTYPE IDeleglatingUnknownQueryInterface(const IID& riid, void** ppv) = 0;
    virtual ULONG STDMETHODCALLTYPE IDeleatingUnknownAddRef();
    virtual ULONG STDMETHODCALLTYPE IDelegatingUnknownRelease();
};

class CUnknown: public IDelegatingUnknown
{
    public:
        virtual HRESULT STDMETHODCALLTYPE IDelgatingUnknownQueryInterface(const IID& riid, void** ppv);
        virtual ULONG STDMETHODCALLTYPE IDelegatingUnknownAddRef();
        virtual ULONG STDMETHODCALLTYPE IDelegatingUnknownRelease();
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



#endif