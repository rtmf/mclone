#include "interact.h"
#include "plumber.h"
#include "game.h"
#include "sound.h"
#include "tileplane.h"
#include "tileobject.h"
#include <iostream>
#include <math.h>
#define REG_JUMP 21
#define RUN_JUMP 3
#define NOTE_BOUNCE 5
#define JUMP_SIZE 0.30 
#define ACCEL_X 0.1
#define ACCEL_Y 0.15
#define TERM_Y_VEL 6
#define TERM_X_NORUN_VEL 2.0
#define TERM_X_RUN_VEL 3.0
#define NON_ICE_FRICT 0.150
#define GAP_VEL_LIMIT 2
#define GAP_FUDGE_LIMIT 2
#define BOUNCE_TIMEOUT 4


#define BLKLEFT (((*i)->x)*16-P_WIDTH)
#define BLKRIGHT (((*i)->x+1)*16)
#define BLKABOVE (((*i)->y)*16-1)
#define BLKBELOW (((*i)->y+1)*16+P_HEIGHT)
#define EJECT_FORCE 0.5

using namespace std;
namespace Game
{
    const Interaction Plumber::interaction[BEHAVE_NUM][D_NUM]=
    {
              //above below left right none
      {I_NONE,I_NONE,I_NONE,I_NONE,I_NONE}, // pass
      {I_STOP_ABOVE,I_STOP_BELOW,I_STOP_LEFT,I_STOP_RIGHT,I_EJECT}, // solid
      {I_STOP_ABOVE,I_STOP_BELOW,I_STOP_LEFT,I_STOP_RIGHT,I_EJECT}, // frown
      {I_STOP_ABOVE,I_NONE,I_NONE,I_NONE,I_NONE}, // platform
      {I_HURT,I_HURT,I_HURT,I_HURT,I_KILL}, // hurt
      {I_KILL,I_KILL,I_KILL,I_KILL,I_KILL}, // kill
      {I_COIN,I_COIN,I_COIN,I_COIN,I_COIN}, // coin
      {I_NONE,I_NONE,I_NONE,I_NONE,I_NONE}, // yoshi-top
      {I_NONE,I_NONE,I_NONE,I_NONE,I_NONE}, // yoshi-bottom
      {I_BOUNCE_ABOVE,I_BOUNCE_BELOW,I_STOP_LEFT,I_STOP_RIGHT,I_EJECT}, // vbounce
      {I_STOP_ABOVE,I_STOP_BELOW,I_BOUNCE_LEFT,I_BOUNCE_RIGHT,I_EJECT}, // hbounce
      {I_BOUNCE_ABOVE,I_BOUNCE_BELOW,I_BOUNCE_LEFT,I_BOUNCE_RIGHT,I_EJECT}, // bounce
      {I_STOP_ABOVE,I_ITEM_BELOW,I_STOP_LEFT,I_STOP_RIGHT,I_EJECT}, // item
      {I_NONE,I_NONE,I_NONE,I_NONE,I_NONE}, // donut
      {I_NONE,I_NONE,I_NONE,I_NONE,I_NONE}, // door
      {I_NONE,I_NONE,I_NONE,I_NONE,I_NONE}, // pipe
      {I_NONE,I_NONE,I_NONE,I_NONE,I_NONE}, // ice-cage
      {I_NONE,I_NONE,I_NONE,I_NONE,I_NONE}, // vine
      {I_NONE,I_NONE,I_NONE,I_NONE,I_NONE}, // fence
      {I_NONE,I_NONE,I_NONE,I_NONE,I_NONE}, // fence-door
      {I_STOP_ABOVE,I_BIGBREAK_BELOW,I_STOP_LEFT,I_STOP_RIGHT,I_EJECT}, // brick
      {I_SPINBREAK_ABOVE,I_FLIP_BELOW,I_STOP_LEFT,I_STOP_RIGHT,I_EJECT}, // flip
      {I_NONE,I_NONE,I_NONE,I_NONE,I_NONE}, // flipping
    };
    void Plumber :: Key(Keynum key, bool press)
    {
        switch(key)
        {
            case CROUCH:
                crouching=press;
                break;
            case LEFT:
                if (press) left=true;
                ax=press?-ACCEL_X:ax==-ACCEL_X?0:ax;
                break;
            case RIGHT:
                if (press) left=false;
                ax=press?ACCEL_X:ax==ACCEL_X?0:ax;
                break;
            case RUN:
                txv=press?TERM_X_RUN_VEL:TERM_X_NORUN_VEL;
                break;
            case SPINJUMP:
                if (press && !jumping)
                {
                    jumping=true;
                    spinning=true;
                    Sound::Play(Sound::SpinJump);
                    jumpthrust=int(REG_JUMP+(fabs(vx)>TERM_X_NORUN_VEL?
                            ((fabs(vx)-TERM_X_NORUN_VEL)/
                             (TERM_X_RUN_VEL-TERM_X_NORUN_VEL))*RUN_JUMP
                            :0));
                    vy=0;
                    bouncetimer=0;
                }
            case JUMP:
                holding_jump=press;
                if (press && (!jumping || bouncetimer))
                {
                    jumping=true;
                    //vy=-4.3-(fabs(vx)>TERM_X_NORUN_VEL?((fabs(vx)-TERM_X_NORUN_VEL)/
                    //            (TERM_X_RUN_VEL-TERM_X_NORUN_VEL))*0.5:0);
                    if (bouncetimer)
                        Sound::Play(Sound::Spring,3);
                    else
                        Sound::Play(Sound::Jump,3);
                    jumpthrust=int(REG_JUMP+(fabs(vx)>TERM_X_NORUN_VEL?
                            ((fabs(vx)-TERM_X_NORUN_VEL)/
                             (TERM_X_RUN_VEL-TERM_X_NORUN_VEL))*RUN_JUMP
                            :0)+(bouncetimer?NOTE_BOUNCE:0));
                    if (!bouncetimer) vy=0;
                    bouncetimer=0;
                }
                else if (!press && jumping)
                {
                    jumpthrust=0;
                }
            default:
                break;
        }
    }
    void Plumber :: Draw(int xof, int yof)
    {
        if (spinning)
        {
            spinface++;
            spinface%=8;
            DrawTile(int(x+P_X_OFF) +(left? 16:0) +xof, int(y+P_Y_OFF) +yof-32, 
                     int(x+P_X_OFF) +(left? 0:16) +xof, int(y+P_Y_OFF) +yof-16, 
                     512+((spinface/2)%2)*(spinface>3?2:1)+32*(small?0:2));
            DrawTile(int(x+P_X_OFF) +(left? 16:0) +xof, int(y+P_Y_OFF) +yof-16, 
                     int(x+P_X_OFF) +(left? 0:16) +xof, int(y+P_Y_OFF) +yof, 
                     512+((spinface/2)%2)*(spinface>3?2:1)+32*(small?1:3));
        }
        else  if (ducking)
        {
            DrawTile(int(x+P_X_OFF) +(left? 16:0) +xof, int(y+P_Y_OFF) +yof-32, 
                     int(x+P_X_OFF) +(left? 0:16) +xof, int(y+P_Y_OFF) +yof-16, 
                     512+6+32*(small?0:2));
            DrawTile(int(x+P_X_OFF) +(left? 16:0) +xof, int(y+P_Y_OFF) +yof-16, 
                     int(x+P_X_OFF) +(left? 0:16) +xof, int(y+P_Y_OFF) +yof, 
                     512+6+32*(small?1:3));
        }
        else if (jumping && vy <=0)
        {
            DrawTile(int(x+P_X_OFF) +(left? 16:0) +xof, int(y+P_Y_OFF) +yof-32, 
                     int(x+P_X_OFF) +(left? 0:16) +xof, int(y+P_Y_OFF) +yof-16, 
                     512+4+32*(small?0:2));
            DrawTile(int(x+P_X_OFF) +(left? 16:0) +xof, int(y+P_Y_OFF) +yof-16, 
                     int(x+P_X_OFF) +(left? 0:16) +xof, int(y+P_Y_OFF) +yof, 
                     512+4+32*(small?1:3));
        }
        else if (jumping && vy > 0)
        {
            DrawTile(int(x+P_X_OFF) +(left? 16:0) +xof, int(y+P_Y_OFF) +yof-32, 
                     int(x+P_X_OFF) +(left? 0:16) +xof, int(y+P_Y_OFF) +yof-16, 
                     512+5+32*(small?0:2));
            DrawTile(int(x+P_X_OFF) +(left? 16:0) +xof, int(y+P_Y_OFF) +yof-16, 
                     int(x+P_X_OFF) +(left? 0:16) +xof, int(y+P_Y_OFF) +yof, 
                     512+5+32*(small?1:3));
        }
        else if ((left && vx<=0)||(!left&&vx>=0))
        {
            DrawTile(int(x+P_X_OFF) +(left? 16:0) +xof, int(y+P_Y_OFF) +yof-32, 
                     int(x+P_X_OFF) +(left? 0:16) +xof, int(y+P_Y_OFF) +yof-16, 
                     512+(step?0:3)+32*(small?0:2));
            DrawTile(int(x+P_X_OFF) +(left? 16:0) +xof, int(y+P_Y_OFF) +yof-16, 
                     int(x+P_X_OFF) +(left? 0:16) +xof, int(y+P_Y_OFF) +yof, 
                     512+(step?0:3)+32*(small?1:3));
        }
        else
        {
            DrawTile(int(x+P_X_OFF) +(left? 16:0) +xof, int(y+P_Y_OFF) +yof-32, 
                     int(x+P_X_OFF) +(left? 0:16) +xof, int(y+P_Y_OFF) +yof-16, 
                     512+8+32*(small?0:2));
            DrawTile(int(x+P_X_OFF) +(left? 16:0) +xof, int(y+P_Y_OFF) +yof-16, 
                     int(x+P_X_OFF) +(left? 0:16) +xof, int(y+P_Y_OFF) +yof, 
                     512+8+32*(small?1:3));
        }
    }

