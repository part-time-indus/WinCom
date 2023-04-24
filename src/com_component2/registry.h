#include<objbase.h>
void CLSIDtoChar(const CLSID&, char*, int);
BOOL CreateKey(const char*, const char*, const char*);
LONG RecursiveDeleteKey(HKEY, const char*);


HRESULT RegisterServer(HMODULE, CLSID&, const char*, const char*, const char*);
HRESULT UnregisterServer(CLSID&, const char*, const char*);