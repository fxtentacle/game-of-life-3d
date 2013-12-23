#include <memory.h>
#include <stdlib.h>
#include <math.h>

#include "Grid.h"

void Grid::init() {
    srand(1705);
    for(int x=0;x<GRID_SIZE_X;x++)
        for(int y=0;y<GRID_SIZE_Y;y++)
            for(int z=0;z<GRID_SIZE_Z;z++)
                data(x,y,z) = 0;//rand()%256==0;
    
    bool test[5][5] = {{1,1,1,1,1},{1,0,0,0,1},{1,0,1,0,1},{1,0,0,0,1},{1,1,1,1,1}};
    for(int tt=0;tt<4;tt++ )
        stamp<5>(test, 5);
    
}
void Grid::clear() {
    memset(dataStorage, 0x00, sizeof(bool)*GRID_SIZE_X*GRID_SIZE_Y*GRID_SIZE_Z);
}
void Grid::toggle(int x, int y, int z) {
    data(x,y,z) = !data(x,y,z);
}

int Grid::neighborsFor(int x, int y, int z) const {
    int neighbors = -data(x,y,z);
    for(int ox=-1;ox<=1;ox++)
        for(int oy=-1;oy<=1;oy++)
            for(int oz=-1;oz<=1;oz++)
                neighbors += data((x+GRID_SIZE_X+ox) % GRID_SIZE_X, (y+GRID_SIZE_Y+oy) % GRID_SIZE_Y, (z+GRID_SIZE_Z+oz) % GRID_SIZE_Z);
    return neighbors;
}

void Grid::simulate(Grid const& current, bool rules[2][27]) {
    for(int x=0;x<GRID_SIZE_X;x++){
        for(int y=0;y<GRID_SIZE_Y;y++){
            for(int z=0;z<GRID_SIZE_Z;z++) {
                int neighbors = current.neighborsFor(x, y, z);
                data(x,y,z) = rules[current.data(x,y,z)][neighbors];
            }
        }
    }
}
void Grid::placeInCenter(Grid const& other) {
    for(int x=0;x<other.GRID_SIZE_X;x++){
        for(int y=0;y<other.GRID_SIZE_Y;y++){
            for(int z=0;z<other.GRID_SIZE_Z;z++) {
                int ox = (GRID_SIZE_X/2 - other.GRID_SIZE_X/2 + x + GRID_SIZE_X) % GRID_SIZE_X;
                int oy = (GRID_SIZE_Y/2 - other.GRID_SIZE_Y/2 + y + GRID_SIZE_Y) % GRID_SIZE_Y;
                int oz = (GRID_SIZE_Z/2 - other.GRID_SIZE_Z/2 + z + GRID_SIZE_Z) % GRID_SIZE_Z;
                data(ox,oy,oz) = other.data(x,y,z);
            }
        }
    }
}

void Grid::copyFrom(Grid const& other){
    memcpy(dataStorage, other.dataStorage, sizeof(bool)*GRID_SIZE_X*GRID_SIZE_Y*GRID_SIZE_Z);
}
    
template<int N> void Grid::stamp(bool stampme[N][N], int thick) {
    int posx = rand()%GRID_SIZE_X;
    int posy = rand()%GRID_SIZE_Y;
    int posz = rand()%GRID_SIZE_Z;
    int axis = rand()%3;
    int invx = (rand()%2)*2-1;
    int invy = (rand()%2)*2-1;
    int invz = (rand()%2)*2-1;
    
    for(int x=0;x<N;x++){
        for(int y=0;y<N;y++){
            for(int z=0;z<thick;z++){
                int v = GRID_SIZE_X + x*invx;
                int w = GRID_SIZE_Y + y*invy;
                int d = GRID_SIZE_Z + z*invz;
                if(axis==0) data((posx+v)%GRID_SIZE_X, (posy+w)%GRID_SIZE_Y, (posz+d)%GRID_SIZE_Z) = stampme[x][y];
                else if(axis==1) data((posx+v)%GRID_SIZE_X, (posy+d)%GRID_SIZE_Y, (posz+w)%GRID_SIZE_Z) = stampme[x][y];
                else if(axis==2) data((posx+d)%GRID_SIZE_X, (posy+v)%GRID_SIZE_Y, (posz+w)%GRID_SIZE_Z) = stampme[x][y];
            }
        }
    }
}

