#ifndef MARIO_TILE_PLANE_HEADER
#define MARIO_TILE_PLANE_HEADER
#include <iostream>
namespace Game
{
    class TileObject;
    class TilePlane
    {
        public:
            // Pass the dimensions of this plane expressed as tiles, and an
            // offset to the world expressed in pixels
            TilePlane(int W, int H, int offx=0, int offy=0);
            TilePlane(std::istream& in, bool sexSwap);
            ~TilePlane();

            // Specify the pixel position of the upper-right hand corner of the
            // stage relative to this tile plane. TilePlane considers this
            // offset PLUS its world offset
            void Draw(int ofx, int ofy, bool rsb);

            void Step ();

            // IMPORTANTNOTE: if anyone adjusts the position of these four
            // variables, then the save and load functiosn will break
            int w, h;      // Plane's dimensions expressed in tiles
            int ox, oy;    // Every plane has a pixel offset
            TileObject *grid;

            // Edit cursor
            int editx, edity;
            double wobble;
            void Paste(TileObject o);
            void Mouse(int x, int y);

            // These routines exist for save/load functions
        friend std::ostream& operator<<(std::ostream& out, TilePlane& plane);
        friend std::istream& operator>>(std::istream&  in, TilePlane& plane);
    };

    // These routines exist for save/load functions
    std::ostream& operator<<(std::ostream& out, TilePlane& plane);
    std::istream& operator>>(std::istream&  in, TilePlane& plane);
}
#endif

