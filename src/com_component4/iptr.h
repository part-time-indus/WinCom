#include<objbase.h>
template<class T, const IID* piid> 
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
        //NOTE: Don't use <IUnknown> to construct the Object
        //NOTE: Will result in ambiguity between these two constructors
        IPtr(IUnknown* pI){
            if(pI != NULL){
                HRESULT hr = pI->QueryInterface(*piid, (void**)&m_pI);
            }
        }
        ~IPtr(){
            Release();
        }
        Release(){
            if(m_pI != NULL){
                T* pOld = m_pI;
                m_pI = NULL;
                pOld->Release();
            }
        }

        operator T*(){return m_pI};

        T& operator *(){(assert m_pI != NULL); return *m_pI;}
        T** operator &(){(assert m_pI != NULL); return &m_pI;}
        T* operator ->(){(assert m_pI != NULL); return m_pI;}

        T* operator =(T* pI){
            if(m_pI != pI){
                T* pOld = m_pI;
                m_pI = pI;
                if(m_pI != NULL){
                    m_pI->AddRef();
                }
                if(pOld != NULL){
                    pOld->Release();
                }
            }
            return m_pI;
        }

        T* operator=(IUnknown* pI){
            T* pOld = m_pI;
            if(pI != NULL){
                HRESULT hr = m_pI>QueryInterface(*piid, (void**)&m_pI);
                assert(SUCCEEDED(hr));
            }
            if(pOld != NULL){
                pOld->Release();
            }
            return m_pI;
        }

        BOOL operator !() {return m_pI == NULL ? TRUE: FALSE;}
        //NOTE: Usage => if(iPterObj)
        operator BOOL() const {return m_pI != NULL ? TRUE: FALSE;}

        const IID& iid(){return *piid;}
        
        
    private: 
        T* m_pI;

};

class IUnknownPtr{
    public:
        IUnknownPtr(){
            m_pI = NULL;
        }

        IUnknownPtr(IUnknown* pI){
            if(pI != NULL){
                m_pI = pI;
                m_pI->AddRef();
            }
        }
        ~IUnknownPtr(){
            Release();
        }
        void Release(){
            if(m_pI != NULL){
                IUnknown* pOld = m_pI;
                m_pI = NULL;
                pOld->Release();
            }
        }

        operator IUnknown*() {return m_pI;}

        IUnknown& operator * () {return *m_pI;}
        IUnknown** operator & () {return &m_pI;}
        IUnknown* operator -> () {return m_pI; }

        IUnknown* operator = (IUnknown* pI) {
            IUnknown* pOld = m_pI;
            m_pI = pI;
            if(m_pI != NULL){
                m_pI->AddRef();
            }
            if(pOld != NULL){
                pOld->Release();
            }
            return m_pI;
        }
        BOOL operator ! () {return m_pI == NULL ? TRUE : FALSE;}
        operator BOOL () {return m_pI != NULL ? TRUE : FALSE;}


    private:
        IUnknown* m_pI;

};