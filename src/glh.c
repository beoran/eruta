/** OpenGL helper functions */
#include "glh.h"
#include "mem.h"


GLuint glh_shader_make(GLenum type, char * data) {
  GLint status;
  GLuint shader = glCreateShader(type);
  
  glShaderSource(shader, 1, &data, NULL);
  glCompileShader(shader);
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  
  if (status == GL_FALSE)  {
    GLint logsize;
    const char *typename = "unknown";
    GLchar *log;
    
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logsize);
    log = mem_alloc(logsize);
    glGetShaderInfoLog(shader, logsize, NULL, log);
    
    switch(type)  {
    case GL_VERTEX_SHADER: typename = "vertex"; break;
    case GL_GEOMETRY_SHADER: typename = "geometry"; break;
    case GL_FRAGMENT_SHADER: typename = "fragment"; break;
    }

    fprintf(stderr, "Compile failure in %s shader:\n%s\n", typename, log);
    mem_free(log);
  }
  return shader;
}

GLuint gl_program_make(GLuint * shaders, int amount) {
  GLint status;
  int index;
  GLuint program = glCreateProgram();
  
  for(index = 0; index < amount; index++)  {
    glAttachShader(program, shaders[index]);
  }  

  glLinkProgram(program);
  
  glGetProgramiv(program, GL_LINK_STATUS, &status);
  if (status == GL_FALSE)  {
    GLint logsize;
    GLchar *log;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logsize);
    log = mem_alloc(logsize);
    
    glGetProgramInfoLog(program, logsize, NULL, log);
    fprintf(stderr, "Linker failure: %s\n", log);
    mem_free(log);
  }
  
  for(index = 0; index < amount; index++)  {
    glDetachShader(program, shaders[index]);
  }
  
  return program;
}
