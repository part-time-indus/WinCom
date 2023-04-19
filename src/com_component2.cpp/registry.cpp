     //Add the component to the windows registry
//Remove the component from the windows registry


//  1. Convert CLSID to char string - CLSID is a struct
//This function will take a pointer to a char as an argument
//There are 38 characters in the resulting string + null terminator
//HRESULT StringFromCLSID(
//   [in]  REFCLSID rclsid,
//   [out] LPOLESTR *lplpsz
// );
// LPOESTR is wide char type, use wcstombs_s to convert to multibyte string UTF-8? 

//  2. Create a key and set its value
//RegCreateKeyEx function creates a key
//RegSetValue function sets key value


//  3. Delete a key and all its components
//RegEnumKeyEx enumerates subkeys of a key
//RegDeleteKeyEx deletes key and its values

