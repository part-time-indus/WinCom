#include<objbase.h>
extern "C" void CLSIDtoChar(const CLSID&, char*, int);
extern "C" BOOL CreateKey(const char*, const char*, const char*);
extern "C" LONG RecursiveDeleteKey(const char*, const char*);
extern "C" HRESULT RegisterDLL(HMODULE, const CLSID&, const char*, const char*, const char*);