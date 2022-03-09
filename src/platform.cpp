#include <iostream>
#ifdef __ANDROID__
#include <SDL_opengles2.h>
#include <SDL.h>
#include <android/log.h>
#else
#include <glad/glad.h>
#include <SDL2/SDL.h>
#endif

#include "imgui.h"
#include "imgui/backends/imgui_impl_sdl.h"
#include "imgui/backends/imgui_impl_opengl3.h"

#include "platform.h"

namespace platform{
    int SCR_WIDTH = 1280;
    int SCR_HEIGHT = 720;
    SDL_Window *window;
    SDL_GLContext context;

    int init(){
#if !defined(__APPLE__) && !defined(__ANDROID__)
        gladLoadGL();
#endif
        if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
            SDL_Log("SDL_Init: %s\n", SDL_GetError());
            return -1;
        }
#ifdef __ANDROID__
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
#else
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
#endif
        window=SDL_CreateWindow("Stackunderfl0w opengl", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCR_WIDTH, SCR_HEIGHT, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
        context = SDL_GL_CreateContext(window);
        if (!window) {
            SDL_Log("SDL_CreateWindow: %s\n", SDL_GetError());
            SDL_Quit();
            return -1;
        }
#ifndef __ANDROID__
        if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return -1;
        }
        std::cout<<glGetString(GL_VERSION)<<std::endl;
#endif
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsClassic();
        ImGuiStyle& style = ImGui::GetStyle();
        SDL_GetWindowSize(window, &SCR_WIDTH, &SCR_HEIGHT);
        float uiscale=1.0;
#ifdef __ANDROID__
        uiscale=max(SCR_WIDTH,SCR_HEIGHT)/600;
#endif
        style.ScaleAllSizes(uiscale);
        io.FontGlobalScale = uiscale;
        ImGui_ImplSDL2_InitForOpenGL(window, context);
#ifdef __ANDROID__
        ImGui_ImplOpenGL3_Init("#version 100");
#else
        ImGui_ImplOpenGL3_Init("#version 330");
#endif

        return 0;
    }
    SDL_Window* get_window(){
        return window;
    }

    int deinit(){
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();

        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(window);
#ifndef __APPLE__ //deleting the context causes macos to freak out for some reason
        SDL_GL_DeleteContext(context);
        //printf("gl context deleted\n");
#endif
        SDL_Quit();
        return 0;
    }
    void set_title(const char *title){
        SDL_SetWindowTitle(window, title);

    }
}

