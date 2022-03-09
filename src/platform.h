

#ifndef GLENGINE_PLATFORM_H
#define GLENGINE_PLATFORM_H

namespace platform{


    int init();

    SDL_Window* get_window();

    int deinit();

    void set_title(const char *title);
}


#endif //GLENGINE_PLATFORM_H
