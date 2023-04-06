#include<iostream>
#include<unknwn.h>
#include<winbase.h>
#include<WinUser.h>
#include<strsafe.h>
#include "create.h"

using namespace std;

typedef IUnknown* (*CREATEINST_FUNCPTR)();

IUnknown* CallCreateInstance(char* name){
    //Load Dynamic Link library
    // char* path = "D:/workspace/COM/build/debug/com_component/";
    LPCSTR name_ptr = name;
    HMODULE module = LoadLibraryA(name_ptr);
    if(module == NULL){
        cout << "CallCreateInstance:\tError: Cannot load component." << endl;
        return NULL ;
    }
    CREATEINST_FUNCPTR CreateInstance = (CREATEINST_FUNCPTR)GetProcAddress(module, "CreateInstance"); 
    if(CreateInstance == NULL){
        cout  << "CallCreateInstance:\tError: "
        << "Cannot find CreateInstance function."
        << endl ;
        cout << "Getting error returned by windows.\n";
        ErrorExit(TEXT("CreateInstance"));
        return NULL ;
    }

    return CreateInstance();
    
}

void ErrorExit(LPTSTR lpszFunction) 
{ 

    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError(); 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    // Display the error message and exit the process

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
        (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR)); 
    StringCchPrintf((LPTSTR)lpDisplayBuf, 
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"), 
        lpszFunction, dw, lpMsgBuf); 
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK); 
    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
    ExitProcess(dw); 
}