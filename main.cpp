#include <SDL3/SDL.h>
#include <iostream>

const char* WINDOW_NAME = "Game Math Gym";

int main(int argc, char* argv[]) {
  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window* win = SDL_CreateWindow(WINDOW_NAME, 640, 480, 0);
  

  SDL_Event e;
  bool quit = false;
  while (!quit) {
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_EVENT_QUIT) {
        quit = true;
      }
    }
  }

  SDL_DestroyWindow(win);
  SDL_Quit();

  return 0;
}