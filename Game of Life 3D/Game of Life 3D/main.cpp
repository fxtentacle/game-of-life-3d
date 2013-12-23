#include "BaseGL.h"
#include "Grid.h"
#include "lodepng.h"


void breakErr(const char* function, int line);
void breakErr(const char* function, int line) {
	GLenum err = glGetError();
	if(err) {
		if(err == 0x506)
			printf("%s %d: INVALID_FRAMEBUFFER_OPERATION_EXT", function, line);
		else if(err == 0x502)
			printf("%s %d: GL_INVALID_OPERATION", function, line);
		else if(err == 0x500)
			printf("%s %d: GL_INVALID_ENUM", function, line);
		else
			printf("%s %d: OpenGL Error: %x", function, line, err);
	}
}

#ifdef WIN32
#define GL_BREAK_ERROR breakErr(__FUNCTION__, __LINE__);
#else
#define GL_BREAK_ERROR breakErr(__PRETTY_FUNCTION__, __LINE__);
#endif

class Game : public BaseGL {
public:
    Grid grid, nextGrid, editGrid;
    
    bool rules[2][27];
    
    int gameState;
    int editLocation[3];
    int ruleEditLocation[2];
    
    double currentFrameTime;
    double nextUpdateTime, lastUpdatetime, updateTimespan, updateSleep;

    int ssaoNoiseSize;
    int ssaoKernelSize;
    float ssaoKernelPow;
    
    
    bool sequenceWrite;
    int sequenceCounter;

    inline Game() {
        bool r2[2][27] = {
            {false,false,false,false,false,true,false,false,true,false},
            {false,false,false,false,false,false,true,true,false,}
        };
        memcpy(rules,r2,sizeof(bool)*2*27);
        gameState = 1;
        editLocation[0] = grid.GRID_SIZE_X/2;
        editLocation[1] = grid.GRID_SIZE_Y/2;
        editLocation[2] = grid.GRID_SIZE_Z/2;
        ruleEditLocation[0] = 0;
        ruleEditLocation[1] = 0;

        currentFrameTime = glfwGetTime();
        nextUpdateTime = currentFrameTime + 1.0;
        lastUpdatetime = currentFrameTime;
        updateTimespan = 1.0;
        updateSleep = 0.5;
        
        ssaoNoiseSize = 3;
        ssaoKernelSize = 64;
        ssaoKernelPow = 3;
        
        sequenceWrite = false;
    }

    int renderWidth, renderHeight, displayHeight;
    
    GLuint drawShaderSolid, drawShader, drawShaderQuad, drawShaderSsao, drawShaderSsaoBlur;
    GLuint drawTexture, textureDot, textureDotB;
    
    GLuint framebufferSolidTg, framebufferSolidTex, framebufferSolidNormal, framebufferSolidDepth;
    GLuint framebufferSsaoTg, framebufferSsaoTex;
    
    GLuint screenshotBuffer;
    
    GLuint textureSsaoNoise;
    
    GLuint gridRenderingVbo;
    int gridRenderingCount;
    
    
    

    inline virtual bool init() {
        renderWidth = width;
        renderHeight = height/2;
        displayHeight = height/2;

        drawShaderSolid = loadShader("data/draw.vsh", "data/draw_solid.fsh", 0);
        drawShader = loadShader("data/draw.vsh", "data/draw.fsh", 0);
        drawShaderQuad = loadShader("data/quad.vsh", "data/quad.fsh", 0);
        drawShaderSsao = loadShader("data/ssao.vsh", "data/ssao.fsh", 0);
        drawShaderSsaoBlur = loadShader("data/quad.vsh", "data/ssaoblur.fsh", 0);
        
        
        drawTexture = SOIL_load_OGL_texture (
                                                    "data/tex.png",
                                                    SOIL_LOAD_AUTO,
                                                    SOIL_CREATE_NEW_ID,
                                                    SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y
                                                    );
        glBindTexture(GL_TEXTURE_2D, drawTexture);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8);
        
        textureDot = SOIL_load_OGL_texture (
                                                   "data/dot.png",
                                                   SOIL_LOAD_AUTO,
                                                   SOIL_CREATE_NEW_ID,
                                                   SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y
                                                   );
        textureDotB = SOIL_load_OGL_texture (
                                                    "data/dot_b.png",
                                                    SOIL_LOAD_AUTO,
                                                    SOIL_CREATE_NEW_ID,
                                                    SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y
                                                    );
        
