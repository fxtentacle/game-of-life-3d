#include "BaseGL.h"

void Keyboard_function(GLFWwindow* window,int key,int scancode,int action,int mods) {
    BaseGL* bgl = reinterpret_cast<BaseGL*>( glfwGetWindowUserPointer(window) );
    bgl->keyboard(key, scancode, action, mods);
}

void Resize_function(GLFWwindow* window,int width, int height) {
    BaseGL* bgl = reinterpret_cast<BaseGL*>( glfwGetWindowUserPointer(window) );
    bgl->width = width;
    bgl->height = height;
    bgl->resized();
}

int BaseGL::main() {
    /* Initialize the library */
    if (!glfwInit())
        return -1;
    
    bool fullscreen = false;
#ifdef NDEBUG 
	fullscreen = true;
#endif
    
    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow* window = 0;
    if(fullscreen) {
		glfwWindowHint(GLFW_DECORATED, GL_FALSE);
        const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        window = glfwCreateWindow(mode->width, mode->height, "Game of Life 3D", glfwGetPrimaryMonitor(), NULL);
    } else
        window = glfwCreateWindow(300,  600, "Game of Life 3D", NULL, NULL);
    
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    
    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, Keyboard_function);
    glfwSetFramebufferSizeCallback(window, Resize_function);
    
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        printf("Error: %s\n", glewGetErrorString(err));
        glfwTerminate();
        return -1;
    }
    
    glfwGetFramebufferSize(window, &width, &height);
    printf("framebuffer: %d, %d\n", width, height);
    init();
    
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        render();
        
        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        
        /* Poll for and process events */
        glfwPollEvents();
        
        update();
    }

    teardown();
    
    glfwTerminate();
    return 0;
}

void BaseGL::drawQuad(float x1, float y1, float x2, float y2) {
    glBegin (GL_QUADS);
    glTexCoord2f (0.0, 0.0);
    glVertex3f (x1, y1, 0.0);
    glTexCoord2f (1.0, 0.0);
    glVertex3f (x2, y1, 0.0);
    glTexCoord2f (1.0, 1.0);
    glVertex3f (x2, y2, 0.0);
    glTexCoord2f (0.0, 1.0);
    glVertex3f (x1, y2, 0.0);
    glEnd ();
}

#ifdef WIN32
#include <windows.h>
#endif

GLchar* readFile(const char* name) {
    std::ifstream t;
    std::streamoff length;
	t.open(name, std::ios::binary);
    t.seekg(0, std::ios::end);
    length = t.tellg();
    t.seekg(0, std::ios::beg);
    GLchar* buffer = new char[length+1];
    t.read(buffer, length);
    t.close();
    buffer[length]=0;
    return buffer;
}

void printShaderInfo(const char* name, GLuint shader) {
    GLint infoLen = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
    if(infoLen > 1) {
        char* infoLog = (char*)malloc(infoLen * sizeof(char));
        glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
        printf("ERROR COMPILING SHADER %s: %s", name, infoLog);
#ifdef WIN32
		OutputDebugStringA(name);
		OutputDebugStringA(infoLog);
#endif
        free(infoLog);
    }
}

GLuint BaseGL::loadShader(const char* fileNameVSH,const char* fileNameFSH,const char* fileNameGSH) {
	GLuint          program = glCreateProgram();
	
	GLuint          vertShader = 0;
	GLuint          fragShader = 0;
	GLuint          geomShader = 0;
	
	GLint           status;
	const   GLchar          *source;
	
    if(fileNameVSH) {
        source = readFile(fileNameVSH);
        
        vertShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertShader, 1, &source, NULL);
        glCompileShader(vertShader);
        
        glGetShaderiv(vertShader, GL_COMPILE_STATUS, &status);
        if (vertShader == 0 || status == 0)
        {
            printShaderInfo(fileNameVSH, vertShader);
            abort();
            return -1;
        }
    }
	
    if(fileNameFSH) {
        source = readFile(fileNameFSH);
        
        fragShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragShader, 1, &source, NULL);
        glCompileShader(fragShader);
        
        glGetShaderiv(fragShader, GL_COMPILE_STATUS, &status);
        if (fragShader == 0 || status == 0)
        {
            printShaderInfo(fileNameFSH, fragShader);
            abort();
            return -1;
        }
    }
    
	
    if(fileNameGSH) {
        source = readFile(fileNameGSH);
        
        geomShader = glCreateShader(GL_GEOMETRY_SHADER_EXT);
        glShaderSource(geomShader, 1, &source, NULL);
        glCompileShader(geomShader);
        
        glGetShaderiv(geomShader, GL_COMPILE_STATUS, &status);
        if (geomShader == 0 || status == 0)
        {
            printShaderInfo(fileNameGSH, geomShader);
            abort();
            return -1;
        }
    }
    
	if(vertShader) glAttachShader(program, vertShader);
	if(geomShader) glAttachShader(program, geomShader);
	if(fragShader) glAttachShader(program, fragShader);
    
    if (geomShader)
    {
        glProgramParameteriEXT(program, GL_GEOMETRY_INPUT_TYPE_EXT, GL_POINTS);
        glProgramParameteriEXT(program, GL_GEOMETRY_OUTPUT_TYPE_EXT, GL_TRIANGLE_STRIP);
        glProgramParameteriEXT(program, GL_GEOMETRY_VERTICES_OUT_EXT, 24);
    }
    
    
	glBindAttribLocation(program, 0, "fColor");
	glBindAttribLocation(program, 1, "fNormal");
	
	glLinkProgram(program);
	
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == 0) {
		printf("ERROR LINKING SHADER.");
		abort();
		return -1;
	}
	
	if (vertShader)
		glDeleteShader(vertShader);
    if (fragShader)
        glDeleteShader(fragShader);
    if (geomShader)
        glDeleteShader(geomShader);
    
    
