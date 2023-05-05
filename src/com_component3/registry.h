#include<objbase.h>
extern "C" void CLSIDtoChar(const CLSID&, char*, int);
extern "C" HRESULT CreateKey(const char*, const char*, const char*);