        glDisable(GL_CULL_FACE);
        glBlendFunc(GL_ONE, GL_ONE);
        

        
        framebufferSolidTg = 0;
        glGenFramebuffers(1, &framebufferSolidTg);
        glBindFramebuffer(GL_FRAMEBUFFER, framebufferSolidTg);
        
        glGenTextures(1, &framebufferSolidTex);
        glBindTexture(GL_TEXTURE_2D, framebufferSolidTex);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, renderWidth, renderHeight, 0,GL_RGB, GL_UNSIGNED_BYTE, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        
        glGenTextures(1, &framebufferSolidNormal);
        glBindTexture(GL_TEXTURE_2D, framebufferSolidNormal);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, renderWidth, renderHeight, 0,GL_RGB, GL_UNSIGNED_BYTE, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        
        glGenTextures(1, &framebufferSolidDepth);
        glBindTexture(GL_TEXTURE_2D, framebufferSolidDepth);
        
        glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT24, renderWidth, renderHeight, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        
        
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferSolidTex, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, framebufferSolidNormal, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, framebufferSolidDepth, 0);
        
        GLenum DrawBuffers[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
        glDrawBuffers(2, DrawBuffers);
        
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            return false;
        }
        
        
        
        framebufferSsaoTg = 0;
        glGenFramebuffers(1, &framebufferSsaoTg);
        glBindFramebuffer(GL_FRAMEBUFFER, framebufferSsaoTg);
        
        glGenTextures(1, &framebufferSsaoTex);
        glBindTexture(GL_TEXTURE_2D, framebufferSsaoTex);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, renderWidth, renderHeight, 0,GL_RGB, GL_UNSIGNED_BYTE, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferSsaoTex, 0);
        
        glDrawBuffers(1, DrawBuffers);
        
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            return false;
        }
        

        
        glGenTextures(1, &textureSsaoNoise);
        glBindTexture(GL_TEXTURE_2D, textureSsaoNoise);
        
        srand(123456);
        float* noiseData = new float[ssaoNoiseSize*ssaoNoiseSize*3];
        for(int i=0;i<ssaoNoiseSize*ssaoNoiseSize;i++) {
            float* c = &noiseData[i*3];
            c[0] = (double)rand() / (double)RAND_MAX * 2.0 - 1.0;
            c[1] = (double)rand() / (double)RAND_MAX * 2.0 - 1.0;
            c[2] = 0;
            float siz = sqrt(c[0]*c[0]+c[1]*c[1]);
            c[0] /= siz;
            c[1] /= siz;
        }
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ssaoNoiseSize, ssaoNoiseSize, 0,GL_RGB, GL_FLOAT, noiseData);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        
        
        float* ssaoKernelData = new float[ssaoKernelSize*3];
        for(int i=0;i<ssaoKernelSize;i++) {
            float* c = &ssaoKernelData[i*3];
            c[0] = (double)rand() / (double)RAND_MAX * 2.0 - 1.0;
            c[1] = (double)rand() / (double)RAND_MAX * 2.0 - 1.0;
            c[2] = (double)rand() / (double)RAND_MAX;
            float scale = (float)i / (float)ssaoKernelSize;
            scale = 0.1 + 0.9 * pow(scale, ssaoKernelPow);
            float siz = sqrt(c[0]*c[0]+c[1]*c[1]+c[2]*c[2]) / scale;
            c[0] /= siz;
            c[1] /= siz;
            c[2] /= siz;
        }
        
        glUseProgram(drawShaderSsao);
        glUniform1i(glGetUniformLocation(drawShaderSsao, "uKernelSize"), ssaoKernelSize);
        glUniform3fv(glGetUniformLocation(drawShaderSsao, "uKernelOffsets"), ssaoKernelSize, (const GLfloat*) ssaoKernelData);
        
        glUniform2f(glGetUniformLocation(drawShaderSsao, "noiseScale"), (float)renderWidth/(float)ssaoNoiseSize, (float)renderHeight/(float)ssaoNoiseSize);
        
        
        
        editGrid.init();
        
        
        glGenBuffersARB(1, &gridRenderingVbo);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, gridRenderingVbo);
        {
            int nmaxV = grid.GRID_SIZE_X*grid.GRID_SIZE_Y*grid.GRID_SIZE_Z*VERTICES_PER_BOX*ATTRIBUTES_PER_VERTEX;
            GLfloat* vertices = new GLfloat[nmaxV];
            glBufferDataARB(GL_ARRAY_BUFFER_ARB, nmaxV*sizeof(float), vertices, GL_STREAM_DRAW_ARB);
            delete[] vertices;
        }
        gridRenderingCount = 0;
        
        
        
        glGenBuffers(1, &screenshotBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, screenshotBuffer);
        glBufferData(GL_ARRAY_BUFFER, width*height*4, NULL, GL_DYNAMIC_READ);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        
        return true;
    }
    
    inline virtual void teardown() {
        glDeleteBuffersARB(1, &gridRenderingVbo);
    }
    
    float projectionMatrix[4*4];
    float invProjectionMatrix[4*4];
    
    inline virtual void render() {
        if(!sequenceWrite) currentFrameTime = glfwGetTime();
        else currentFrameTime += 1.0/50.0;
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0,0,width,height);
        
        glClearColor(0, 0, 0, 0);
        glClearDepth(1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glDepthFunc(GL_LESS);
        
        if(gameState != 2) prepareGrid();
        
        for(int eye = 0; eye<2;eye++) {
            float aspect = (double)renderWidth / (double)renderHeight;
            
            float side = eye*2.0-1.0;
            float separation = 0.03 / aspect;
            float convergence = 22;

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            float tanHalfFov = tan(70.0 / 180.0 * 3.1415926 / 2.0);
            float clipNear_ = 2;
            float clipFar_ = 100.0;
            float d = 1.0 / tanHalfFov;
            float npf = clipFar_ + clipNear_;
            float nmf = clipNear_ - clipFar_;
            float nf = clipNear_ * clipFar_;
            float pmat[4*4] = {
                
                d / aspect,                     0.0,			0.0,							0.0,
                0.0,                            d,				0.0,							0.0,
                side*separation,                0.0,			-npf / nmf,                     1.0f,
                -side*separation*convergence,	0.0,			2.0f * nf / nmf,				0.0
            };
            memcpy(projectionMatrix, pmat, sizeof(float)*4*4);
            glLoadMatrixf(projectionMatrix);
            gluInvertMatrix(projectionMatrix, invProjectionMatrix);
            
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            glTranslatef(0, 0, 2.3*16);
            
            
            if(gameState != 2) drawGridSsao();
            
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0,(1-eye)*displayHeight,width,displayHeight);
            
            if(gameState != 2) drawGrid();
            else drawRules();
        }
     }
    
    inline void drawRules() {
        glUseProgram(drawShaderQuad);
        glBindTexture(GL_TEXTURE_2D, textureDot);
        
        for(int s=0;s<2;s++) {
            for(int r=0;r<3;r++) {
                for(int c=0;c<9;c++) {
                    int f = r*9+c;
                    if(rules[s][f]) glColor3f(0.0,0.8,0.0);
                    else  glColor3f(0.8,0.0,0.0);
                    
                    float y = -((double)(s*4+r) / 5.0 - 1.0) - 0.3;
                    float x = (double)(c) / 5.0 - 1.0 + 0.1;
                    drawQuad(x, y, x+0.2, y-0.2);
                    
                    if(s*3+r == ruleEditLocation[1] && c == ruleEditLocation[0]) {
                        glBindTexture(GL_TEXTURE_2D, textureDotB);
                        glEnable(GL_BLEND);
                        float b = 0.75+sin(currentFrameTime*10.0)*0.25;
                        glColor3f(b,b,b);
                        drawQuad(x, y, x+0.2, y-0.2);
                        glDisable(GL_BLEND);
                        glBindTexture(GL_TEXTURE_2D, textureDot);
                    }
                }
            }
        }
    }
    
    inline void prepareGrid() {
        if(gameState==1) {
            grid.copyFrom(editGrid);
            nextGrid.copyFrom(editGrid);
        }
        
        
        float lerp = (currentFrameTime - lastUpdatetime) / updateTimespan;
        if(lerp < 0) lerp = 0;
        if(lerp > 1) lerp = 1;
        glBindBuffer(GL_ARRAY_BUFFER, gridRenderingVbo);
        float* vertices = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY_ARB);
        if(vertices)
        {
            gridRenderingCount = Grid::emitVertices(vertices, grid, nextGrid, lerp);
            glUnmapBuffer(GL_ARRAY_BUFFER); // unmap it after use
        }
        
    }
    
    inline void drawGridSsao() {
        glBindFramebuffer(GL_FRAMEBUFFER, framebufferSolidTg);
        glViewport(0,0,renderWidth,renderHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        
        glBindBuffer(GL_ARRAY_BUFFER, gridRenderingVbo);
        
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, sizeof(float)*ATTRIBUTES_PER_VERTEX, 0);
        GL_BREAK_ERROR
        
        glClientActiveTexture(GL_TEXTURE0);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(3, GL_FLOAT, sizeof(float)*ATTRIBUTES_PER_VERTEX, ((const char*)0)+(sizeof(float)*3) );
        GL_BREAK_ERROR
        
        glClientActiveTexture(GL_TEXTURE1);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(3, GL_FLOAT, sizeof(float)*ATTRIBUTES_PER_VERTEX, ((const char*)0)+(sizeof(float)*6) );
        
        glClientActiveTexture(GL_TEXTURE2);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(3, GL_FLOAT, sizeof(float)*ATTRIBUTES_PER_VERTEX, ((const char*)0)+(sizeof(float)*9) );
        
        glClientActiveTexture(GL_TEXTURE0);
        
        
        
//        glUseProgram(drawShader);
        glUseProgram(drawShaderSolid);
        glBindTexture(GL_TEXTURE_2D, drawTexture);
        glEnable(GL_DEPTH_TEST);
GL_BREAK_ERROR
        glDrawArrays(GL_TRIANGLES, 0, gridRenderingCount);
GL_BREAK_ERROR
        glDisable(GL_DEPTH_TEST);
        
                
        glBindFramebuffer(GL_FRAMEBUFFER, framebufferSsaoTg);
        glViewport(0,0,renderWidth,renderHeight);
        
        glUseProgram(drawShaderSsao);
        glUniform1i(glGetUniformLocation(drawShaderSsao, "uGBufferGeometricTex"), 0);
        glUniform1i(glGetUniformLocation(drawShaderSsao, "uGBufferDepthTex"), 1);
        glUniform1i(glGetUniformLocation(drawShaderSsao, "uNoiseTex"), 2);
        glUniformMatrix4fv(glGetUniformLocation(drawShaderSsao, "uProjectionMatrix"), 1, GL_FALSE, projectionMatrix);
        glUniformMatrix4fv(glGetUniformLocation(drawShaderSsao, "uInvProjectionMatrix"), 1, GL_FALSE, invProjectionMatrix);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, framebufferSolidNormal);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, framebufferSolidDepth);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, textureSsaoNoise);
        glColor3f(1,1,1);
        drawQuad(-1, -1, 1, 1);
        
    }
    inline void drawGrid() {
        glUseProgram(drawShaderSsaoBlur);
        glUniform1i(glGetUniformLocation(drawShaderSsaoBlur, "uColorTex"), 0);
        glUniform1i(glGetUniformLocation(drawShaderSsaoBlur, "uSsaoTex"), 1);
		glUniform2f(glGetUniformLocation(drawShaderSsaoBlur, "texelSize"), 1.0/(double)renderWidth, 1.0/(double)renderHeight);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, framebufferSolidTex);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, framebufferSsaoTex);
        glColor3f(1,1,1);
        drawQuad(-1, -1, 1, 1);
        
    
        glActiveTexture(GL_TEXTURE0);
        
        /*
         glUseProgram(drawShader);
         glBindTexture(GL_TEXTURE_2D, drawTexture);
         glEnable(GL_BLEND);
         glColor3f(0.1,0.1,0.1);
         glDrawArrays(GL_TRIANGLES, 0, gridRenderingCount);
         glDisable(GL_BLEND);
        */
        
        
        if( gameState==1) {
            // editing grid
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            
            float tgbuf[VERTICES_PER_BOX*ATTRIBUTES_PER_VERTEX];
            int nptr = grid.emitPoint(tgbuf, editLocation[0], editLocation[1], editLocation[2]);
            
            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(3, GL_FLOAT, sizeof(float)*ATTRIBUTES_PER_VERTEX, tgbuf);
            GL_BREAK_ERROR
            
            glClientActiveTexture(GL_TEXTURE0);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glTexCoordPointer(3, GL_FLOAT, sizeof(float)*ATTRIBUTES_PER_VERTEX, tgbuf+3 );
            GL_BREAK_ERROR
            
            glClientActiveTexture(GL_TEXTURE1);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glTexCoordPointer(3, GL_FLOAT, sizeof(float)*ATTRIBUTES_PER_VERTEX, tgbuf+6 );
            
            glClientActiveTexture(GL_TEXTURE2);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glTexCoordPointer(3, GL_FLOAT, sizeof(float)*ATTRIBUTES_PER_VERTEX, tgbuf+9 );
            
            glClientActiveTexture(GL_TEXTURE0);
            
            glUseProgram(drawShader);
            glBindTexture(GL_TEXTURE_2D, drawTexture);
            glEnable(GL_BLEND);
            glColor3f(0.75+sin(currentFrameTime*10.0)*0.25,0,0);
            glDrawArrays(GL_TRIANGLES, 0, nptr);
            glDisable(GL_BLEND);
        }
    }
    
    inline virtual void update() {
        if(sequenceWrite) {
            glReadBuffer(GL_FRONT);
            glBindBuffer(GL_PIXEL_PACK_BUFFER, screenshotBuffer);
            glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, 0);
            glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
           
            glBindBuffer(GL_ARRAY_BUFFER, screenshotBuffer);
            const unsigned char *pixels = (const unsigned char*) glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY_ARB);
