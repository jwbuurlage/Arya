#pragma once

//The singleton MUST be made with the create method

namespace Arya
{
    template <typename T> class Singleton{
        private:
            //Private copy constructor and assignment to prevent copies
            Singleton(const Singleton<T>&);
            Singleton& operator=(const Singleton<T>&);

        protected:
            static T* singleton;
            Singleton(){
                if(!singleton) singleton = static_cast<T*>(this);
            }
            virtual ~Singleton(){
                if(this == singleton) singleton = 0;
            }

        public:
            static T* create(){ if(!singleton) singleton = new T; return singleton; }
            static void destroy(){ delete singleton; singleton = 0; }
            static T& shared(){ return (*singleton); }
    };
}
