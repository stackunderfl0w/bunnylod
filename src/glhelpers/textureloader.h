//
// Created by Pat on 3/14/2020.
//

#ifndef GL_TEXTURELOADER_H
#define GL_TEXTURELOADER_H
#include <stb/stb_image.h>
#include <glad/glad.h>

GLuint loadtexture(const char *file_name,bool flip, int *WIDTH= nullptr, int *HEIGHT = nullptr);

GLuint loadtexture(const char *file_name, bool flip, unsigned char *&pixels, int *WIDTH = nullptr, int *HEIGHT = nullptr);

GLuint loadtexture_no_filter(const char *file_name,bool flip, int *WIDTH= nullptr, int *HEIGHT = nullptr);

GLuint loadtexture_transparent(const char *file_name);

GLuint loadtexture_transparent_no_filter(const char *file_name,bool flip);


#endif //GL_TEXTURELOADER_H