#include "Vec3.h"
class GeomEmitter {
public:
    float* vertices;
    int nTriangles;

    Vec3 pos, texFace, normal;
    
    float tmp[4][ATTRIBUTES_PER_VERTEX];
    int ntmp;
    
    inline GeomEmitter() {
        ntmp = 0;
        nTriangles = 0;
    }
    
    inline void EmitVertex() {
        pos.copyTo(&tmp[ntmp][0]);
        texFace.copyTo(&tmp[ntmp][3]);
        Vec3 texWorld = (pos+Vec3(16.0)) / 32.0;
        texWorld.copyTo(&tmp[ntmp][6]);
        normal.copyTo(&tmp[ntmp][9]);
        ntmp++;
    }
    
    inline void EndPrimitive() {
        for(int t=0;t<2;t++) {
            for(int p=0;p<3;p++) {
                memcpy(&vertices[(nTriangles*3+p)*ATTRIBUTES_PER_VERTEX], tmp[t+p], sizeof(float)*ATTRIBUTES_PER_VERTEX);
//                printf("%f, %f, %f\n", )
            }
            nTriangles++;
        }
        ntmp=0;
    }
    
    inline void EmitPoint(float cx, float cy, float cz, float siz) {
        Vec3 centerTex(cx,cy,cz);
        Vec3 centerPosition = centerTex * 32.0 - Vec3(16.0);
        siz*=0.5;
        
        normal = Vec3(0,0,-1, 0);
        texFace = Vec3(0,0,0,0);
        pos = centerPosition + siz* Vec3(-1, -1, -1);
        
        EmitVertex();
        texFace = Vec3(1,0,0,0);
        pos = centerPosition + siz* Vec3(1, -1, -1);
        
        EmitVertex();
        texFace = Vec3(0,1,0,0);
        pos = centerPosition + siz* Vec3(-1, 1, -1);
        
        EmitVertex();
        texFace = Vec3(1,1,0,0);
        pos = centerPosition + siz* Vec3(1, 1, -1);
        
        EmitVertex();
        EndPrimitive();
        
        normal = Vec3(0,0,1, 0);
        texFace = Vec3(0,0,0,0);
        pos = centerPosition + siz* Vec3(-1, -1, 1);
        
        EmitVertex();
        texFace = Vec3(1,0,0,0);
        pos = centerPosition + siz* Vec3(1, -1, 1);
        
        EmitVertex();
        texFace = Vec3(0,1,0,0);
        pos = centerPosition + siz* Vec3(-1, 1, 1);
        
        EmitVertex();
        texFace = Vec3(1,1,0,0);
        pos = centerPosition + siz* Vec3(1, 1, 1);
        
        EmitVertex();
        EndPrimitive();
        
        
        
        normal = Vec3(-1,0,0, 0);
        texFace = Vec3(0,0,0,0);
        pos = centerPosition + siz* Vec3(-1, -1, -1);
        
        EmitVertex();
        texFace = Vec3(1,0,0,0);
        pos = centerPosition + siz* Vec3(-1, -1, 1);
        
        EmitVertex();
        texFace = Vec3(0,1,0,0);
        pos = centerPosition + siz* Vec3(-1, 1, -1);
        
        EmitVertex();
        texFace = Vec3(1,1,0,0);
        pos = centerPosition + siz* Vec3(-1, 1, 1);
        
        EmitVertex();
        EndPrimitive();
        
        normal = Vec3(1,0,0, 0);
        texFace = Vec3(0,0,0,0);
        pos = centerPosition + siz* Vec3(1, -1, -1);
        
        EmitVertex();
        texFace = Vec3(1,0,0,0);
        pos = centerPosition + siz* Vec3(1, -1, 1);
        
        EmitVertex();
        texFace = Vec3(0,1,0,0);
        pos = centerPosition + siz* Vec3(1, 1, -1);
        
        EmitVertex();
        texFace = Vec3(1,1,0,0);
        pos = centerPosition + siz* Vec3(1, 1, 1);
        
        EmitVertex();
        EndPrimitive();
        
        
        
        normal = Vec3(0,1,0, 0);
        texFace = Vec3(0,0,0,0);
        pos = centerPosition + siz* Vec3(-1, 1, -1);
        
        EmitVertex();
        texFace = Vec3(1,0,0,0);
        pos = centerPosition + siz* Vec3(-1, 1, 1);
        
        EmitVertex();
        texFace = Vec3(0,1,0,0);
        pos = centerPosition + siz* Vec3(1, 1, -1);
        
        EmitVertex();
        texFace = Vec3(1,1,0,0);
        pos = centerPosition + siz* Vec3(1, 1, 1);
        
        EmitVertex();
        EndPrimitive();
        
        
        normal = Vec3(0,-1,0, 0);
        texFace = Vec3(0,0,0,0);
        pos = centerPosition + siz* Vec3(-1, -1, -1);
        
        EmitVertex();
        texFace = Vec3(1,0,0,0);
        pos = centerPosition + siz* Vec3(-1, -1, 1);
        
        EmitVertex();
        texFace = Vec3(0,1,0,0);
        pos = centerPosition + siz* Vec3(1, -1, -1);
        
        EmitVertex();
        texFace = Vec3(1,1,0,0);
        pos = centerPosition + siz* Vec3(1, -1, 1);
        
        EmitVertex();
        EndPrimitive();
    }
};


