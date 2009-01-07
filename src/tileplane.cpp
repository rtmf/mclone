#include "tileplane.h"
#include "tileobject.h"
#ifdef __MACH__
#  include <OpenGL/gl.h>
#else
#  include <GL/gl.h>
#endif
// TODO: Temporary bs
#define ScreenWidth  320
#define ScreenHeight 240
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <math.h>
using namespace std;

namespace Game
{
    // For drawing the edit cursor... when we have multiple planes this'll have to be changed a bit
    extern unsigned short Clip_Visual;

    void TilePlane :: Draw (int ofx, int ofy, bool rsb)
    {
        static int framecount=0;
        // Accumulate our rendering offset and world offset
        ofx += ox;
        ofy += oy;
        
        // Determine which tiles will be rendered
        // TODO: big = small + dimension / 16 + (small%16?1:0) ????
        int left = -ofx / 16;
        int right = left + ScreenWidth / 16;
        int top = -ofy / 16;
        int bottom = top + ScreenHeight / 16;
        int xof=ofx%16;
        int yof=ofy%16;

/*      glColor3ub(random()%256, random()%256, random()%256);
        glVertex2d(0, 0);
        glVertex2d(640, 0);
        glVertex2d(640, 480);
        glVertex2d(0, 480);*/
        framecount++;
        /*if (framecount>7)
        framecount=0;*/

        for (int x=left; x<=right; x++)
        {
            int x1 = (x+ofx/16) * 16+xof;
            int x2 = (x+ofx/16+1) * 16+xof;

            for (int y=top; y<=bottom; y++)
            {
                int y1 = (y+ofy/16) * 16+yof;
                int y2 = (y+ofy/16+1) * 16+yof;

                if (x>=0 && x<w && y>=0 && y<h)
                    grid[x + y * w].Draw(x1,y1, x2,y2,rsb);
            }
        }

        // Draw edit cursor
        if(editx>=0)
        {
            //if (framecount%2)
            //{
          /*glColor4d((framecount&(1<<2))>>2,
                     (framecount&(1<<3))>>3,
          (framecount&(1<<4))>>4,0.7);*/
                /*DrawTile(editx*16 + ofx,
                         edity*16 + ofy,
                         editx*16 + ofx + 16,
          edity*16 + ofy + 16, Clip_Visual);*/
          glColor4d(1,1,1,0.7+sin(framecount*0.15)*0.2);
          DrawSpinTile(editx*16+ofx+8,edity*16+ofy+8,12,3.14/4+3.14+sin(framecount*0.15)*wobble,Clip_Visual);
          wobble *= 0.96;
                glColor4d(1,1,1,1);
            //}
        }
    }

    void TilePlane::Paste(TileObject o)
    {
        if (editx<0 || editx>=w || edity<0 || edity>=h) return;
        grid[editx + edity * w] = o;
    }
    void TilePlane::Mouse(int x, int y) // Mouse for editing
    {
        int oex=editx,oey=edity;
        editx = (x+ox)/16;
        edity = (y+oy)/16;
        if (editx!=oex || edity!=oey)
            wobble=1;
    }
    
    void TilePlane::Step()
    {
        for (int x=0;x<w;x++)
            for (int y=0;y<h;y++)
                grid[x+y*w].Step();
    }