#if TARGET_IPHONE_SIMULATOR || !TARGET_OS_IPHONE
    {
        GLint logLength, status;
        
        glValidateProgram(program);
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0)
        {
            GLchar *log = (GLchar *)malloc(logLength);
            glGetProgramInfoLog(program, logLength, &logLength, log);
            printf("Program validate log:\n%s", log);
            free(log);
        }
        
        glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
        if (status == GL_FALSE) {
            printf("Failed to validate program %d", program);
            abort();
        }
        
    }
#endif
	
	
	glUseProgram(0);
	return program;
}

bool BaseGL::gluInvertMatrix(const float m[16], float invOut[16])
{
    double inv[16], det;
    int i;
    
    inv[0] = m[5]  * m[10] * m[15] -
    m[5]  * m[11] * m[14] -
    m[9]  * m[6]  * m[15] +
    m[9]  * m[7]  * m[14] +
    m[13] * m[6]  * m[11] -
    m[13] * m[7]  * m[10];
    
    inv[4] = -m[4]  * m[10] * m[15] +
    m[4]  * m[11] * m[14] +
    m[8]  * m[6]  * m[15] -
    m[8]  * m[7]  * m[14] -
    m[12] * m[6]  * m[11] +
    m[12] * m[7]  * m[10];
    
    inv[8] = m[4]  * m[9] * m[15] -
    m[4]  * m[11] * m[13] -
    m[8]  * m[5] * m[15] +
    m[8]  * m[7] * m[13] +
    m[12] * m[5] * m[11] -
    m[12] * m[7] * m[9];
    
    inv[12] = -m[4]  * m[9] * m[14] +
    m[4]  * m[10] * m[13] +
    m[8]  * m[5] * m[14] -
    m[8]  * m[6] * m[13] -
    m[12] * m[5] * m[10] +
    m[12] * m[6] * m[9];
    
    inv[1] = -m[1]  * m[10] * m[15] +
    m[1]  * m[11] * m[14] +
    m[9]  * m[2] * m[15] -
    m[9]  * m[3] * m[14] -
    m[13] * m[2] * m[11] +
    m[13] * m[3] * m[10];
    
    inv[5] = m[0]  * m[10] * m[15] -
    m[0]  * m[11] * m[14] -
    m[8]  * m[2] * m[15] +
    m[8]  * m[3] * m[14] +
    m[12] * m[2] * m[11] -
    m[12] * m[3] * m[10];
    
    inv[9] = -m[0]  * m[9] * m[15] +
    m[0]  * m[11] * m[13] +
    m[8]  * m[1] * m[15] -
    m[8]  * m[3] * m[13] -
    m[12] * m[1] * m[11] +
    m[12] * m[3] * m[9];
    
    inv[13] = m[0]  * m[9] * m[14] -
    m[0]  * m[10] * m[13] -
    m[8]  * m[1] * m[14] +
    m[8]  * m[2] * m[13] +
    m[12] * m[1] * m[10] -
    m[12] * m[2] * m[9];
    
    inv[2] = m[1]  * m[6] * m[15] -
    m[1]  * m[7] * m[14] -
    m[5]  * m[2] * m[15] +
    m[5]  * m[3] * m[14] +
    m[13] * m[2] * m[7] -
    m[13] * m[3] * m[6];
    
    inv[6] = -m[0]  * m[6] * m[15] +
    m[0]  * m[7] * m[14] +
    m[4]  * m[2] * m[15] -
    m[4]  * m[3] * m[14] -
    m[12] * m[2] * m[7] +
    m[12] * m[3] * m[6];
    
    inv[10] = m[0]  * m[5] * m[15] -
    m[0]  * m[7] * m[13] -
    m[4]  * m[1] * m[15] +
    m[4]  * m[3] * m[13] +
    m[12] * m[1] * m[7] -
    m[12] * m[3] * m[5];
    
    inv[14] = -m[0]  * m[5] * m[14] +
    m[0]  * m[6] * m[13] +
    m[4]  * m[1] * m[14] -
    m[4]  * m[2] * m[13] -
    m[12] * m[1] * m[6] +
    m[12] * m[2] * m[5];
    
    inv[3] = -m[1] * m[6] * m[11] +
    m[1] * m[7] * m[10] +
    m[5] * m[2] * m[11] -
    m[5] * m[3] * m[10] -
    m[9] * m[2] * m[7] +
    m[9] * m[3] * m[6];
    
    inv[7] = m[0] * m[6] * m[11] -
    m[0] * m[7] * m[10] -
    m[4] * m[2] * m[11] +
    m[4] * m[3] * m[10] +
    m[8] * m[2] * m[7] -
    m[8] * m[3] * m[6];
    
    inv[11] = -m[0] * m[5] * m[11] +
    m[0] * m[7] * m[9] +
    m[4] * m[1] * m[11] -
    m[4] * m[3] * m[9] -
    m[8] * m[1] * m[7] +
    m[8] * m[3] * m[5];
    
    inv[15] = m[0] * m[5] * m[10] -
    m[0] * m[6] * m[9] -
    m[4] * m[1] * m[10] +
    m[4] * m[2] * m[9] +
    m[8] * m[1] * m[6] -
    m[8] * m[2] * m[5];
    
    det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];
    
    if (det == 0)
        return false;
    
    det = 1.0 / det;
    
    for (i = 0; i < 16; i++)
        invOut[i] = inv[i] * det;
    
    return true;
}




