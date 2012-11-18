#include <string>
using std::string;

namespace Arya
{
  enum ShaderType
  {
    VERTEX,
    FRAGMENT
  };

  class Shader
  {
    public:
      Shader() { };
      ~Shader() { };

      bool loadFromString(string s, ShaderType type);
      bool loadFromFile(string file, ShaderType type);
      bool compile();

    private:
      string source;
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
