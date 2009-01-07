#include "oglconsole.h"
#include "tileobject.h"
#include "plumber.h"
#include "game.h"
#include "tileplane.h"
#include "glerror.h"
#include "faller.h"
#include <SDL.h>
#include <math.h>
#include <list>
#include <string>
#include <fstream>
#include "interact.h"
#ifdef __MACH__
#  include <OpenGL/gl.h>
#else
#  include <GL/gl.h>
#endif
using namespace std;

#define PBACK ((Plumber *)(Movers.back()))

// TODO: Remove/improve that old "texture manager" thing
#include "texture.h"
TextureManager *texmex;

namespace Game
{
    int xscroll,yscroll;
    bool rsb;

    bool paused = false;

    // Diagnostic crap
    bool diagCollision=false;

    // Editing scroll
    bool SuperScroll = false;
    int SuperScrollX=0, SuperScrollY=0;

    list <TilePlane*> Planes;
    typedef list <TilePlane*> :: iterator IPlane;

    list <Mover*> Movers;
    typedef list <Mover*> :: iterator IMover;

    /*list <Plumber*> Plumbers;
    typedef list <Plumber*> :: iterator IPlumber;*/

    list <Faller*> Fallers;
    typedef list <Faller*> :: iterator IFaller;

    void Init()
    {
        const char *tileset = "data/tileset.png";
        texmex = new TextureManager(1, &tileset);
        rsb=false;
    }

    void DefaultGame()
    {
        //Planes.push_back(new TilePlane(256, 32));
	LoadMap("one.map");
        Movers.push_back((Mover *)(new Plumber()));
    }

    bool Jaxis(int axis, int value)
    {
        switch(axis%2)
        {
            case 0:
                // x axis
                if (value<-100)
                {
                    PBACK->Key(LEFT,true);
                    PBACK->Key(RIGHT,false);
                }
                else if (value>100)
                {
                    PBACK->Key(RIGHT,true);
                    PBACK->Key(LEFT,false);
                }
                else
                {
                    PBACK->Key(LEFT,false);
                    PBACK->Key(RIGHT,false);
                }
                break;
            case 1:
                // y axis
                if (value<-100)
                {
                    PBACK->Key(CROUCH,false);
                }
                else if (value>100)
                {
                    PBACK->Key(CROUCH,true);
                }
                else
                {
                    PBACK->Key(CROUCH,false);
                }
                break;
        }
        return false;
    }
    
    bool Jbutton(int button, bool down)
    {
        switch(button)
        {
            case 0:
                PBACK->Key(JUMP,down);
                break;
            case 1:
                PBACK->Key(SPINJUMP,down);
                break;
            case 2:
                PBACK->Key(RUN,down);
                break;
            case 3:
                if (down)
                {
                    PBACK->small=!PBACK->small;
                    PBACK->h=PBACK->ducking||PBACK->small?12:20;
                }
                break;
          case 4:
            if (down)
              rsb=!rsb;
            break;
          default:
            return true;
        }
        return false;
    }
    
    bool Key(int key, bool down)
    {
        switch(key)
        {
            case SDLK_p:
                if (down) paused = ! paused;
                break;
          case SDLK_c:
            if (down)
              rsb=!rsb;
            break;
            case SDLK_t:
                if (down){
                    PBACK->small=!PBACK->small;
                    PBACK->h=PBACK->ducking||PBACK->small?12:20;}
                break;
            case SDLK_e:
                PBACK->Key(SPINJUMP,down);
                break;
            case SDLK_s:
                PBACK->Key(CROUCH,down);
                break;
            case SDLK_a:
                PBACK->Key(LEFT,down);
                break;
            case SDLK_d:
                PBACK->Key(RIGHT,down);
                break;
            case SDLK_w:
                PBACK->Key(JUMP,down);
                break;
            case SDLK_f:
                PBACK->Key(RUN,down);
                break;

            case ']':
                if (down) {
                SuperScroll = ! SuperScroll;
                OGLCONSOLE_Print("The ']' key turned SuperScroll %s\n",
                        SuperScroll?"on":"off");
                }
                break;

            case SDLK_UP:
                if(down)SuperScrollY=-10;
                else SuperScrollY=0;
                break;
            case SDLK_DOWN:
                if(down)SuperScrollY=10;
                else SuperScrollY=0;
                break;
            case SDLK_LEFT:
                if(down)SuperScrollX=-10;
                else SuperScrollX=0;
                break;
            case SDLK_RIGHT:
                if(down)SuperScrollX=10;
                else SuperScrollX=0;
                break;

            default:
                return true;
        }
        return false;
    }
    
