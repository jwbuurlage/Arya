#include "Material.h"
#include "common/logger.h"
#include <vector>
#include <fstream>

namespace Arya
{
	template<> MaterialManager* Singleton<MaterialManager>::singleton = 0;

	bool MaterialManager::initialize(std::vector<std::string> filenames) {
		for(int i=0; i<filenames.size(); i++)
			getResource(filenames[i]);
		return true;
	}

	Material* MaterialManager::loadResource(std::string filename) {
		std::string name=filename.substr(0,filename.size()-4);
		std::string type;
		float a,b,c,d;
		std::ifstream fin(name+".matty");
		if(fin.good()) {
			fin >> type >> a >> b >> c >> d;
		} else {
			type="Default";
			a=1.0;
			b=1.0;
			c=0.3;
			d=0.7;
		}

		Material* result=new Material(name , TextureManager::shared().getTexture(filename), type, a, b, c, d);
		addResource(filename, result);
		return result;
	}

	void MaterialManager::cleanup() {
		unloadAll();
	}

}
