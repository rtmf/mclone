#ifndef MARIO_PLUMBER_HEADER
#define MARIO_PLUMBER_HEADER
#include "game.h"
#include "mover.h"
#include "tileobject.h"
#include "tileplane.h"
#include <list>
//TODO make these member vars
#define P_X_OFF -4
#define P_Y_OFF 3 //(ducking?-4:8) 

namespace Game
{
    class Plumber: public Mover
    {
        public:
            double vx, vy;      // Velocity
            double ax;          // Acceleration
            double txv;         // Terminal velocity
            bool crouching;
            bool jumping;
            bool falling;
            bool left;          // facing left?
            bool runfire;       // holding B button?
            bool moving;
            bool step;
            int stepoff;        // graphical data
            bool holding_jump;
            bool ducking;
            bool small;
            bool spinning;
            int spinface;       // graphical data
            int bouncetimer;
            int jumpthrust;

            Plumber();
            virtual ~Plumber();
            virtual void Draw(int xof, int yof);
            virtual void Step(double * nxp, double * nyp);
            virtual void Key(Keynum key, bool press);
            virtual Interaction Interact(Behaviour b, Direction d)
            {
              return interaction[b][d];
            };
            virtual BlockAction Act(Interaction action, int xp, int yp, double * nxp, double * nyp);
            virtual void StepTo(double nxp, double nyp)
            {
              x=nxp;
              if (int(y)!=int(nyp))
                jumping=true;
              y=nyp;
            }
            
      private:
            static const Interaction interaction[BEHAVE_NUM][D_NUM];
    };
};

#endif