    void Step()
    {
        if (paused) return;

        //for (IPlumber i = Plumbers.begin(); i != Plumbers.end(); i++) (*i)->Step();
        double nx,ny;
        for (IMover i = Movers.begin(); i != Movers.end(); i++)
        {
          (*i)->Step(&nx,&ny);
          moveAndInteract(*i,nx,ny,&Planes);
        }
        // Mover-on-mover interaction
        diagCollision = false; // diagnostic crap
        for (IMover i = Movers.begin(); i != Movers.end(); i++)
        {
            IMover j = i;
            while (++j != Movers.end())
                MoverFight(*i, *j);
        }
        for (IPlane i = Planes.begin(); i != Planes.end(); i++) (*i)->Step();
        for (IFaller i = Fallers.begin(); i != Fallers.end(); i++) 
        {
            if (!((*i)->Step()))
            {
                IFaller j=i;
                i++;
                Fallers.remove(*j);
                i--;
            }
        }

        //scroll -- TODO - bind this properly with the bound plane...thing
        if (!SuperScroll)
        {
            if (PBACK->x-xscroll>320-80) xscroll=int(PBACK->x)-(320-80);
            if (PBACK->x-xscroll<80) xscroll=int(PBACK->x)-80;
            if (xscroll<0) xscroll=0;
            if (xscroll>(Planes.back()->w*16-320)) xscroll=Planes.back()->w*16-320; 
            if (PBACK->y-yscroll>240-80) yscroll=int(PBACK->y)-(240-80);
            if (PBACK->y-yscroll<80) yscroll=int(PBACK->y)-80;
            if (yscroll<0) yscroll=0;
            if (yscroll>(Planes.back()->h*16-240)) yscroll=Planes.back()->h*16-240; 
        }
        else
        {
            xscroll+=SuperScrollX;
            yscroll+=SuperScrollY;
        }
       
    }

    void DrawBegin();
    void DrawEnd();

    void Draw()
    {
        DrawBegin();

        // Draw the background TODO: Fix glitches over the edge of the level?
        for (int x=0;x<=(320/16);x++)
        {
            for (int y=0;y<=(240/16);y++)
            {
                DrawTile(x*16-(xscroll/4)%16,y*16-(yscroll/4)%16,
                        (x+1)*16-(xscroll/4)%16,(y+1)*16-(yscroll/4)%16,17);
            }
        }

        // Draw each tile plane
        for (IPlane i = Planes.begin(); i != Planes.end(); i++) (*i)->Draw(-xscroll,-yscroll,rsb);

        // Draw each plumber
        for (IMover i = Movers.begin(); i != Movers.end(); i++) (*i)->Draw(-xscroll,-yscroll);

        // Draw each fallers
        for (IFaller i = Fallers.begin(); i != Fallers.end(); i++) (*i)->Draw(-xscroll,-yscroll);
        
        // Diagnostic information HUD
        if (PBACK->jumping)
          DrawTile(0,240-16,16,240,484);

        if (PBACK->jumpthrust>0)
          DrawTile(16,240-16,32,240,4*32+19);

        if (diagCollision)
          DrawTile(32,240-16,48,240,64*32+9);

        // Relinquish the GL
        DrawEnd();

        static unsigned int errstore=0;
        glError("Game::Draw()", &errstore);
    }

    // EDITING //
    unsigned short Clip_Visual = 0;         // Clipboard
    unsigned char  Clip_Behavior = 0;
    bool Pasting = false;
    void Paste()// TODO: be able to interact with other planes
    {
        TileObject O;
        O.Visual = Clip_Visual;
        O.Behavior = Clip_Behavior;
        Planes.back()->Paste(O);
    }
    void MouseButton(int button, bool down)
    {
        if (down)
        {
            if (!Planes.size()) return;
            if (button == 3) // Copy to clipboard
            {
                // TODO: overload tileplane subscript operator
                TilePlane *P = Planes.back();
                TileObject O = P->grid[P->editx + P->edity * P->w];
                Clip_Visual = O.Visual;
                Clip_Behavior = O.Behavior;
            }
            else if (button == 1) // Paste tile
            {
                Paste();
                Pasting = true;
            }
        }
        else Pasting = false;
    }
    void Mouse(int x, int y)                // Edit cursor
    {
        // TODO: be able to interact with other planes
        if(Planes.size())
            (*Planes.back()).Mouse(x/2+xscroll,y/2+yscroll);

        if (Pasting)
            Paste();
    }

