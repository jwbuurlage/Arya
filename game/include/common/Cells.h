#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "GameLogger.h"

using std::vector;
using glm::vec2;

struct Cell
{
    // for unit id's for example
    // maybe linked list is faster
    // and keep it sorted
    vector<int> cellPoints;

    CellList* cellList; //pointer to parent list
    int cellx, celly; //x,y of this cell

    void add(int i)
    {
        cellPoints.push_back(i);
    }

    void remove(int i)
    {
        for (vector<int>::iterator it = cellPoints.begin(); it != cellPoints.end(); ++it)
        {
            if((*it) == i)
            {
                cellPoints.erase(it);
                return;
            }
        }

        GAME_LOG_ERROR("No such point in the cell: " << i << " possible rounding errors");
    }
};

struct CellList
{
    CellList(int _size, float _mapSize)
    {
        gridSize = _size;
        mapSize = _mapSize;
        cells = new Cell[gridSize * gridSize];
        for(int i = 0; i < gridSize; ++i)
            for(int j = 0; j < gridSize; ++j)
            {
                cellForIndex(i,j)->cellList = this;
                cellForIndex(i,j)->cellx = i;
                cellForIndex(i,j)->celly = j;
            }
    }

    void cellForPositionGivenSize(vec2 p, int& i, int& j)
    {
        i = (int)((p.x + mapSize/2)/mapSize * gridSize);
        j = (int)((p.y + mapSize/2)/mapSize * gridSize);
    }

    Cell* cellForPosition(const vec2& p)
    {
        int i = (int)((p.x + mapSize/2)/mapSize * gridSize);
        int j = (int)((p.y + mapSize/2)/mapSize * gridSize);
        return cellForIndex(i, j);
    }

    inline Cell* cellForIndex(int i, int j)
    {
        return &cells[j*gridSize + i];
    }

    void clear()
    {
        for(int i = 0; i < gridSize; ++i)
            for(int j = 0; j < gridSize; ++j)
                cellForIndex(i,j)->cellPoints.clear();
    }

    int gridSize;
    float mapSize;
    Cell* cells;
};
