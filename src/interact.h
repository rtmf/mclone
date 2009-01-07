#ifndef MARIO_INTERACT_HEADER
#define MARIO_INTERACT_HEADER
#include "mover.h"
#include "tileplane.h"
#include <list>
namespace Game
{
  class InteractionResult
  {
    public:
      InteractionResult(Interaction I, int X, int Y, TileObject * O)
      : i(I), x(X), y(Y), o(O) {};
      Interaction i;
      int x, y;
      TileObject * o;
  };
  typedef std::list <TilePlane*> :: iterator IPlane;
  typedef std::list <InteractionResult*> :: iterator IResult;
  void moveAndInteract(Mover * P, double nx, double ny, std::list <Game::TilePlane *> * PPlanes);
}
#endif
