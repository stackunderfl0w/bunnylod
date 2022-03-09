#include <iostream>

#ifdef __ANDROID__
#include <SDL.h>
#include <SDL_opengles2.h>
#include <android/log.h>
#include "glhelpers/shaders_gles.h"
#else
#include <glad/glad.h>
//#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL.h>
#include "glhelpers/shaders.h"
#endif

#ifdef NINTENDO_SWITCH
#include <switch.h>
#endif


#include <array>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glm/vec3.hpp>

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_sdl.h"
#include "imgui/backends/imgui_impl_opengl3.h"
//#include "tinyexpr/tinyexpr.h"

#include "platform.h"
#define STB_IMAGE_IMPLEMENTATION

#include "glhelpers/shaderloader.h"
#include "glhelpers/textureloader.h"
#include "glhelpers/shader.h"
#include "glhelpers/camera.h"
#include "rabdata.h"
#include "progmesh.h"
typedef std::chrono::high_resolution_clock Clock;
#define systime chrono::duration_cast<std::chrono::microseconds>(chrono::high_resolution_clock::now()-starttime).count()

Camera camera(glm::vec3(3.0f, 1.0f, 2.0f),
			  glm::vec3(0.0f, 1.0f, 0.0f),
			  215,
			  -15);


int SCR_WIDTH = 1280;
int SCR_HEIGHT = 720;

int sdl_interval_mode=1;
/*struct Vertex {
	// position
	glm::vec3 Position;
	// normal
	glm::vec3 Normal;
};*/
std::vector<glm::vec3> vert;
std::vector<glm::vec3> normals;       // global Array of normals
std::vector<glm::vec3>vrt;
std::vector<tridata> tri;       // global Array of triangles

std::vector<int> collapse_map;
int Map(int a,int mx) {
	if(mx<=0) return 0;
	while(a>=mx) {
		a=collapse_map[a];
	}
	return a;
}


