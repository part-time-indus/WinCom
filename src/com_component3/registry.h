#include<objbase.h>
extern "C" HRESULT RegisterDll(HMODULE, const CLSID&, const char*, const char*, const char*);
extern "C" HRESULT UnregisterDll(const CLSID&, const char*, const char*);