int Grid::emitVertices(float* vertices, Grid const& from, Grid const& to, float lerp) {
    GeomEmitter ge;
    ge.vertices = vertices;
    
    for(int x=0;x<from.GRID_SIZE_X;x++) {
        for(int y=0;y<from.GRID_SIZE_Y;y++) {
            for(int z=0;z<from.GRID_SIZE_Z;z++) {
                if(from.data(x,y,z) || to.data(x,y,z)) {
                    float cx = (float)x / (float)from.GRID_SIZE_X;
                    float cy = (float)y / (float)from.GRID_SIZE_Y;
                    float cz = (float)z / (float)from.GRID_SIZE_Z;
                    
                    double pv = 0.5;
                    float siz = from.data(x,y,z) * pow((double)(1.0-lerp), pv) + to.data(x,y,z) * pow((double)lerp,pv);
                    if(from.data(x,y,z)==to.data(x,y,z)) siz = 1;

                    ge.EmitPoint(cx,cy,cz,siz);
                }
            }
        }
    }
    return ge.nTriangles*3;
}

int Grid::emitPoint(float* vertices, int x, int y, int z) {
    GeomEmitter ge;
    ge.vertices = vertices;
    
    float cx = (float)x / (float)GRID_SIZE_X;
    float cy = (float)y / (float)GRID_SIZE_Y;
    float cz = (float)z / (float)GRID_SIZE_Z;
    
    ge.EmitPoint(cx,cy,cz,1.0);

    return ge.nTriangles*3;
}




void Grid::writeWithRules(const char* filename, bool rules[2][27]) {
    FILE* f = fopen(filename,"wb");
    for(int r=0;r<2;r++) {
        for(int c=0;c<27;c++) {
            unsigned char v = rules[r][c] ? 0xFF : 0;
            fwrite(&v,1,1,f);
        }
    }
    fwrite(&GRID_SIZE_X, 4, 1, f);
    fwrite(&GRID_SIZE_Y, 4, 1, f);
    fwrite(&GRID_SIZE_Z, 4, 1, f);
    for(int z=0;z<GRID_SIZE_Z;z++) {
        for(int y=0;y<GRID_SIZE_Y;y++) {
            for(int x=0;x<GRID_SIZE_X;x++) {
                unsigned char v = data(x,y,z) ? 0xFF : 0;
                fwrite(&v,1,1,f);
            }
        }
    }
    fclose(f);
}

void Grid::readWithRules(const char* filename, bool rules[2][27]) {
    delete[] dataStorage;
    
    FILE* f = fopen(filename,"rb");
    for(int r=0;r<2;r++) {
        for(int c=0;c<27;c++) {
            unsigned char v;
            fread(&v,1,1,f);
            rules[r][c] = v != 0;
        }
    }
    fread(&GRID_SIZE_X, 4, 1, f);
    fread(&GRID_SIZE_Y, 4, 1, f);
    fread(&GRID_SIZE_Z, 4, 1, f);
    
    dataStorage = new bool[GRID_SIZE_X * GRID_SIZE_Y * GRID_SIZE_Z];
    for(int z=0;z<GRID_SIZE_Z;z++) {
        for(int y=0;y<GRID_SIZE_Y;y++) {
            for(int x=0;x<GRID_SIZE_X;x++) {
                unsigned char v;
                fread(&v,1,1,f);
                data(x,y,z) = v != 0;
            }
        }
    }
    fclose(f);
}

