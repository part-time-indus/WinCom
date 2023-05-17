#include<iostream>
#include "iface.h"
#include "registry.h"
#include <objbase.h>


using namespace std;

void trace(const char* msg) { cout << "Client: \t" << msg << endl ;}

//
// main function
//
int main()
{
	// Initialize COM Library
	CoInitializeEx(NULL,COINIT_APARTMENTTHREADED) ;
	
	trace("Get interface IY from Component 1.") ;
	IY* pIY = NULL ; 
	HRESULT hr = ::CoCreateInstance(CLSID_COMPONENT1,
	                                NULL, 
	                                CLSCTX_INPROC_SERVER,
	                                IID_IY, 
	                                (void**)&pIY) ;
	if (SUCCEEDED(hr))
	{
	   IZ* pIZ = NULL;
	   pIY->Fy();
	   hr = pIY->QueryInterface(IID_IZ, (void**)&pIZ);
	   if(SUCCEEDED(hr)){
	   	pIZ->Fz();
		IX* pIX = NULL;
		hr = pIZ->QueryInterface(IID_IX, (void**)&pIX);
		if(SUCCEEDED(hr)){
			pIX->Fx();
	   		pIX->Release();
		}
		pIZ->Release();
	   }


	}
	else
	{
		cout << "Could not create component: " << hex << hr << endl ;
	}
	pIY->Release();
	
	CoUninitialize();
	std::cout << "Exiting Main";
	return 0 ;
}
