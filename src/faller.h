#ifndef MARIO_FALLING_JUNK_HEADER
#define MARIO_FALLING_JUNK_HEADER
#include "game.h"
namespace Game
{
    class Faller
    {
        public:
        
            Faller(double X, double Y, double VX, double VY, 
                    double AX, double AY, int W, int H, int VISUAL, int LIFE)
                : x(X),y(Y),vx(VX),vy(VY),ax(AX),ay(AY),w(W),h(H),Visual(VISUAL),Life(LIFE),
                    frame(0)
                    {};
            double x, y, vx, vy, ax, ay;
            int w, h, Visual, Life, frame;
            bool Step() { vx+=ax; vy+=ay; x+=vx; y+=vy; frame=(frame+1)%24; return --Life>0; }; 
                // returns false if should be deleted.
            void Draw(int xo, int yo) 
            { DrawTile(int(x+xo),int(y+yo),int(x+w+xo),int(y+h+yo),
                    (Visual>=480&&Visual<500)?(Visual&~3)|(frame/6):Visual); };
    };
}
#endif
