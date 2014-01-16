/* THIS IS NOT USED FOR NOW */

#pragma once

#include <glm/glm.hpp>
#include <vector>

using std::vector;
using glm::vec2;

struct TreePoint
{
    TreePoint(int _i, vec2 _p)
    {
        id = _i;
        pos = _p;
    }

    int id;
    vec2 pos;
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
            children[0] = new QuadTree(level + 1, maxLevel, vec2(position.x - 0.25f*size.x, position.y - 0.25f*size.y), 0.5f*size);
            children[1] = new QuadTree(level + 1, maxLevel, vec2(position.x - 0.25f*size.x, position.y + 0.25f*size.y), 0.5f*size);
            children[2] = new QuadTree(level + 1, maxLevel, vec2(position.x + 0.25f*size.x, position.y - 0.25f*size.y), 0.5f*size);
            children[3] = new QuadTree(level + 1, maxLevel, vec2(position.x + 0.25f*size.x, position.y + 0.25f*size.y), 0.5f*size);
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
        {
            refs.push_back(TreePoint(id, insertPosition));
        }
        else
        {
            if((insertPosition.x <= position.x) && (insertPosition.y <= position.y))
                children[0]->insert(id, insertPosition);
            else if((insertPosition.x <= position.x) && (insertPosition.y >= position.y))
                children[1]->insert(id, insertPosition);
            else if((insertPosition.x >= position.x) && (insertPosition.y <= position.y))
                children[2]->insert(id, insertPosition);
            else if((insertPosition.x >= position.x) && (insertPosition.y >= position.y))
                children[3]->insert(id, insertPosition);

        }
    }

    void clear()
    {
        if(level == maxLevel)
            refs.clear();
        else
            for(int i = 0; i < 4; ++i)
                children[i]->clear();
    }

    int closestId(vec2 checkPosition)
    {
        if(level == maxLevel)
        {
            int currentClosestId = -1;
            float currentClosestDistance = 100000.0f;
            for(unsigned int i = 0; i < refs.size(); ++i)
            {
                if(glm::distance(position, refs[i].pos) < currentClosestDistance)
                {
                    currentClosestDistance = glm::distance(position, refs[i].pos);
                    currentClosestId = refs[i].id;
                }
            }
            return currentClosestId;
        }
        else
        {
            if((checkPosition.x <= position.x) && (checkPosition.y <= position.y))
                return children[0]->closestId(checkPosition);
            else if((checkPosition.x <= position.x) && (checkPosition.y >= position.y))
                return children[1]->closestId(checkPosition);
            else if((checkPosition.x >= position.x) && (checkPosition.y <= position.y))
                return children[2]->closestId(checkPosition);
            else if((checkPosition.x >= position.x) && (checkPosition.y >= position.y))
                return children[3]->closestId(checkPosition);
        }
        return -1;
    }

    // This vector can be used to store
    // for example Unit ids
    vector<TreePoint> refs;

    // This can be used to store for example
    // whether a quad is invisible
    bool flag;
    void setFlag(bool _flag)
    {
        flag = _flag;
        if(level != maxLevel)
            for(int i = 0; i < 4; ++i)
                children[i]->setFlag(_flag);
    }
};
