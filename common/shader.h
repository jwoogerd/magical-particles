#ifndef SHADER_H
#define SHADER_H

#include <GL/glut.h>
#include <string>

using namespace std;

class Shader
{
public:
    Shader(void);
    ~Shader(void);

    GLuint loadShader(const char* vertPath, const char* fragPath);
private:
    string readFile(const char* path);
};

#endif /* shader.h */