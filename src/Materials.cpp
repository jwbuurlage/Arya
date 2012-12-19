#include "Materials.h"
#include "Files.h"
#include "common/Logger.h"
#include <vector>

namespace Arya
{
	template<> MaterialManager* Singleton<MaterialManager>::singleton = 0;

	bool MaterialManager::initialize(std::vector<std::string> filenames) {
		for(unsigned int i=0; i<filenames.size(); i++)
			getResource(filenames[i]);
		return true;
	}

	Material* MaterialManager::loadResource(std::string filename) {
		std::string name=filename.substr(0,filename.size()-4);
		std::string type;
		float a,b,c,d;
		File* mattyfile = FileSystem::shared().getFile(string("materials/") + string(name+".matty"));
		if(mattyfile == 0) {
			LOG_WARNING("Using default matty.");
			type="Default";
			a=1.0; b=1.0; c=0.3; d=0.7;
		} else {
			std::stringstream str(mattyfile->getData());
			str >> type >> a >> b >> c >> d;
			FileSystem::shared().releaseFile(mattyfile);
		}
		Material* result=new Material(name , TextureManager::shared().getTexture(filename), type, a, b, c, d);
		addResource(filename, result);
		return result;
	}

	void MaterialManager::cleanup() {
		unloadAll();
	}

}
