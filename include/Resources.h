//Base class for TextureManager, MeshManager, SoundManager and so on
//This class supplies public functions:
//      getResource - returns resource if loaded, or calls loadResource if not
//      unloadAll - deletes all resources
//      resourceLoaded - check if a resource is loaded
//The sub class must implement only 'loadResource'
//      This implementation must call addResource() to add it to the resource list

#pragma once
#include "common/Logger.h"
#include <string>
#include <map>

using std::string;
using std::map;

namespace Arya
{
    template <typename T> class ResourceManager {
        public:
            ResourceManager(){ defaultResource = 0; };
            virtual ~ResourceManager(){ unloadAll(); }

            //Will load the resource if not already loaded
            T* getResource( std::string filename )
            {
                ResourceContainer::iterator iter = resources.find(filename);
                if( iter != resources.end() )
                    return (T*)(iter->second);
                T* ret = loadResource(filename);
                if(ret) return ret;
                return defaultResource;
            }

            void unloadAll()
            {
                for( ResourceContainer::iterator iter = resources.begin(); iter != resources.end(); ++iter ){
                    T* resource = (T*)iter->second;
                    delete resource; //This will call the deconstructor
                }
                resources.clear();
            }

            bool resourceLoaded( std::string name ){
                ResourceContainer::iterator iter = resources.find(name);
                return (iter != resources.end());
            }

        private:
            typedef std::multimap<string,void*> ResourceContainer;
            ResourceContainer resources;

        protected:
            //Must be implemented by subclass and use addResource to add the resource
            virtual T* loadResource( std::string filename )=0;

            T* defaultResource;

            void addResource( std::string name, T* res ){
                resources.insert( ResourceContainer::value_type( name, res ) );
            }
    };
}
