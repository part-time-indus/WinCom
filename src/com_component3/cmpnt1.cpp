#include<objbase.h>
#include "iface.h";


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

