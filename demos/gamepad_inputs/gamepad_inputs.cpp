#ifdef __EMSCRIPTEN__
  #include <GLES3/gl3.h> // web builds
#else
  #include <glad/glad.h> // desktop builds
#endif

#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

#include <iostream>

/* Set to 1 to enable gamepad events 0 otherwise */
#define USE_GAMEPAD_EVENTS 0
#define USE_GAMEPAD_POLLING 0
#define USE_KEYBOARD_POLLING 1

const char* WINDOW_NAME = "Gamepad Inputs Demo";
SDL_Window* win = NULL;
SDL_GLContext glContext;
const int WIDTH = 1280;
const int HEIGHT = 800;

// ASSUMPTION: only using 1 gamepad controller connected to device
// If multiple are needed, I'd need to use a container data structure to store them
SDL_Gamepad* gamepad = NULL;
const bool* keyStates = NULL;

const SDL_GamepadButton buttons[] = {
  SDL_GAMEPAD_BUTTON_SOUTH,
  SDL_GAMEPAD_BUTTON_EAST,
  SDL_GAMEPAD_BUTTON_WEST,
  SDL_GAMEPAD_BUTTON_NORTH,
  SDL_GAMEPAD_BUTTON_BACK,
  SDL_GAMEPAD_BUTTON_GUIDE,
  SDL_GAMEPAD_BUTTON_START,
  SDL_GAMEPAD_BUTTON_LEFT_STICK,
  SDL_GAMEPAD_BUTTON_RIGHT_STICK,
  SDL_GAMEPAD_BUTTON_LEFT_SHOULDER,
  SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER,
  SDL_GAMEPAD_BUTTON_DPAD_UP,
  SDL_GAMEPAD_BUTTON_DPAD_DOWN,
  SDL_GAMEPAD_BUTTON_DPAD_LEFT,
  SDL_GAMEPAD_BUTTON_DPAD_RIGHT,
};

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD);

  // Select GL version
  #if defined(IMGUI_IMPL_OPENGL_ES3)
    // GL ES 3.0 + GLSL 300 es (WebGL 2.0)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
  #elif defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100 (WebGL 1.0)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
  #else
    // GL 3.0 + generally GLSL 130
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
  #endif

  // Create window with graphics context
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  float mainScale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
  SDL_WindowFlags windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY; 
  win = SDL_CreateWindow(WINDOW_NAME, (int)(WIDTH * mainScale), (int)(HEIGHT * mainScale), windowFlags);


  glContext = SDL_GL_CreateContext(win);
  SDL_GL_MakeCurrent(win, glContext);
  SDL_GL_SetSwapInterval(1); // enable vsync
  SDL_SetWindowPosition(win, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
  SDL_ShowWindow(win);

  #ifndef __EMSCRIPTEN__
  // Setup Glad Library
  if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
    return SDL_APP_FAILURE;
  }
  #endif

  /* Verify hardware that GPU will be using */
  {
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Shading Language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
  }

#if USE_KEYBOARD_POLLING
  keyStates = SDL_GetKeyboardState(NULL);
