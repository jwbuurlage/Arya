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
    CellList(int _size)
    {
        gridSize = _size;
        cells = new Cell[gridSize * gridSize];
    }

    void cellForPositionGivenSize(vec2 p, float s, int& i, int& j)
    {
        i = (int)((p.x + s/2)/s * gridSize);
        j = (int)((p.y + s/2)/s * gridSize);
    }

    Cell* cellForIndex(int i, int j)
    {
        return &cells[j*gridSize + i];
    }

    void clear()
    {
        if(cells)
            delete[] cells;
        cells = new Cell[gridSize * gridSize];
    }

    int gridSize;
    Cell* cells;
};
