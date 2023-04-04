#include<iostream>
#include<objbase.h>
using namespace std;

interface IX
{
        virtual void __stdcall Fx1() = 0;
        virtual void __stdcall Fx2() = 0;
};


interface IY
{
        virtual void __stdcall Fy1() = 0;
        virtual void __stdcall Fy2() = 0;
};

class CA: public IX, public IY
{
    public:
        virtual void Fx1(){
            cout << "Fx1 Called" << endl;
        }
        virtual void Fx2(){
            cout << "Fx2 Called" << endl;
        }

        virtual void Fy1(){
            cout << "Fy1 Called" << endl;
        }

        virtual void Fy2(){
            cout << "Fy2 Called" << endl;
        }
};

int main(){
    cout << "Program begins." << endl;
    cout << "Creating CA class instanance" << endl;
    CA* ca_inst = new CA();
    cout << "Interface IX" << endl;
    IX* ix_inst = ca_inst;
    cout << "Calling Fx1" << endl;
    ix_inst->Fx1();
    cout << "Calling Fx2" << endl;
    ix_inst->Fx2();

    cout << "Iterface IY" << endl;
    IY* iy_inst = ca_inst;
    cout << "Calling Fy1" << endl;
    iy_inst->Fy1();
    cout << "Calling Fy2" << endl;
    iy_inst->Fy2();

    cout << "Program Ends" << endl;
    return(0);
}