#endif

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event* event) {
  if (event->type == SDL_EVENT_QUIT) {
    std::cout << "quitting app" << std::endl;
    return SDL_APP_SUCCESS;
  }

  if (event->type == SDL_EVENT_GAMEPAD_ADDED) {
    // used to obtain which gamepad was connected to the device
    const SDL_JoystickID which = event->gdevice.which;
    gamepad = SDL_OpenGamepad(which);
    if (!gamepad) {
      std::cout << "Gamepad " << which << " added but not opened: " << SDL_GetError() << std::endl;
    } else {
      // This obtains the controller hardware's controller configuration settings
      /*
        Example mapping string

        Gamepad 2 mapping: 0300fa675e040000ff02000000007801,*,a:b0,b:b1,back:b6,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b10,leftshoulder:b4,leftstick:b8,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b9,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b2,y:b3,platform:Windows,
      */
      char* mapping = SDL_GetGamepadMapping(gamepad);
      std::cout << "Gamepad " << which << " added! " << SDL_GetGamepadName(gamepad);
      if (mapping) {
        std::cout << "Gamepad " << which << " mapping: " << mapping << std::endl;
        SDL_free(mapping);
      }
    }

  } else if (event->type == SDL_EVENT_GAMEPAD_REMOVED) {
    // can obtain the gamepad joystick ID used to identify which controller is triggering the event
    const SDL_JoystickID which = event->gdevice.which;
    SDL_Gamepad* aGamepad = SDL_GetGamepadFromID(which);
    if (aGamepad) {
      // share same gamepad reference
      if (aGamepad == gamepad) {
        gamepad = NULL;
      }
      // gamepad unplugged
      SDL_CloseGamepad(aGamepad);
      aGamepad = NULL;
    }
    std::cout << "Gamepad " << which << " is removed" << std::endl;
  }
  
  /* Gamepad Input Events */
#if USE_GAMEPAD_EVENTS
    Uint32 gamepadEvents[] = {
      SDL_EVENT_GAMEPAD_BUTTON_UP,
      SDL_EVENT_GAMEPAD_BUTTON_DOWN,
      SDL_EVENT_GAMEPAD_AXIS_MOTION
    };

    bool hasGamepadStructs = false;
    for (Uint32 gamepadEvent : gamepadEvents) {
      if (event->type == gamepadEvent) {
        hasGamepadStructs = true;
        break;
      }
    }

    SDL_JoystickID which = 0;
    SDL_Gamepad* aGamepad = NULL;
    if (hasGamepadStructs) {
      which = event->gbutton.which;
      aGamepad = SDL_GetGamepadFromID(which);
    }

  if (event->type == SDL_EVENT_GAMEPAD_BUTTON_UP) {
    SDL_GamepadButton buttonReleased = (SDL_GamepadButton)event->gbutton.button;
    const char* buttonName = SDL_GetGamepadStringForButton(buttonReleased);
    std::cout << "Gamepad " << which << " button released " << buttonName << std::endl;
    // turn off controller rumble
    SDL_RumbleGamepad(aGamepad, 0x0000, 0x0000, 0);

  } else if (event->type == SDL_EVENT_GAMEPAD_BUTTON_DOWN) {
    SDL_GamepadButton buttonPressed = (SDL_GamepadButton)event->gbutton.button;
    const char* buttonName = SDL_GetGamepadStringForButton(buttonPressed);
    std::cout << "Gamepad " << which << " button pressed " << buttonName << std::endl;

    /* testing out the rumbling */
    // on wired xbox 360 controller the rumble either happens on the right or left side of the controller
    // high frequency rumble - right side of controller rumble felt the most
    // low frequency rumble - left side of controller rumble felt the most
    switch (buttonPressed) {
      case SDL_GAMEPAD_BUTTON_SOUTH:
        SDL_RumbleGamepad(aGamepad, 0xFFFF, 0x0000, 5000);
        std::cout << "low frequency rumble" << std::endl;
        break;
      case SDL_GAMEPAD_BUTTON_NORTH:
        SDL_RumbleGamepad(aGamepad, 0, 0xFFFF, 5000);
        std::cout << "high frequency rumble" << std::endl;
        break;
    }
  } else if (event->type == SDL_EVENT_GAMEPAD_AXIS_MOTION) {
    // joystick, analog stick, or trigger buttons
    const SDL_GamepadAxis axis = (SDL_GamepadAxis)event->gaxis.axis;
    const char* axisName = SDL_GetGamepadStringForAxis(axis);
    // ranges between -32768 to 32768 -- these are shorts
    // joystick up -32768 ; joystick down 32768
    // joystick left -32768 ; joystick right 32768
    int axisValue = (int)SDL_GetGamepadAxis(aGamepad, axis);
    std::cout << "Gamepad " << which << " axis: " << axisName << " | axis value: " << axisValue << "\n\n";
  }
#endif

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {

  /* Gamepad Polling */
#if USE_GAMEPAD_POLLING
  for (int i = 0; i < SDL_arraysize(buttons); ++i) {
    if (SDL_GetGamepadButton(gamepad, buttons[i])) {
      const char* buttonPressed = SDL_GetGamepadStringForButton(buttons[i]);
      std::cout << "button pressed is: " << buttonPressed << std::endl;
    }
  }
#endif

#if USE_KEYBOARD_POLLING
  if (keyStates[SDL_SCANCODE_W]) {
    std::cout << "physical W key pressed" << std::endl;
  }

  if (keyStates[SDL_SCANCODE_S]) {
    std::cout << "physical S key pressed" << std::endl;
  }
#endif

  /* Mouse Inputs */
  float x, y;
  SDL_MouseButtonFlags mouseFlags = SDL_GetMouseState(&x, &y);
  bool isLeftPressed = mouseFlags & SDL_BUTTON_MASK(SDL_BUTTON_LEFT);
  if (isLeftPressed) {
    std::cout << "left click" << std::endl;
    std::cout << "position: " << x << ", " << y << std::endl;
  }


  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
  if (glContext) {
    SDL_GL_DestroyContext(glContext);
  }

  if (win) {
    SDL_DestroyWindow(win);
  }

  if(gamepad) {
    SDL_CloseGamepad(gamepad);
    gamepad = NULL;
  }

  SDL_Quit();
  std::cout << "shutting down SDL3 game app" << std::endl;
}