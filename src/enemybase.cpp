#include "interact.h"
#include "enemybase.h"
#include "game.h"
#include "sound.h"
#include "tileplane.h"
#include "tileobject.h"

namespace Game
{
    const Interaction EnemyBase::interaction[BEHAVE_NUM][D_NUM]=
    {
              //above below left right none
      {I_NONE,I_NONE,I_NONE,I_NONE,I_NONE}, // pass
      {I_STOP_ABOVE,I_STOP_BELOW,I_STOP_LEFT,I_STOP_RIGHT,I_EJECT}, // solid
      {I_STOP_ABOVE,I_STOP_BELOW,I_STOP_LEFT,I_STOP_RIGHT,I_EJECT}, // frown
      {I_STOP_ABOVE,I_NONE,I_NONE,I_NONE,I_NONE}, // platform
      {I_NONE,I_NONE,I_NONE,I_NONE,I_NONE}, // hurt
      {I_NONE,I_NONE,I_NONE,I_NONE,I_NONE}, // kill
      {I_NONE,I_NONE,I_NONE,I_NONE,I_NONE}, // coin
      {I_NONE,I_NONE,I_NONE,I_NONE,I_NONE}, // yoshi-top
      {I_NONE,I_NONE,I_NONE,I_NONE,I_NONE}, // yoshi-bottom
      {I_STOP_ABOVE,I_STOP_BELOW,I_STOP_LEFT,I_STOP_RIGHT,I_EJECT}, // vbounce
      {I_STOP_ABOVE,I_STOP_BELOW,I_STOP_LEFT,I_STOP_RIGHT,I_EJECT}, // hbounce
      {I_STOP_ABOVE,I_STOP_BELOW,I_STOP_LEFT,I_STOP_RIGHT,I_EJECT}, // bounce
      {I_NONE,I_NONE,I_NONE,I_NONE,I_NONE}, // item
      {I_NONE,I_NONE,I_NONE,I_NONE,I_NONE}, // donut
      {I_NONE,I_NONE,I_NONE,I_NONE,I_NONE}, // door
      {I_NONE,I_NONE,I_NONE,I_NONE,I_NONE}, // pipe
      {I_NONE,I_NONE,I_NONE,I_NONE,I_NONE}, // ice-cage
      {I_NONE,I_NONE,I_NONE,I_NONE,I_NONE}, // vine
      {I_NONE,I_NONE,I_NONE,I_NONE,I_NONE}, // fence
      {I_NONE,I_NONE,I_NONE,I_NONE,I_NONE}, // fence-door
      {I_STOP_ABOVE,I_STOP_BELOW,I_STOP_LEFT,I_STOP_RIGHT,I_EJECT}, // brick
      {I_STOP_ABOVE,I_STOP_BELOW,I_STOP_LEFT,I_STOP_RIGHT,I_EJECT}, // flip
      {I_NONE,I_NONE,I_NONE,I_NONE,I_NONE}, // flipping
    };

// TODO: REMOVE THIS SHIT
#define P_X_OFF 0
#define P_Y_OFF 0

    void EnemyBase::Draw(int xof, int yof)
    {
        DrawTile(int(x+P_X_OFF) +(left? 16:0) +xof, int(y+P_Y_OFF) +yof-32, 
                int(x+P_X_OFF) +(left? 0:16) +xof, int(y+P_Y_OFF) +yof-16, 
                512+8+32* /*(small?*/0/*:2)*/);
        DrawTile(int(x+P_X_OFF) +(left? 16:0) +xof, int(y+P_Y_OFF) +yof-16, 
                int(x+P_X_OFF) +(left? 0:16) +xof, int(y+P_Y_OFF) +yof, 
                512+8+32* /*(small?*/1/*:3)*/);
    }

    // TODO: Ask tom what the hell this routine is supposed to do
    BlockAction EnemyBase::Act(Interaction action, int xp, int yp, double *nxp, double *nyp)
    {
        return A_NONE;
    }

    void EnemyBase::Step(double *nxp, double *nyp)
    {
        *nxp += left?-0.1:0.1;
    }

    EnemyBase::EnemyBase()
    {
        x = 10;
        y = 6 * 16;
    }

    EnemyBase::~EnemyBase()
    {
    }
};

