#ifndef MARIO_ENEMY_BASE_HEADER
#define MARIO_ENEMY_BASE_HEADER
#include "game.h"
#include "mover.h"
#include "tileobject.h"
#include "tileplane.h"

namespace Game
{
    class EnemyBase : public Mover
    {
        public:
            EnemyBase();
            virtual ~EnemyBase();
            virtual void Draw(int xof, int yof);
            virtual void Step(double * nxp, double * nyp);
            virtual Interaction Interact(Behaviour b, Direction d){
              return interaction[b][d];
            }
            virtual BlockAction Act(Interaction action, int xp, int yp, double * nxp, double * nyp);
            virtual void StepTo(double nxp, double nyp) { x=nxp; y=nyp; }

        protected:
            bool left;
            static const Interaction interaction[BEHAVE_NUM][D_NUM];
    };
};

#endif

