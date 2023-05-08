#include<objbase.h>
void CLSIDtoChar(const CLSID&, char*, int);
BOOL CreateKey(const char*, const char*, const char*);
LONG RecursiveDeleteKey(const char*, const char*);
extern "C" HRESULT RegisterDLL(HMODULE, const CLSID&, const char*, const char*, const char*);
extern "C" HRESULT UnregisterDLL(const CLSID&, const char*, const char*);