//            const unsigned char *pixels = 0;
            GL_BREAK_ERROR
            if(pixels) {
                char filename[512];
                sprintf(filename, "img%04d.png", sequenceCounter);

                LodePNGState state;
                lodepng_state_init(&state);
                state.info_raw.colortype = LCT_RGBA;
                state.info_raw.bitdepth = 8;
                state.info_png.color.colortype = LCT_RGBA;
                state.info_png.color.bitdepth = 8;
                state.encoder.zlibsettings.windowsize = 256;
                state.encoder.zlibsettings.lazymatching = false;
                
                unsigned char* buffer;
                size_t buffersize;
                lodepng_encode(&buffer, &buffersize, pixels, width, height, &state);
                lodepng_save_file(buffer, buffersize, filename);
                free(buffer);

                glUnmapBuffer(GL_ARRAY_BUFFER);
            }
            sequenceCounter++;
            
        }

        if( gameState==3 ) {
            //playing
            if(currentFrameTime > nextUpdateTime) {
                lastUpdatetime = currentFrameTime;
                nextUpdateTime = lastUpdatetime + updateTimespan + updateSleep;
                grid.copyFrom(nextGrid);
                nextGrid.simulate(grid, rules);
            }
        }
    }
    
    inline virtual bool resized() {
        renderWidth = width;
        renderHeight = height/2;
        displayHeight = height/2;
        
        glBindTexture(GL_TEXTURE_2D, framebufferSolidTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, renderWidth, renderHeight, 0,GL_RGB, GL_UNSIGNED_BYTE, 0);

        glBindTexture(GL_TEXTURE_2D, framebufferSolidNormal);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, renderWidth, renderHeight, 0,GL_RGB, GL_UNSIGNED_BYTE, 0);

        glBindTexture(GL_TEXTURE_2D, framebufferSolidDepth);
        glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT24, renderWidth, renderHeight, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, framebufferSolidTg);
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            return false;
        }
        
        
        glBindTexture(GL_TEXTURE_2D, framebufferSsaoTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, renderWidth, renderHeight, 0,GL_RGB, GL_UNSIGNED_BYTE, 0);
        
        glBindFramebuffer(GL_FRAMEBUFFER, framebufferSsaoTg);
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            return false;
        }
        
        
        glUseProgram(drawShaderSsao);
        glUniform2f(glGetUniformLocation(drawShaderSsao, "noiseScale"), (float)renderWidth/(float)ssaoNoiseSize, (float)renderHeight/(float)ssaoNoiseSize);
        
        
        glBindBuffer(GL_ARRAY_BUFFER, screenshotBuffer);
        glBufferData(GL_ARRAY_BUFFER, width*height*4, NULL, GL_DYNAMIC_READ);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        
        return true;
    }
    
    inline virtual void keyboard(int key,int scancode,int action,int mods) {
        if(gameState==1) {
            if(action==GLFW_PRESS || action==GLFW_REPEAT) {
                int axis = (mods & GLFW_MOD_SHIFT)!=0 ? 2:1;
                int inv = axis==2? 1:1;
                int axisGridSize = axis == 2 ? grid.GRID_SIZE_Z : grid.GRID_SIZE_Y;
                switch(key) {
                    case GLFW_KEY_LEFT:
                        editLocation[0] = (editLocation[0] + grid.GRID_SIZE_X - 1) % grid.GRID_SIZE_X;
                        break;
                    case GLFW_KEY_RIGHT:
                        editLocation[0] = (editLocation[0] + 1) % grid.GRID_SIZE_X;
                        break;
                    case GLFW_KEY_UP:
                        editLocation[axis] = (editLocation[axis] + axisGridSize + inv) % axisGridSize;
                        break;
                    case GLFW_KEY_DOWN:
                        editLocation[axis] = (editLocation[axis] + axisGridSize - inv) % axisGridSize;
                        break;
                    case GLFW_KEY_SPACE:
                        editGrid.toggle(editLocation[0], editLocation[1], editLocation[2]);
                        break;
                    case GLFW_KEY_ESCAPE:
                        editGrid.clear();
                        break;
                    case GLFW_KEY_S:
                        editGrid.writeWithRules("/gitosis/gol3d-game/gol3dgrid.bin", rules);
                        break;
                }
            }
        } else if(gameState==2) {
            if(action==GLFW_PRESS || action==GLFW_REPEAT) {
                switch(key) {
                    case GLFW_KEY_LEFT:
                        ruleEditLocation[0] = (ruleEditLocation[0] + 9 - 1) % 9;
                        break;
                    case GLFW_KEY_RIGHT:
                        ruleEditLocation[0] = (ruleEditLocation[0] + 1) % 9;
                        break;
                    case GLFW_KEY_UP:
                        ruleEditLocation[1] = (ruleEditLocation[1] + 6 -1) % 6;
                        break;
                    case GLFW_KEY_DOWN:
                        ruleEditLocation[1] = (ruleEditLocation[1] + 1) % 6;
                        break;
                    case GLFW_KEY_SPACE:
                        int block = ruleEditLocation[1] >= 3 ? 1:0;
                        int row = ruleEditLocation[1] - block*3;
                        int f = row*9+ruleEditLocation[0];
                        rules[block][f] = !rules[block][f];
                        break;
                }
            }
        
    } else if(gameState==3) {
        if(action==GLFW_PRESS || action==GLFW_REPEAT) {
            switch(key) {
                case GLFW_KEY_SPACE:
                    nextUpdateTime = currentFrameTime + 1.0;
                    grid.copyFrom(editGrid);
                    nextGrid.copyFrom(editGrid);
                    break;
            }
        }
    }
    

        
        if(action==GLFW_PRESS || action==GLFW_REPEAT) {
            switch(key) {
                case GLFW_KEY_1:
                    gameState = 1;
                    break;
                case GLFW_KEY_2:
                    gameState = 2;
                    break;
                case GLFW_KEY_3:
                    gameState = 3;
                    break;
                case GLFW_KEY_W:
                    sequenceWrite = !sequenceWrite;
                    if(sequenceWrite) sequenceCounter = 0;
                    break;
            }
            
        }
    }
   
};





int main(void)
{
    Game g;
    return g.main();
}

#ifdef WIN32
#include <windows.h>
int WINAPI WinMain(
  _In_  HINSTANCE hInstance,
  _In_  HINSTANCE hPrevInstance,
  _In_  LPSTR lpCmdLine,
  _In_  int nCmdShow
  ) {
	  return main();
}
#endif
