//
// Created by Pat on 3/26/2020.
//

#ifndef GL_OBJECT_H
#define GL_OBJECT_H
#include "model.h"
#include "glhelpers/shader.h"
#include <glm/gtx/euler_angles.hpp>
#include <utility>
static int ids=0;

class object{

public:
    int id=0;
    glm::vec3 position=glm::vec3(0.0f);
    string name;
    glm::vec3 rotation=glm::vec3(0.0f);
    float scale=1.0;


    object(string name, Model &model0, glm::vec3 pos):model(model0) {
        this->name=std::move(name);
        position=pos;
        id=ids++;
        //cout<<(void*)&model<<endl;
    }
    object(string name, Model &model0):model(model0) {
        this->name=std::move(name);
        id=ids++;
        //cout<<(void*)&model<<endl;


    }
    void Draw(Shader shader)
    {
        glm::mat4 RotationMatrix = glm::eulerAngleYXZ(rotation.y, rotation.x, rotation.z);
        glm::mat4 modelmatrix = glm::mat4(1.0);
        modelmatrix = glm::translate(modelmatrix, position); // translate it down so it's at the center of the scene
        modelmatrix = glm::scale(modelmatrix, glm::vec3(0.1f*scale));	// it's a bit too big for our scene, so scale it down
        modelmatrix=modelmatrix*RotationMatrix;
        shader.setMat4("model",modelmatrix);
        model.Draw(shader);
        //cout<<"drawn";
    }
    void print(){
        model.print();
    }
    void setPosition(glm::vec3 pos){
        position=pos;
        //cout<<position.z;
    }
    void setRotaion(glm::vec3 rot){
        rotation=rot;
        //cout<<rot.x;
    }
    void setScale(float sc){
        scale=sc;
        //cout<<rot.x;
    }
    glm::vec3 getPosition()
    {
        return position;
    }
private:

    Model &model;

};
#endif //GL_OBJECT_H
