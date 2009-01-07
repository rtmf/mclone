#ifndef MARIO_MOVER_HEADER
#define MARIO_MOVER_HEADER
#include "game.h"
#include "tileobject.h"
#include "tileplane.h"
#include <list>
namespace Game
{
  class Mover
  {
    public:
      double x,y;
      int h,w;
      virtual Interaction Interact(Behaviour b, Direction d)=0;
          //[BEHAVE_NUM][D_NUM];
      virtual BlockAction Act(Interaction action, int xp, int yp, double * nxp, double * nyp)=0;
      virtual void Draw(int xof, int yof)=0;
      virtual void Step(double * nxp, double * nyp)=0;
      virtual void StepTo(double nxp, double nyp)=0;
      Mover();
      virtual ~Mover();
      friend void MoverFight(Mover *m1, Mover *m2);
  };
   
  void MoverFight(Mover *m1, Mover *m2);
}
#endif

