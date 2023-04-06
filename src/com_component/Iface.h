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

extern "C"{
    extern const IID IID_IX;
    extern const IID IID_IY;
    extern const IID IID_IZ;  

}