//
// Created by Pat on 4/4/2020.
//

#ifndef INC_3DGRAPHER_SHADERLOADER_H
#define INC_3DGRAPHER_SHADERLOADER_H
#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
using namespace std;

#include <stdlib.h>
#include <string.h>
//#include <android/log.h>
#define APPNAME "MyApp"

GLuint LoadShaders_text(const char * vertex_source,const char * fragment_sorce);
GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);
#endif //INC_3DGRAPHER_SHADERLOADER_H
