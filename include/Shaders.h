#include <string>
#include <vector>

using std::string;
using std::vector;

namespace Arya
{
  class AFile;

  enum ShaderType
  {
    VERTEX,
    FRAGMENT
  };

  class Shader
  {
    public:
      Shader(ShaderType _type) { type = _type; };
      ~Shader() { };

      bool addSourceFile(string f);
      bool compile();

    private:
      bool compiled;
      vector<AFile> sources;
      ShaderType type;
  };

  class ShaderProgram
  {
    public:
      ShaderProgram() { };
      ~ShaderProgram() { };

      void attach(Shader shader);
      bool link();
      bool use();

    private:
      int handle;
      bool linked;
  };

  class ShaderManager
  {
    public:
      ShaderManager() { };
      ~ShaderManager() { };

      bool init();
  };
}
