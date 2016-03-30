#ifndef GAME_H
#define GAME_H

// Define this symbol (VIRTUAL_MACHINE) if using a virtual machine
// Currently, vmware doesn't support OpenGL 3
// If unsure, install the mesa-utils package and then type
// glxinfo | grep "OpenGL" to get the OpenGL version information
// for your machine
#define VIRTUAL_MACHINE

#include "GlobalIncludes.h"

class GameState;

class Game
{
public:

    static Game* GetInstance();

    bool Initialize();
    bool LoadContent(GameState* state);
    void UnloadContent();
    void Update();
    void Draw();
    void EventHandler();
    void ChangeState(GameState* state);
    void PushState(GameState* state);
    void PopState();
    static void DestroyInstance();
    void ToggleFullScreen();

    // Member Accessors
    SDL_Event GetEvent() const;
    bool IsRunning() const;
    void StopRunning();
    SDL_Window* GetWindow() const;
    SDL_Surface* GetSurface() const;
    int GetScreenWidth() const;
    int GetScreenHeight() const;
    int GetMaxFPS() const;
    void SetMaxFPS(int newFPS);
    float GetAspectRatio() const;

    std::vector<GameState *> GameStates; //GameState stack

    std::chrono::time_point<std::chrono::high_resolution_clock> previousTime, currentTime;
    float GetTimeDelta();

private:

    static Game* inst;

    Game();

    ~Game()
    {
    };

    bool isRunning;
    bool isFullscreen;
    std::string caption;
    SDL_Window* window;
    SDL_Surface* screenSurface;
    SDL_Event event;
    SDL_GLContext context;

    int screenWidth;
    int screenHeight;

    int maxFPS;

    SDL_DisplayMode currentDisplayMode;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GameState
{
public:
    virtual ~GameState() {};

    virtual void Initialize() = 0;
    virtual void Finalize() = 0;

    virtual void Pause() = 0;
    virtual void Resume() = 0;

    virtual void HandleEvents() = 0;
    virtual void Draw() = 0;
    virtual void Update() = 0;

    static void ChangeState(GameState* state)
    {
        Game::GetInstance()->ChangeState(state);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif

