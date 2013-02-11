#include <glm/glm.hpp>
#include <vector>

using std::vector;
using glm::vec2;

struct Point
{
    Point(int _i, int _p)
    {
        id = _i;
        pos = _p;
    }

    int id;
    int pos;
};

struct QuadTree 
{
    QuadTree(int _level, int _maxLevel,
            vec2 _position, vec2 _size)
    {
        level = _level;
        maxLevel = _maxLevel;
        position = _position;
        size = _size;

        // make children and grid
        if(level != maxLevel)
        {
            children[0] = new QuadTree();
            children[1] = new QuadTree();
            children[2] = new QuadTree();
            children[3] = new QuadTree();
        }
    }

    ~QuadTree() 
    {

    }

    // these will be in world coordinates
    vec2 position;
    vec2 size;

    // how far down we are (0 = root, maxLevel = leaf)
    int maxLevel;
    int level;

    // unique identifier for this QuadTree node
    int id;

    //  Children:
    //  |-----------|
    //  |  0  |  2  |
    //  | ----------|
    //  |  1  |  3  |
    //  |-----------|
    QuadTree* children[4];
    QuadTree* parent;
    QuadTree* neighbours;

    void insert(int id, vec2 insertPosition)
    {
        if(level == maxLevel)
            refs.push_back(Point(id, insertPosition));
        else
        {
            if((insertPosition.x <= position.x) && (insertPosition.y <= position.y))
                children[0].insert(id, insertPosition);
            else if((insertPosition.x <= position.x) && (insertPosition.y >= position.y))
                children[1].insert(id, insertPosition);
            else if((insertPosition.x >= position.x) && (insertPosition.y <= position.y))
                children[2].insert(id, insertPosition);
            else if((insertPosition.x >= position.x) && (insertPosition.y >= position.y))
                children[3].insert(id, insertPosition);
        }
    }

    void clear()
    {
        // clear all levels
    }

    int closestId(vec2 position)
    {
        return 1;
    }

    // This vector can be used to store
    // for example Unit ids
    vector<Point> refs;

    // This can be used to store for example
    // whether a quad is invisible
    bool flag;
    void setFlag(bool _flag)
    {
        flag = _flag;
        if(level != maxLevel)
            for(int i = 0; i < 4; ++i)
                children[i].setFlag(_flag);
    }
};