    TilePlane :: TilePlane (int W, int H, int offx, int offy)
    {
        w = W;
        h = H;
        ox = offx;
        oy = offy;
        
        // Initialize edit cursor position (negative means do not display)
        editx = -1;
        edity = -1;

        grid = new TileObject[W*H];
        for (int x=0; x<w; x++) for(int y=0; y<h; y++)
        {
            grid[x+y*w].Visual = y==0?142:y>28?(x>0?x<(w-1)?33:34:32):98;
            grid[x+y*w].Behavior = y==0?BEHAVE_SOLID:BEHAVE_PASS;
        }
        for (int x=0;x<w;x++)
        {
            //grid[x+6*w].Visual = random()%1024;
            grid[x+28*w].Visual = x>0?x<(w-1)?1:2:0;
            grid[x+28*w].Behavior = BEHAVE_PLATFORM;
        }
        grid[3+8*w].Visual=4;
        grid[4+8*w].Visual=5;
        grid[5+8*w].Visual=6;
        grid[3+8*w].Behavior=BEHAVE_PLATFORM;
        grid[4+8*w].Behavior=BEHAVE_PLATFORM;
        grid[5+8*w].Behavior=BEHAVE_PLATFORM;
        grid[7+8*w].Visual=492;
        grid[7+8*w].Behavior=BEHAVE_BRICK;
        grid[9+8*w].Visual=484;
        grid[9+8*w].Behavior=BEHAVE_VBOUNCE;
        grid[8+9*w].Visual=96;
        grid[8+9*w].Behavior=BEHAVE_HBOUNCE;
        for (int x=14;x<w;x++)
        {
            for (int y=1;y<28;y++)
            {
                int V, B;
                switch(random()%64)
                {
                    case 0:
                    case 1:
                    case 2:
                        B=BEHAVE_PLATFORM;
                        V=14*32+19;
                        break;
                    case 3:
                    case 4:
                    case 5:
                        B=BEHAVE_BRICK;
                        V=492;
                        break;
                    case 6:
                    case 7:
                        B=BEHAVE_VBOUNCE | (1<<7);
                        V=484;
                        break;
                    case 8:
                    case 9:
                        B=BEHAVE_HBOUNCE;
                        V=96;
                        break;
                    case 10:
                        B=BEHAVE_BOUNCE | (1<<7);
                        V=4*32+19;
                        break;
                    case 11:
                    case 12:
                    case 13:
                    case 14:
                        B=BEHAVE_COIN;
                        V=496;
                        break;
                    case 15:
                    case 16:
                    case 17:
                    case 18:
                    case 19:
                        B=BEHAVE_SOLID;
                        V=5*32+1;
                        break;
                    case 20:
                    case 21:
                    case 22:
                        B=BEHAVE_FLIP;
                        V=7*32+9;
                        break;
                    case 23:
                    case 24:
                    case 25:
                        B=BEHAVE_ITEM_BOX | (1<<7);
                        V=480;
                        break;
                    default:
                        B=BEHAVE_PASS;
                        V=1023;
                        break;
                }
                grid[x+y*w].Visual=V;
                grid[x+y*w].Behavior=B;//==BEHAVE_PASS?B:BEHAVE_BRICK;
            }
        }
    }

    TilePlane :: ~TilePlane()
    {
        delete[] grid;
    }

#define fagswap(p,n) {char*c=(char*)&p;char t=c[0];c[0]=c[3];c[3]=t;t=c[1];c[1]=c[2];c[2]=t;}
    TilePlane :: TilePlane(istream& in, bool sexSwap)
    {
        // read plane dimensions, and offset
        in.read((char*)&w, sizeof(int));
        in.read((char*)&h, sizeof(int));
        in.read((char*)&ox, sizeof(int));
        in.read((char*)&oy, sizeof(int));
        if (sexSwap)
        {
            fagswap(w,1);
            fagswap(h,1);
            fagswap(ox,1);
            fagswap(oy,1);
        }
        int len = w * h;
        grid = new TileObject[len];
        for (int i=0; i<len; i++)
            in >> grid[i];

        if (sexSwap)
            for (int i=0; i<len; i++)
                grid[i].SexSwap();
    }

    // write
    ostream& operator<<(ostream& out, TilePlane& plane)
    {
        // write plane dimensions, and offset
        out.write((char*)&plane.w, sizeof(int));
        out.write((char*)&plane.h, sizeof(int));
        out.write((char*)&plane.ox, sizeof(int));
        out.write((char*)&plane.oy, sizeof(int));
        int len=plane.w*plane.h;
        for (int i=0; i<len; i++)
            out << plane.grid[i];
        return out;
    }
};

