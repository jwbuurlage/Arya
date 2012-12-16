#pragma once
#include "Textures.h"
#include "common/Singleton.h"
#include <vector>
#include <map>
#include <glm/glm.hpp>
using glm::vec4;

namespace Arya
{
    class Material
    {
        public:
			std::string name;
			Texture* texture;
			std::string type;
			float specAmp;	// The "amount" of highlights
			float specPow;	// The "sharpness" of highlights
			float ambient;  // The "amount" of ambient lighting
			float diffuse;  // The "amount" of diffuse lighting

			vec4 getParameters() {
				return vec4(specAmp, specPow, ambient, diffuse);
			}
		private:
			friend class MaterialManager;
            friend class ResourceManager<Material>;
			Material(std::string _name, Texture* _texture, std::string _type, float _specAmp, float _specPow, float _ambient, float _diffuse){
				name=_name; texture=_texture; type=_type; specAmp=_specAmp; specPow=_specPow; ambient=_ambient; diffuse=_diffuse;
			}
			~Material(){}
    };

	class MaterialManager : public Singleton<MaterialManager>, public ResourceManager<Material> {
        public:
			MaterialManager(){}
			~MaterialManager(){cleanup();}

            bool initialize(std::vector<std::string> filenames);
            void cleanup();

			Material* getMaterial( std::string filename ){return getResource(filename); }

        private:
			Material* loadResource(std::string filename);
    };
};
