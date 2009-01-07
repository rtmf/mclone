#ifndef MARIO_GAME_HEADER
#define MARIO_GAME_HEADER
#include <list>
#include <string>
namespace Game
{
    using namespace std;

    void Init();
    void Step();
    void Draw();
    void Quit();
    void DefaultGame();
    void Mouse(int x, int y); // Mouse only gets into Game for editing
    void MouseButton(int button, bool down);
    bool Key(int key, bool down);
    bool Jaxis(int axis, int value);
    bool Jbutton(int button, bool down);

    bool LoadMap(const string& filename);
    bool SaveMap(const string& filename);

    class Faller;
    extern std::list<Faller *> Fallers;
    
    // TODO: Move
    // Pass left and right pixel coordinates, and which tile to draw
    void DrawTile (int x1, int y1, int x2, int y2, int tile);
    void DrawSpinTile (int cx, int cy, int size, double spin, int tile);
    enum Keynum
    {
        LEFT,
        RIGHT,
        JUMP,
        CROUCH,
        RUN,
        SPINJUMP,
        KEY_NUM
    };
    enum Direction
    {
        D_ABOVE,
        D_BELOW,
        D_LEFT,
        D_RIGHT,
        D_NONE,
        D_NUM
    };

    // Diagnostic crap
    extern bool diagCollision;
};
#endif