    void Plumber :: Step(double * nxp, double * nyp)
    {
        if (ducking!=crouching && !jumping)
        {
            ducking=crouching;
            h=ducking|small?12:20;
        }

        double nx,ny;
        
        // Accelerate horizontally
        if (!ducking || jumping) vx += ax;

        //if (y>=6*16) vx*=NON_ICE_FRICT_RECIP;

        // Slide to a stop
        if ((ax==0 || ducking) && !jumping)
        {
          double f=NON_ICE_FRICT;
          if (vx>0)
          {
            vx-=f;
            if (vx<0) vx=0;
          }
          if (vx<0)
          {
            vx+=f;
            if (vx>0) vx=0;
          }
        }

        // Cap velocity
        if      (vx >  txv)  vx =  txv;
        else if (vx < -txv)  vx = -txv;

        //TODO?? If we've reached a halt, set animation data to reflect so
        if (vx==0) 
        {
          stepoff=-1;
          step=true;
        }

        //TODO?? If we're walking, animate
        else
        {
          stepoff++;
          if (stepoff>4) 
          {
            stepoff=0;
            step=!step;
          }
        }

        // Move horizontally
        nx = x+vx;

        // Hit the wall at the edge of the stage TODO: Implement some kind of
        // level barrier specifically: to stop the world from scrolling beyond,
        // and to stop mario from walking beyond; the edge of the stage?
        if (nx<=0) {nx=0;vx=0;}

        if (bouncetimer)
        {
            if (holding_jump)
            {
                jumping=true;
                Sound::Play(Sound::Spring,3);
                jumpthrust=int(REG_JUMP+(fabs(vx)>TERM_X_NORUN_VEL?
                        ((fabs(vx)-TERM_X_NORUN_VEL)/
                         (TERM_X_RUN_VEL-TERM_X_NORUN_VEL))*RUN_JUMP
                        :0)+NOTE_BOUNCE);
                bouncetimer=0;
            }
            else
                if (!--bouncetimer)
                    Sound::Play(Sound::Bounce,3);
        }


        // Upward thrust into the air
        if (jumping && jumpthrust>0)
        {
          ny=y-jumpthrust*JUMP_SIZE;
          jumpthrust--;
          vy=0;
        }

        // Falling
        if ((jumping && jumpthrust==0) || !jumping)
        {
          ny = y+vy;
          vy+=ACCEL_Y;
          if (vy>TERM_Y_VEL)
            vy=TERM_Y_VEL;
            //if (y>=6*16)
            //{
            //    y=6*16;
            //    jumping=false;
            //}
        }
        //moveAndInteract(this,nx,ny); // handle moving this to (nx,ny)
        *nxp=nx;
        *nyp=ny;
    }
#define SETLEFT *nxp=xp*16-w
//(((*i)->x)*16-P_WIDTH)
//#define SETLEFT *nxp=x
#define SETRIGHT *nxp=(xp+1)*16
//(((*i)->x+1)*16)
//#define SETRIGHT *nxp=x
#define SETABOVE *nyp=yp*16-1
//(((*i)->y)*16-1)
//#define SETABOVE *nyp=y
#define SETBELOW *nyp=(yp+1)*16+h
//(((*i)->y+1)*16+P_HEIGHT)
//#define SETBELOW *nyp=y

