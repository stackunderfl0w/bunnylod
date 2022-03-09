//
// Created by pat on 4/20/21.
//

#ifndef GLENGINE_SHADOW_H
#define GLENGINE_SHADOW_H
#include <glad/glad.h>
#include "glhelpers/shader.h"
#include "shadow-shaders.h"
#include "scene.h"
#include "glhelpers/camera.h"

GLuint quadVBO, quadUVVBO;
GLuint FramebufferName;
GLuint depthTexture;
Shader shadowmaper;
Shader debugDepthQuad;
Shader shadowrenderer;
Shader simpleshadows;
Shader complexshadows;
int SHADOW_WIDTH, SHADOW_HEIGHT=0;

bool setup_shadows(int SHADOW_W, int SHADOW_H){
    SHADOW_WIDTH=SHADOW_W;
    SHADOW_HEIGHT=SHADOW_H;
    shadowmaper.init(shadowmapvs, shadowmapfs);
    debugDepthQuad.init(debugdepthquadvs, debugdepthquadfs);
    shadowrenderer.init(shadowrendervs, shadowrenderfs);
    simpleshadows.init(simpleshadowvs,simpleshadowfs);
    complexshadows.init(complexshadowvs,complexshadowfs);
    cout<<"hi"<<endl;
    float quadCoords[] = {
            -0.1f, -0.1f, 0.0f,  // bottom left
            0.1f, -0.1f, 0.0f,  // bottom right
            0.1f,  0.1f, 0.0f,  // top right
            0.1f,  0.1f, 0.0f,  // top right
            -0.1f,  0.1f, 0.0f,   // top left
            -0.1f, -0.1f, 0.0f  // bottom left

    };
    float texCoords[] = {
            0.0f, 0.0f, // bottom left
            1.0f, 0.0f, // bottom right
            1.0f, 1.0f, // top right
            1.0f, 1.0f, // top right
            0.0f, 1.0f, // top left
            0.0f, 0.0f // bottom left

    };
    glGenBuffers(1, &quadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadCoords)* sizeof(float) , &quadCoords[0], GL_DYNAMIC_DRAW);

    glGenBuffers(1, &quadUVVBO);
    glBindBuffer(GL_ARRAY_BUFFER, quadUVVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords)* sizeof(float) , &texCoords[0], GL_DYNAMIC_DRAW);

    FramebufferName = 0;
    glGenFramebuffers(1, &FramebufferName);
    glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
    // Depth texture. Slower than a depth buffer, but you can sample it later in your shader
    depthTexture;
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);

    // No color output in the bound framebuffer, only depth.
    glDrawBuffer(GL_NONE);

    // Always check that our framebuffer is ok
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        cout<<"failed to create framebuffer"<<endl;
        return -1;
    }


    // shader configuration
    // --------------------
    debugDepthQuad.use();
    debugDepthQuad.setInt("depthMap", 0);
    debugDepthQuad.setFloat("near_plane", 1.0);
    debugDepthQuad.setFloat("far_plane", 7.5);
}


GLuint shadowmap(scene scene,glm::mat4 sVP,Camera camera){
    // 1. first render to depth map
    glEnable(GL_DEPTH_TEST);
    glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
    glViewport(0,0,SHADOW_WIDTH,SHADOW_HEIGHT); // Render on the whole framebuffer, complete from the lower left corner to the upper right
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    shadowmaper.use();
    shadowmaper.setMat4("VP",sVP);
    //shade3d.use();
    //shade3d.setMat4("MVP",MVP);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    scene.Draw(shadowmaper);
    glCullFace(GL_BACK);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);


}
void draw_depthmap(){
    debugDepthQuad.use();
    debugDepthQuad.setInt("depthMap",0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthTexture);


    //glBindVertexArray(0);//fuck i have to do this shit

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glVertexAttribPointer(
            0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
    );
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, quadUVVBO);
    glVertexAttribPointer(
            1,                                // attribute
            2,                                // size
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            (void*)0                          // array buffer offset
    );
    glDisable(GL_DEPTH_TEST);
    glDrawArrays(GL_TRIANGLES, 0, 6); // 12*3 indices starting at 0 -> 12 triangles

    glEnable(GL_DEPTH_TEST);
}

#endif //GLENGINE_SHADOW_H
