#include<objbase.h>
#include "guids.cpp"


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
        virtual HRESULT QueryInterface(IID& iid, void** ppv);
        virtual ULONG AddRef();
        virtual ULONG Release();
        virtual void Fx();
        virtual void Fy();
    private:
        int sm_ref;
};