    // Prepare the GL for drawing
    void DrawBegin()
    {
        glPushAttrib(GL_ALL_ATTRIB_BITS);

        // Orthographix
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        //glOrtho(0, 640, 0, 480, 1, -1);
        glOrtho(0, 320, 240, 0, 1, -1);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_2D);
        texmex->Bind(0);
//      glBindTexture(GL_TEXTURE_2D, spriteHandle);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glBegin(GL_QUADS);
    }

    // Relinquish the GL
    void DrawEnd()
    {
        glEnd();

        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();

        glPopAttrib();
    }

    // Draw tile with spin
    void DrawSpinTile (int cx, int cy, int size, double spin, int tile)
    {
        double tx1 = double(tile % (512/16) * 16)      / 512.0;
        double tx2 = double(tile % (512/16) * 16 + 16) / 512.0;
        double ty1 = double(tile / (512/16) * 16)      / 512.0;
        double ty2 = double(tile / (512/16) * 16 + 16) / 512.0;

        double dx, dy;
        dx = sin(spin) * size;
        dy = cos(spin) * size;

        glTexCoord2d(tx1, ty1);
        glVertex2d  ( cx+dx,  cy+dy);

        glTexCoord2d(tx2, ty1);
        glVertex2d  ( cx-dy,  cy+dx);

        glTexCoord2d(tx2, ty2);
        glVertex2d  ( cx-dx,  cy-dy);

        glTexCoord2d(tx1, ty2);
        glVertex2d  ( cx+dy,  cy-dx);
    }

    // Pass left and right pixel coordinates, and which tile to draw
    void DrawTile (int x1, int y1, int x2, int y2, int tile)
    {
        double tx1 = double(tile % (512/16) * 16)      / 512.0;
        double tx2 = double(tile % (512/16) * 16 + 16) / 512.0;
        double ty1 = double(tile / (512/16) * 16)      / 512.0;
        double ty2 = double(tile / (512/16) * 16 + 16) / 512.0;

        glTexCoord2d(tx1, ty1);
        glVertex2i  ( x1,  y1);

        glTexCoord2d(tx2, ty1);
        glVertex2i  ( x2,  y1);

        glTexCoord2d(tx2, ty2);
        glVertex2i  ( x2,  y2);

        glTexCoord2d(tx1, ty2);
        glVertex2i  ( x1,  y2);
    }

    void Quit()
    {
        delete texmex;

        for (IMover i = Movers.begin(); i != Movers.end(); i++) delete *i;
        Movers.clear();

        for (IPlane i = Planes.begin(); i != Planes.end(); i++) delete *i;
        Planes.clear();
    }

    const char fileMagic[] = "mario.map file magic";
    const int magicLen = 20;
    int byteSex[2] = {0xDEADBEEF, 0};
    int byteXes[2] = {0xEFBEADDE, 0};

    bool SaveMap(const string& filename) {
        // TODO: assert that int is 32 bit
        int numPlanes = Planes.size();
        fstream f(filename.c_str(),ios::out|ios::binary);
        f.write(fileMagic, magicLen);                           // Write file magic
        f.write((char*)byteSex, sizeof(int)*2);                 // Write byte sex test
        f.write((char*)&numPlanes, sizeof(int));                // Write number of tile planes
        for (IPlane i=Planes.begin(); i!=Planes.end(); i++)
            f << **i;                                           // Print out each plane
        f.close();                                              // Is this necessary?
        return true;//TODO:check for failure, lol
    }

#define fagswap(p,n) {char*c=(char*)&p;char t=c[0];c[0]=c[3];c[3]=t;t=c[1];c[1]=c[2];c[2]=t;}
    bool LoadMap(const string& filename) {
        // TODO: assert that int is 32 bit
        int numPlanes;
        bool sexSwap = false;
        fstream f(filename.c_str(),ios::in|ios::binary);
        {
            char magicTest[sizeof(fileMagic)];
            int sexTest[2];
            f.read(magicTest,magicLen);
            f.read((char*)sexTest,sizeof(int)*2);
            if (strncmp(magicTest,fileMagic,magicLen)) {        // Check file magic
                OGLCONSOLE_Print("Unrecognized file format in map file \"%s\"\n", filename.c_str());
                return false;
            }
            if (sexTest[0] != byteSex[0] || sexTest[1] != byteSex[1]) {
                if (sexTest[0] == byteXes[0] && sexTest[1] == byteXes[1]) {
                    OGLCONSOLE_Print("This map file is written in the opposite bytesex!\n");
                    sexSwap = true;
                }
                else 
                {
                    OGLCONSOLE_Print("0x%x%x\n0x%x%x\nBroken bytesex magic! May not be a map file?\n",
                            byteSex[0], byteSex[1], sexTest[0], sexTest[1]);
                    return false;
                }
            }
        }
        f.read((char*)&numPlanes, sizeof(int));

        if (sexSwap)
            fagswap(numPlanes,1);

        for (IPlane i=Planes.begin(); i!=Planes.end(); i++)
            delete *i;
        Planes.clear();
        for (; numPlanes; numPlanes--)
            Planes.push_back(new TilePlane(f, sexSwap));
        f.close();
        return true;//TODO:check for failure, lol
    }
};

