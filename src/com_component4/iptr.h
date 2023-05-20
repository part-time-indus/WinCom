#include<objbase.h>
template<class T, const IID* iid> 
class IPtr
{
    public:
        IPtr(){
            m_pI = NULL;
        }
        IPtr(T* pI){
            m_pI = pI;
            if(m_pI != NULL){
                m_pI->AddRef();
            }
        }
        IPtr(IUnknown* pI){
            if(pI != NULL){
                HRESULT hr = pI->QueryInterface(iid, (void**)&m_pI);
            }
        }

        
    private: 
        T* m_pI;


};