    BlockAction Plumber :: Act(Interaction action, int xp, int yp, double * nxp, double * nyp)
    {
      switch(action)
      {
        case I_EJECT:
          jumping=false;
          spinning=false;
          //vx=-EJECT_FORCE;
          //vy=EJECT_FORCE;
          x--;
          *nxp=x;
          bouncetimer=0;
          jumpthrust=0;
          return A_NONE;
        case I_COIN:
          return A_COIN;
        case I_STOP_LEFT:
          vx=0;
          SETLEFT;
          return A_NONE;
        case I_BOUNCE_LEFT:
          vx=-3;
          SETLEFT;
          return A_BOUNCE_LEFT;
        case I_STOP_RIGHT:
          vx=0;
          SETRIGHT;
          return A_NONE;
        case I_BOUNCE_RIGHT:
          vx=3;
          SETRIGHT;
          return A_BOUNCE_RIGHT;
        case I_BOUNCE_BELOW:
          vy=3;
          jumpthrust=0;
          SETBELOW;
          Sound::Play(Sound::Hit,1);
          return A_BOUNCE_BELOW;
        case I_STOP_BELOW:
          vy=0;
          jumpthrust=0;
          SETBELOW;
          Sound::Play(Sound::Hit,1);
          return A_NONE;
        case I_BIGBREAK_BELOW:
          vy=small?3:0;
          jumpthrust=0;
          SETBELOW;
          Sound::Play(Sound::Hit,1);
          return small?A_BOUNCE_BELOW:A_BREAK;
        case I_BOUNCE_ABOVE:
          vy=-3;
          bouncetimer=BOUNCE_TIMEOUT;
          if (ducking!=crouching)
          {
            ducking=crouching;
          }
          jumping=false;
          spinning=false;
          jumpthrust=0;
          SETABOVE;
          return A_BOUNCE_ABOVE;
        case I_SPINBREAK_ABOVE:
          SETABOVE;
          if (spinning && !small)
          {
            vy=-2;
            jumping=true;
            spinning=true;
            jumpthrust=0;
            return A_BREAK;
          }
          else
          {
            vy=0;
            jumping=false;
            spinning=false;
            jumpthrust=0;
          }
          return A_NONE;
        case I_STOP_ABOVE:
          SETABOVE;
          vy=0;
          jumping=false;
          spinning=false;
          jumpthrust=0;
          return A_NONE;
        case I_ITEM_BELOW:
          Sound::Play(Sound::Hit,1);
          SETBELOW;
          vy=0;
          jumpthrust=0;
          return A_GET_ITEM;
        case I_NONE:
          return A_NONE;
        case I_HURT:
          return A_NONE;
        case I_KILL:
          return A_NONE;
        case I_FLIP_BELOW:
          Sound::Play(Sound::Hit,1);
          SETBELOW;
          vy=0;
          jumpthrust=0;
          return A_FLIP_BEGIN;
        default:
          break;
      }
      return A_NONE;
    }

    Plumber :: Plumber()
        : Mover()
    {
        crouching = false;
        jumping = true;
        falling = false;
        left = false;
        runfire = false;
        moving = false;
        step = false;
        stepoff = 0;
        spinning = false;
        spinface = 0;
        jumpthrust = 0;
        ax = 0;
        bouncetimer = 0;
        ducking=false;
        small=true;


        vx = 0;
        vy = 0;
        x = 20;
        y = 6*16;
        h=ducking|small?12:20;
        w=8;
        txv = TERM_X_NORUN_VEL;
    }

    Plumber :: ~Plumber()
    {
    }
};

