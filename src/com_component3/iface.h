#include<objbase.h>

interface IX: public IUnknown{
    virtual void Fx() = 0;
};

interface IY: public IUnknown{
    virtual void Fy() = 0;
};

interface IZ: public IUnknown{
    virtual void Fz() = 0;
};

extern "C" CLSID CLSID_COMPONENT1;
extern "C" CLSID CLSID_COMPONENT2;
extern "C" IID IID_IX;
extern "C" IID IID_IY;
extern "C" IID IID_IZ;