int main(int argc, char *argv[]) {
    std::cout << "Hello, World!" << std::endl;

    if(argv[1]!=NULL){
        cout<<argv[1]<<endl;
    }
    else{
        //return 1;
    }

    platform::init();
    SDL_Window *window=platform::get_window();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    auto starttime = Clock::now();

    //platform::set_title("hello");
    //glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);

    unsigned long long deltatime;
    double fps;
    bool mouse_down = false;
    float ddpi, hdpi, vdpi;
    SDL_GetDisplayDPI(0,&ddpi,&hdpi,&vdpi);

#ifndef __ANDROID__
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
#endif

	//Shader uiShader(vertex2d,fragment2d);
	Shader uiShader(vertex2d,singlecolorfragment2d);
    uiShader.use();
	//Shader lightingShader(simplevertextransform,singlecolorfragment);
	Shader lightingShader(basiclightingvertextransform, basiclightingfragment);
	//lightingShader.use();
	for(int i=0;i<RABBIT_VERTEX_NUM;i++) {
		float *vp=rabbit_vertices[i];
		vrt.push_back(glm::vec3(vp[0],vp[1],vp[2]));
	}
	for(int i=0;i<RABBIT_TRIANGLE_NUM;i++) {
		tridata td;
		td.v[0]=rabbit_triangles[i][0];
		td.v[1]=rabbit_triangles[i][1];
		td.v[2]=rabbit_triangles[i][2];
		tri.push_back(td);
	}

	std::vector<int> permutation;

	std::vector<Vertex*> *vertices;
	std::vector<Triangle*> *triangles;

	auto time = std::chrono::high_resolution_clock::now();

	Init_mesh(vrt,tri,collapse_map,permutation,vertices,triangles);

	auto endtime=std::chrono::high_resolution_clock::now();

	cout<<chrono::duration_cast<chrono::microseconds>(endtime - time).count()<<endl;


	// rearrange the vertex Array
	std::vector<glm::vec3> temp_Array;
	assert(permutation.size() == vrt.size());
	for(int i = 0; i<vrt.size(); i++) {
		temp_Array.push_back(vrt[i]);
	}
	for(int i=0;i<vrt.size();i++) {
		vrt[permutation[i]]=temp_Array[i];
	}
	// update the changes in the entries in the triangle Array
	for(int i = 0; i<tri.size(); i++) {
		for(int j=0;j<3;j++) {
			tri[i].v[j] = permutation[tri[i].v[j]];
		}
	}
	/*for (int i = 0; i < tri.size(); ++i)
	{
		//cout<<tri[i].v[0]<<" "<<tri[i].v[1]<<" "<<tri[2].v[0]<<endl;
	}
	for (auto t:tri) {
		//printf("%d %d %d\n",t.v[0],t.v[1],t.v[2]);
	}*/


	int renderpolycount=0;
	int render_num=150;
	for (int i = 0; i < tri.size(); ++i) {
		int p0= Map(tri[i].v[0],render_num);
		int p1= Map(tri[i].v[1],render_num);
		int p2= Map(tri[i].v[2],render_num);
		// note:  serious optimization opportunity here,
		//  by sorting the triangles the following "continue"
		//  could have been made into a "break" statement.
		if(p0==p1 || p1==p2 || p2==p0) continue;
		renderpolycount++;

		vert.push_back(vrt[p0]);
		vert.push_back(vrt[p1]);
		vert.push_back(vrt[p2]);
		glm::vec3 dir=glm::cross((vrt[p1]-vrt[p0]),(vrt[p2]-vrt[p0]));
		for (int j = 0; j < 3; ++j) {
			normals.push_back(dir);
		}
	}
	//cout<<vert.size()<<endl;
	endtime=std::chrono::high_resolution_clock::now();

	cout<<"TIME ELAPSED "<<chrono::duration_cast<chrono::milliseconds>(endtime - time).count()<<endl;


	/*for (int i = 0; i < RABBIT_TRIANGLE_NUM; ++i) {
		vert.push_back(glm::vec3(rabbit_vertices[rabbit_triangles[i][0]][0],
									 rabbit_vertices[rabbit_triangles[i][0]][1],
									 rabbit_vertices[rabbit_triangles[i][0]][2]));
		vert.push_back(glm::vec3(rabbit_vertices[rabbit_triangles[i][1]][0],
									 rabbit_vertices[rabbit_triangles[i][1]][1],
									 rabbit_vertices[rabbit_triangles[i][1]][2]));
		vert.push_back(glm::vec3(rabbit_vertices[rabbit_triangles[i][2]][0],
									 rabbit_vertices[rabbit_triangles[i][2]][1],
									 rabbit_vertices[rabbit_triangles[i][2]][2]));
		glm::vec3 dir=glm::cross((vert[i*3+1]-vert[i*3]),(vert[i*3+2]-vert[i*3]));
		for (int j = 0; j < 3; ++j) {
			normals.push_back(dir);
		}


	}*/


	//PermuteVertices(permutation);

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vert.size() * sizeof(glm::vec3), &vert[0], GL_DYNAMIC_DRAW);

	GLuint normalbuffer;
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_DYNAMIC_DRAW);



	bool show_demo_window = false;
    ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);

    long long last_fps_update = 0;//systime
    long long last_frame_render = 0;//systime
    long long next_frame=0;
    long long last_frame[60];
    float deltaTime = 0.0f; // time between current frame and last frame
    float lastFrame = 0.0f;

    int max_fps=60;
    bool running=true;
    SDL_Event windowEvent;
    bool captureinput=false;
    float x=0,y=0;
    float scale=1.0;
    bool refresh=true;
    bool alt = false;

    while (running){
        SDL_GetWindowSize(window, &SCR_WIDTH, &SCR_HEIGHT);
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

        float ratio = (float)max(SCR_WIDTH,SCR_HEIGHT)/ (float)min(SCR_WIDTH,SCR_HEIGHT);
        long long framestart = systime;
        deltatime = (systime - last_frame[0]);
        float currentFrame = SDL_GetTicks()/1000.0;
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        //calc fps
        if(true){
            //next_frame = systime + 1000000 / (max_fps);
            fps = (1000000.0 / (systime - last_frame[59]) * 60);
            //cout<<systime - last_fps_update<<endl;
            if (systime - last_fps_update > 500000) {
                last_fps_update = systime;
                platform::set_title(to_string(fps).c_str());
                //cout<<to_string(fps).c_str()<<endl;
            }
            for (int i = 59; i > 0; --i) {
                last_frame[i] = last_frame[i - 1];
            }
            last_frame[0] = systime;
        }
		SDL_PumpEvents();
		const Uint8 *keysArray = const_cast <Uint8*> (SDL_GetKeyboardState(NULL));
		// Move forward
		if(captureinput){
			if (keysArray[SDL_SCANCODE_UP]|keysArray[SDL_SCANCODE_W])
				camera.ProcessKeyboard(FORWARD, deltaTime);

			if (keysArray[SDL_SCANCODE_DOWN]|keysArray[SDL_SCANCODE_S])
				camera.ProcessKeyboard(BACKWARD, deltaTime);

			if (keysArray[SDL_SCANCODE_LEFT]|keysArray[SDL_SCANCODE_A])
				camera.ProcessKeyboard(LEFT, deltaTime);

			if (keysArray[SDL_SCANCODE_RIGHT]|keysArray[SDL_SCANCODE_D])
				camera.ProcessKeyboard(RIGHT, deltaTime);

			if (keysArray[SDL_SCANCODE_RIGHT]|keysArray[SDL_SCANCODE_SPACE])
				camera.ProcessKeyboard(UP, deltaTime);

			if (keysArray[SDL_SCANCODE_RIGHT]|keysArray[SDL_SCANCODE_LSHIFT])
				camera.ProcessKeyboard(DOWN, deltaTime);
		}


        while(SDL_PollEvent(&windowEvent)) {
            ImGui_ImplSDL2_ProcessEvent(&windowEvent);
            if (windowEvent.type == SDL_QUIT) {
                running = false;
            }
            if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_ESCAPE){
                running=false;
            }
            if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_LALT){
                alt=!alt;
            }
            if (windowEvent.type == SDL_KEYDOWN &&
                windowEvent.key.keysym.sym == SDLK_TAB){
                captureinput=!captureinput;
                if(captureinput){
                    SDL_SetRelativeMouseMode(SDL_TRUE);
                    SDL_SetWindowGrab(window, SDL_TRUE);

                }
                else{
                    SDL_SetRelativeMouseMode(SDL_FALSE);
                    SDL_SetWindowGrab(window, SDL_FALSE);
                }

            }
            if(!io.WantCaptureMouse){
                if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
                    if(windowEvent.type == SDL_MOUSEMOTION){
                        refresh=true;
                        x+=1/scale*windowEvent.motion.xrel/SCR_WIDTH*2;
                        y-=1/scale*windowEvent.motion.yrel/SCR_HEIGHT*2;
                    }
                }

				if(captureinput){
					if(windowEvent.type == SDL_MOUSEMOTION){
						camera.ProcessMouseMovement(windowEvent.motion.xrel,0- windowEvent.motion.yrel);
						//cout<<windowEvent.motion.xrel<<" "<<windowEvent.motion.yrel<<endl;
					}
				}
				if(windowEvent.type == SDL_MOUSEWHEEL){
					camera.ProcessMouseScroll(windowEvent.wheel.y);
				}
				if((mouse_down&!io.WantCaptureMouse)){
					if(windowEvent.type == SDL_MOUSEMOTION){
						camera.ProcessMouseMovement(windowEvent.motion.xrel,0- windowEvent.motion.yrel);
						//cout<<windowEvent.motion.xrel<<" "<<windowEvent.motion.yrel<<endl;
					}
				}
            }
        }
        //uiShader.use();




		glm::mat4 projection;
		projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);

		//glm::mat4 view = glm::lookAt(glm::vec3(2.0f, 1.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 view = camera.GetViewMatrix();

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 MVP=projection*view*model;
		lightingShader.use();
		lightingShader.use();
		lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
		lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
		lightingShader.setVec3("lightPos", glm::vec3(2.0,2.0,2.0));
		lightingShader.setVec3("viewPos", glm::vec3(2.0f, 1.0f, 2.0f));

		// view/projection transformations
		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);

		// world transformation
		lightingShader.setMat4("model", model);
		//lightingShader.setMat4("MVP", MVP);


		glBindVertexArray(VertexArrayID);//fuck i have to do this shit

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
				0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
				3,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				0,                  // stride
				(void*)0            // array buffer offset
		);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);

		glVertexAttribPointer(
				1,
				3,
				GL_FLOAT
				, GL_FALSE,
				3 * sizeof(float),
				(void*)0
		);
		glDrawArrays(GL_TRIANGLES, 0, vert.size()); // 12*3 indices starting at 0 -> 12 triangles




        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();

        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
        //ImGui::Text("Position = %f %f %f", camera.Position.x, camera.Position.y,camera.Position.z);

        //ImGui::SliderFloat("scale", &scale,.5,2.0);
		ImGui::SliderInt("Polygons",&render_num,0,RABBIT_VERTEX_NUM);
		static int old_render_rum=render_num;
		if(render_num!=old_render_rum){
			old_render_rum=render_num;
			auto time = std::chrono::high_resolution_clock::now();

			vert.clear();
			normals.clear();
			for (int i = 0; i < tri.size(); ++i) {
				int p0= Map(tri[i].v[0],render_num);
				int p1= Map(tri[i].v[1],render_num);
				int p2= Map(tri[i].v[2],render_num);
				// note:  serious optimization opportunity here,
				//  by sorting the triangles the following "continue"
				//  could have been made into a "break" statement.
				if(p0==p1 || p1==p2 || p2==p0) continue;
				renderpolycount++;

				vert.push_back(vrt[p0]);
				vert.push_back(vrt[p1]);
				vert.push_back(vrt[p2]);
				glm::vec3 dir=glm::cross((vrt[p1]-vrt[p0]),(vrt[p2]-vrt[p0]));
				for (int j = 0; j < 3; ++j) {
					normals.push_back(dir);
				}
			}
			auto endtime=std::chrono::high_resolution_clock::now();

			//cout<<chrono::duration_cast<chrono::microseconds>(endtime - time).count()<<endl;

			glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
			glBufferData(GL_ARRAY_BUFFER, vert.size() * sizeof(glm::vec3), &vert[0], GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
			glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_DYNAMIC_DRAW);

		}


        ImGui::NewLine();ImGui::NewLine();ImGui::NewLine();

        ImGui::SliderInt("Swap interval",&sdl_interval_mode,-1,1);
        SDL_GL_SetSwapInterval(sdl_interval_mode);
		ImGui::Text("pos: %f %f %f", camera.Position.x, camera.Position.y,camera.Position.z);
		ImGui::Text("yaw: %f pitch: %f", camera.Yaw, camera.Pitch);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();

        ImGui::Render();

        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);


        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }
    return 0;
}





