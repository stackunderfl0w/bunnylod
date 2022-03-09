//
// Created by Pat on 3/25/2020.
//

#ifndef GL_SCENE_H
#define GL_SCENE_H
#include "model.h"
#include "object.h"

#include "glhelpers/shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;
struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct ShadowSpotLight{
    vec3 position;
    vec3 direction;

};

class scene{
public:
    vector<object*> objects;
    glm::vec3 position;
    vector<PointLight> plights;
    vector<ShadowSpotLight> slights;

    scene(){
        for (int i = 0; i < 4; ++i) {
            plights.push_back(PointLight());
        }
    }
    scene(glm::vec3 pos){
        position=pos;
    }
    void Draw(Shader shader)
    {
        GLuint last_array_buffer; glGetIntegerv(GL_ARRAY_BUFFER_BINDING, (GLint*)&last_array_buffer);

        shader.use();
        shader.setInt("point_lights_count",int(plights.size()));
        for (int i = 0; i < 4; i++)
        {
            string number = to_string(i);
            shader.setVec3("pointLights[" + number + "].position",plights[i].position);
            shader.setVec3("pointLights[" + number + "].ambient",0.05f, 0.05f, 0.05f);
            shader.setVec3("pointLights[" + number + "].diffuse",0.8f, 0.8f, 0.8f);
            shader.setVec3("pointLights[" + number + "].specular", 1.0f, 1.0f, 1.0f);
            shader.setFloat("pointLights[" + number + "].constant", 1.0f);
            shader.setFloat("pointLights[" + number + "].linear",0.09);
            shader.setFloat("pointLights[" + number + "].quadratic", 0.032);
        }
        for(unsigned int i = 0; i < objects.size(); i++)
            objects[i]->Draw(shader);
        glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);

    }
    void draw_lights(Shader lampShader, Model box){
        lampShader.use();
        //lampShader.setMat4("projection", projection);
        //lampShader.setMat4("view", view);

        for (unsigned int i = 0; i < 4; i++){
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, plights[i].position);
            model = glm::scale(model, glm::vec3(0.1f)); // Make it a smaller cube
            lampShader.setMat4("model", model);
            box.Draw(lampShader);
        }
    }
    void addObject(object *object){
        objects.push_back(object);
    }
private:
};
#endif //GL_SCENE_H
