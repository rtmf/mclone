// this is just a test!
#include "oglconsole.h"
#include "updater.h"
#include "game.h"
#include "sound.h"
#ifdef __MACH__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#endif
#include <SDL.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <iostream>
#define FPS 60

using namespace std;

SDL_Event event;

#include "enemybase.h"
#include "plumber.h"
#include "tileplane.h"
#include "mover.h"
namespace Game { extern list <Mover*> Movers; extern bool SaveMap(const string& filename); }
 //extern list <TilePlane*> Planes; };
Updater *updater = NULL;

void conCB(OGLCONSOLE_Console console, char *cmd)
{
    if (!strcmp(cmd,"plumb"))
    {
        OGLCONSOLE_Print("Creating a new plumber\n");
        Game::Movers.push_back((Game::Mover *)(new Game::Plumber()));
    }
    else if (!strcmp(cmd,"die"))
    {
        delete Game::Movers.back();
        Game::Movers.pop_back();
    }
    else if (!strcmp(cmd,"update"))
    {
	    if (updater) {
            OGLCONSOLE_Print("Interrupting previous update process\n");
            delete updater;
        }
        updater = new Updater();
    }
    else if (!strcmp(cmd,"mkfoe"))
    {
        OGLCONSOLE_Print("Creating a new enemy\n");
        Game::Movers.push_back((Game::Mover *)(new Game::EnemyBase()));
    }
    else if (!strncmp(cmd,"load ",5))
    {
        OGLCONSOLE_Print("Loading from filename %s\n", cmd+5);
        Game::LoadMap(cmd+5);
    }
    else if (!strncmp(cmd,"save ",5))
    {
        OGLCONSOLE_Print("Saving to filename %s\n", cmd+5);
        Game::SaveMap(cmd+5);
    }
    else OGLCONSOLE_Print("Unrecognized command :(\n");
}

int main(int argc, char **argv)
{
    int fps_counter = 0, fps_timer = 0;
    bool quit = false;

    srandom(time(NULL));

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) < 0)
    {
        printf("SDL_Init error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Joystick * JS;
    SDL_JoystickEventState(SDL_ENABLE);
    JS=SDL_JoystickOpen(0);
    printf("Opened joystick index %d\n", SDL_JoystickIndex(JS));

    if (SDL_SetVideoMode(1024, 768, 32,
                SDL_OPENGL | SDL_HWPALETTE | SDL_DOUBLEBUF) == 0)
    {
        printf("SDL_SetVideoMode error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, 0);
    glDisable(GL_DEPTH_TEST);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    OGLCONSOLE_Create();
    OGLCONSOLE_EnterKey(conCB);

    Sound :: Init();
    Game :: Init();
    Game :: DefaultGame();

    SDL_GL_SwapBuffers();

    int next_expected_frame_time = 0;

    while (!quit)
    {
        while (SDL_PollEvent(&event))
        {
            if (OGLCONSOLE_SDLEvent(&event)) continue;

            // The Game gets last dibs on these events
            switch (event.type)
            {
                case SDL_MOUSEMOTION:
                    Game :: Mouse(event.motion.x, event.motion.y);
                    break;

                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                    Game :: MouseButton(event.button.button,event.button.state);
                    break;
            
                case SDL_JOYAXISMOTION:
                    Game :: Jaxis(event.jaxis.axis, event.jaxis.value);
                    break;

                case SDL_JOYBUTTONDOWN:
                case SDL_JOYBUTTONUP:
                    if (Game :: Jbutton(event.jbutton.button,
                                event.jbutton.state)) continue;
                    break;

                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_ESCAPE
                            ||  event.key.keysym.sym == SDLK_q)
                        quit = true;

                    // TODO: Change this into an FPS display toggle?
                    else if (event.key.keysym.sym == SDLK_p)
                    {
                        int t = SDL_GetTicks();
                        double seconds = (t - fps_timer) / 1000.0;
                        double fps = fps_counter / seconds;

                        OGLCONSOLE_Print("%d frames in %g seconds = %g FPS\n",
                                fps_counter, seconds, fps);

                        fps_timer = t;
                        fps_counter = 0;
                    }

                case SDL_KEYUP:
                    /*else a temporary fix*/
                    Game :: Key(event.key.keysym.sym, event.key.state);
                    break;

                case SDL_QUIT: quit=true;
            }
        }

        // Tick game progress
        Game :: Step();

        // Run updater asynchronously
        if (updater)
        {
            updater->Work();
            if (updater->Done())
            {
                if (!updater->Restart())
                {
                    delete updater;
                    updater = NULL;
                    OGLCONSOLE_Print("Update complete!\n");
                }
                else quit = true;
            }
        }

        // If the current time is past the desired frame time, then we skip this
        // frame
        int t = SDL_GetTicks();
        if (t < next_expected_frame_time)
        {
            // Render the screen
            //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            Game :: Draw();
            OGLCONSOLE_Draw();

            // If the current time is before the desired frame time, then we
            // choke our performance here with a delay
            int u = SDL_GetTicks();
            if (u < next_expected_frame_time)
                // TODO: Put some kind of offset in here?
                SDL_Delay(next_expected_frame_time - u);

            // Determine the next expected frame time
            next_expected_frame_time += 1000 / FPS;

            // Flip screen buffers
            SDL_GL_SwapBuffers();
        }
        else
        {
            // Perhaps some diagnostic output should be made available here?
            // cout << "Below 60 FPS" << endl;

            // Determine the next expected frame time
            next_expected_frame_time += 1000 / FPS;
        }


        fps_counter++;
    }

    OGLCONSOLE_Quit();
    Game :: Quit();
    Sound :: Quit();
    SDL_Quit();

    // Restart?
    if (updater)
    {
        if (updater->Restart())
        {
            delete updater;
            execv(argv[0], argv);
            puts("execv() failed");
        }
        delete updater;
    }

    return 0;
}

