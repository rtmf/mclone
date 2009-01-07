#include "game.h"
#include "sound.h"
#include "faller.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
using namespace std;
#include "tileobject.h"
#ifdef __MACH__
#  include <OpenGL/gl.h>
#else
#  include <GL/gl.h>
#endif

#define FLIP_TIMEOUT 128
#define BRICK_TIMEOUT 128
#define ITEM_VIS_TIMEOUT 8
#define BOUNCE_VIS_TIMEOUT 8
namespace Game
{
    void TileObject :: Step ()
    {
        framecount++;
        if (framecount>=24) framecount=0;
        if (framecount%6==0)
        {
            if (Visual>=480 && Visual<500)
            {
                Visual=(Visual&(~3))|(framecount/6);
            }
        }
        if (SpecialCounter)
        {
            SpecialCounter--;
            if (SpecialVisual && !SpecialCounter)
            {
              if (SpecialVisual==VIS_FLIP)
              {
                Behavior=BEHAVE_FLIP;
              }
                if (SpecialVisual==VIS_ITEM_BOX)
                    Visual=7*32+8;
                SpecialVisual=VIS_NONE;
            }
        }
    }
    void TileObject :: Act(BlockAction action, int x, int y)
    {
      switch(action)
      {
        case A_BREAK:
          Sound::Play(Sound::Brick,3);
          Behavior=BEHAVE_PASS;
          Fallers.push_back(new Faller(x*16,y*16,-1,-6,0,0.5,8,8,Visual,200));
          Fallers.push_back(new Faller(x*16+8,y*16,1,-6,0,0.5,8,8,Visual,200));
          Fallers.push_back(new Faller(x*16,y*16+8,-1.1,-4,0,0.5,8,8,Visual,200));
          Fallers.push_back(new Faller(x*16+8,y*16+8,1.1,-4,0,0.5,8,8,Visual,200));
          Visual=1023; // FIXME
          break;
        case A_COIN:
          Visual=1023; // FIXME
          Sound::Play(Sound::Coin,2);
          Behavior=BEHAVE_PASS;
          break;
        case A_GET_ITEM:
          SpecialVisual=VIS_ITEM_BOX;
          SpecialCounter=ITEM_VIS_TIMEOUT;
          Behavior=BEHAVE_FROWN_BLOCK;
          Sound::Play(Sound::Coin,2);
          Fallers.push_back(new Faller(x*16,y*16,0,-6,0,0.4,16,16,496,20));
          break;
        case A_BOUNCE_LEFT:
          SpecialCounter=BOUNCE_VIS_TIMEOUT;
          SpecialVisual=VIS_BOUNCE_LEFT;
          Sound::Play(Sound::Bounce,3);
          break;
        case A_BOUNCE_RIGHT:
          SpecialCounter=BOUNCE_VIS_TIMEOUT;
          SpecialVisual=VIS_BOUNCE_RIGHT;
          Sound::Play(Sound::Bounce,3);
          break;
        case A_BOUNCE_ABOVE:
          SpecialCounter=BOUNCE_VIS_TIMEOUT;
          SpecialVisual=VIS_BOUNCE_ABOVE;
          //Sound::Play(Sound::Bounce,3);
          break;
        case A_BOUNCE_BELOW:
          SpecialCounter=BOUNCE_VIS_TIMEOUT;
          SpecialVisual=VIS_BOUNCE_BELOW;
          Sound::Play(Sound::Bounce,3);
          break;
        case A_FLIP_BEGIN:
          Behavior=BEHAVE_FLIPPING;
          SpecialVisual=VIS_FLIP;
          SpecialCounter=FLIP_TIMEOUT;
          break;
        default:
          break;
      }
    }
    void TileObject :: Draw (int x1, int y1, int x2, int y2, bool rsb)
    {
        if (Surprise())
        {
          if (!rsb)
            return;
          glColor4d(0.5,0.5,0.5,0.5);
        }
        else
          glColor4d(1,1,1,1);
        int MyVisual=Visual;
        switch(SpecialVisual)
        {
            case VIS_BOUNCE_ABOVE:
                y1+=(4-abs(4-SpecialCounter))*2;
                //y2+=(4-abs(4-SpecialCounter))*2;
                break;
            case VIS_ITEM_BOX:
            case VIS_BOUNCE_BELOW:
                //y1-=4-abs(4-SpecialCounter);
                y2-=4-abs(4-SpecialCounter);
                break;
            case VIS_BOUNCE_LEFT:
                x1+=4-abs(4-SpecialCounter);
                //x2+=4-abs(4-SpecialCounter);
                break;
            case VIS_BOUNCE_RIGHT:
                //x1-=4-abs(4-SpecialCounter);
                x2-=4-abs(4-SpecialCounter);
                break;
            case VIS_FLIP:
                MyVisual=488|(framecount/6);
                break;
        }
        // TODO: Visual is more than a tile index
        DrawTile(x1, y1, x2, y2, MyVisual);
    }

    void TileObject :: SexSwap()
    {
        char *p = (char*)&Visual;
        char t;
        t = p[0];
        p[0] = p[1];
        p[1] = t;
    }

    istream& operator>>(istream& in, TileObject& o)
    {
        in.read((char*)&o.Visual, 3);
        return in;
    }

    ostream& operator<<(ostream& out, TileObject& o)
    {
        out.write((char*)&o.Visual, 3);
        return out;
    }
};

