#pragma once

#include <GL/glew.h>
#include <SDL2/SDL.h> 
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_timer.h> 


#include "import/imgui/imgui_draw.cpp"
#include "import/imgui/imgui_widgets.cpp"
#include "import/imgui/imgui.cpp"
#include "import/imgui/imgui_impl_sdl.cpp"
#include "import/imgui/imgui_impl_opengl3.cpp"

#include "cp_lib/basic.cc"
#include "cp_lib/vector.cc"

#include "game.cc"

using namespace cp;

void gl_debug_callback(GLenum source,
            GLenum type, GLuint id,
            GLenum severity, GLsizei length,
            const GLchar *message, const void *userParam) 
{
    puts(message);
}

int main()
{

    window_size = {1280, 720};

    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window* window = SDL_CreateWindow("GPU Graphics", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_size.x, window_size.y, SDL_WINDOW_OPENGL);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GLContext gr_context = SDL_GL_CreateContext(window);

    if (glewInit() != GLEW_OK) {
        puts("Glew init failed!");
    }

    // enable debug stuff
    puts((char*)glGetString(GL_VERSION));

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(gl_debug_callback, null);
    //
    
    // gui init

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplSDL2_InitForOpenGL(window, &gr_context);
    ImGui_ImplOpenGL3_Init("#version 440 core");
    //


    game_init();


    while (is_running) {
        for (u32 i = 0; i < Input::keys_down.cap; i++) {
            Input::keys_down.buffer[i] = 0;
            Input::keys_up.buffer[i] = 0;
        }

        // Process events.
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            switch(event.type)
            {
                case SDL_QUIT: is_running = false; break;
                case SDL_KEYDOWN: {
                    if (event.key.keysym.sym < KEY_COUNT) {
                        set_bit_high(Input::keys_down, event.key.keysym.sym);
                        set_bit_high(Input::keys_hold, event.key.keysym.sym);
                    }
                } break;
                case SDL_KEYUP: {
                    if (event.key.keysym.sym < KEY_COUNT) {
                        set_bit_high(Input::keys_up, event.key.keysym.sym);
                        set_bit_low(Input::keys_hold, event.key.keysym.sym);
                    }
                } break;
                // case SDL_MOUSEBUTTONDOWN: {
                //     if (event.key.keysym.sym < MOUSE_BUTTON_COUNT) {
                //         set_bit_high(Input::mouse_button_down, event.key.keysym.sym);
                //         set_bit_low(Input::mouse_button_hold, event.key.keysym.sym);
                //     }
                // } break;
                // case SDL_MOUSEBUTTONUP: {
                //     if (event.key.keysym.sym < MOUSE_BUTTON_COUNT) {
                //         set_bit_high(Input::mouse_button_up, event.key.keysym.sym);
                //         set_bit_low(Input::mouse_button_hold, event.key.keysym.sym);
                //     }
                // } break;
                case SDL_MOUSEMOTION: {
                    Input::mouse_pos = { event.motion.x, event.motion.y };
                } break;
            }

            // gui events
            // ImGui_ImplSDL2_ProcessEvent(&event);
        }

        // render gui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();        

        game_update();
        draw_gui();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        //

        SDL_GL_SwapWindow(window);
        SDL_Delay(1000/60);
    }

    game_shut();

    // gui cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gr_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}