#include "oglconsole.h"
#include "mover.h"
#include <math.h>
namespace Game
{
  Mover::Mover()
  {
  }
  Mover::~Mover()
  {
  }
    // Call this routine to test for a 1 dimnsional collision where the
    // width/height along this dimension is the same for both members being
    // tested for collision; returns true on collision
    inline bool D1Collider_SameSize(const int a, const int b, const int w) {
        return abs(a-b)<w;
    }

    void MoverFight(Mover *m1, Mover *m2)
    {
        // Check for any collision at all TODO: support objects w/differing
        // dimensions
        if(!D1Collider_SameSize(int(m1->x), int(m2->x), m1->w)
        || !D1Collider_SameSize(int(m1->y), int(m2->y), m1->h))
            return;

        // Diagnostic crap
        diagCollision = true;
    }
}
