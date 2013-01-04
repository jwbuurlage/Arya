#include <glm/glm.hpp>
#include <vector>

using std::vector;
using glm::vec2;

struct QuadTree 
{
    QuadTree(int _level, int _maxLevel,
            vec2 _position, vec2 _size)
    {
        level = _level;
        maxLevel = _maxLevel;
        position = _position;
        size = _size;
    }

    ~QuadTree() { };

    // these will be in world coordinates
    vec2 position;
    vec2 size;

    // how far down we are (0 = root, maxLevel = leaf)
    int maxLevel;
    int level;

    // unique identifier for this QuadTree level
    int id;

    QuadTree* children[4];
    QuadTree* parent;

    void insert(int id, vec2 position)
    {
        if(level == maxLevel)
            //insert
        else
        {
            // cases insert children
        }
    }

    // This vector can be used to store
    // for example Unit ids
    vector<int> idRefs;

    // This can be used to store for example
    // whether a quad is invisible
    bool flag;
    void setFlag(bool _flag)
    {
        flag = _flag;
        if(level)
            for(int i = 0; i < 4; ++i)
                children[i].setFlag(_flag);
    }
};
