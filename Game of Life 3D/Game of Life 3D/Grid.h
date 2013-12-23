#pragma once

#define VERTICES_PER_BOX (2*3*6)
#define ATTRIBUTES_PER_VERTEX (3+3+3+3)
// pos, 3xtex

class Grid {
public:
    int32_t GRID_SIZE_X, GRID_SIZE_Y, GRID_SIZE_Z;
    bool* dataStorage;
    
    inline bool& data(int x, int y, int z) {
        return dataStorage[GRID_SIZE_X*GRID_SIZE_Y*z + GRID_SIZE_X*y + x];
    }
    
    inline bool data(int x, int y, int z) const {
        return dataStorage[GRID_SIZE_X*GRID_SIZE_Y*z + GRID_SIZE_X*y + x];
    }
    
    inline Grid() {
        GRID_SIZE_X = GRID_SIZE_Y = GRID_SIZE_Z = 32;
        dataStorage = new bool[GRID_SIZE_X * GRID_SIZE_Y * GRID_SIZE_Z];
    }
    
    inline Grid(int sizx, int sizy, int sizz) {
        GRID_SIZE_X = sizx;
        GRID_SIZE_Y = sizy;
        GRID_SIZE_Z = sizz;
        dataStorage = new bool[GRID_SIZE_X * GRID_SIZE_Y * GRID_SIZE_Z];
    }
    
    inline ~Grid() {
        delete[] dataStorage;
    }
    
    void init();
    void clear();
    void toggle(int x, int y, int z);
    void copyFrom(Grid const& other);
    void placeInCenter(Grid const& other);
    void writeWithRules(const char* filename, bool rules[2][27]);
    void readWithRules(const char* filename, bool rules[2][27]);

    
    int neighborsFor(int x, int y, int z) const;
    void simulate(Grid const& current, bool rules[2][27]);

    template<int N> void stamp(bool stampme[N][N], int thick);

    static int emitVertices(float* vertices, Grid const& from, Grid const& to, float lerp);
    int emitPoint(float* vertices, int x, int y, int z);
};