#include<objbase.h>
void CLSIDtoChar(const CLSID&, char*, int);
BOOL CreateKey(const char*, const char*, const char*);
LONG RecursiveDeleteKey(HKEY, const char*);


HRESULT RegisterServer(HMODULE, const CLSID&, const char*, const char*, const char*);
HRESULT UnregisterServer(const CLSID&, const char*, const char*);