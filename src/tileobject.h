#ifndef MARIO_BASE_OBJECT_HEADER
#define MARIO_BASE_OBJECT_HEADER

#include <iostream>
#include "game.h"
namespace Game
{
    enum Behaviour
    {
        BEHAVE_PASS,
        BEHAVE_SOLID,
        BEHAVE_FROWN_BLOCK,
        BEHAVE_PLATFORM,
        BEHAVE_HURT,
        BEHAVE_KILL,
        BEHAVE_COIN,         // Fixed-position coin
        BEHAVE_YOSHI_COIN_T, // Yoshi coin; top and..
        BEHAVE_YOSHI_COIN_B, // bottom half of
        BEHAVE_VBOUNCE,      // Like note block
        BEHAVE_HBOUNCE,      // Like feather in wood block SMB3:1-3
        BEHAVE_BOUNCE,       // Both (like in SMWar)
        BEHAVE_ITEM_BOX,     // Like ? and ! boxes
        BEHAVE_DONUT,
        BEHAVE_DOOR,         // Press up to enter
        BEHAVE_PIPE,
        BEHAVE_ICE_CAGE,     // Fireball to liberate contents
        BEHAVE_VINE,
        BEHAVE_FENCE,
        BEHAVE_FENCE_DOOR,   // TODO: Design/implement
        BEHAVE_BRICK,
        BEHAVE_FLIP,
        BEHAVE_FLIPPING,
        BEHAVE_NUM
    };
    enum Visual
    {
      VIS_NONE,
      VIS_BOUNCE_ABOVE,
      VIS_BOUNCE_BELOW,
      VIS_BOUNCE_LEFT,
      VIS_BOUNCE_RIGHT,
      VIS_FLIP,
      VIS_ITEM_BOX,
      VIS_NUM
    };
    enum Interaction
    {
        I_STOP_ABOVE=0,
        I_STOP_BELOW,
        I_STOP_LEFT,
        I_STOP_RIGHT,
        I_BOUNCE_ABOVE,
        I_BOUNCE_BELOW,
        I_BOUNCE_LEFT,
        I_BOUNCE_RIGHT,
        I_EJECT,
        I_COIN,
        I_SPINBREAK_ABOVE, // break if spinning & big
        I_BIGBREAK_BELOW,  // break if big
        I_NONE,
        I_HURT,
        I_KILL,
        I_ITEM_BELOW,
        I_FLIP_BELOW,
        I_NUM
    };
    enum BlockAction
    {
      A_NONE, // perhaps unused?
      A_BREAK,
      A_BOUNCE_ABOVE,
      A_BOUNCE_BELOW,
      A_BOUNCE_LEFT,
      A_BOUNCE_RIGHT,
      A_FLIP_BEGIN,
      A_GET_ITEM,
      A_COIN,
      A_NUM
    };
    class TileObject
    {
        public:
            TileObject() {SpecialVisual=0;SpecialCounter=0;framecount=0;};
            // Pass the rectangle to render (slight optimization)
            void Draw (int x1, int y1, int x2, int y2, bool rsb);
            void Step ();
            
            char framecount;
            
            // let's try this for now.
            unsigned char SpecialVisual;
            unsigned char SpecialCounter;
            
            // IMPORTANTNOTE: if anyone adjusts the position of these two
            // variables, then the save and load functiosn will break

            // This byte indicates the static tile or animation sequence to use
            unsigned short Visual;

            // The high-bit of behavior is the SURPRISE bit
            unsigned char Behavior;

            // Fetch the surprise bit from the Behavior byte
            bool Surprise() { return (1<<7)& Behavior? true:false; }

            // Fetch the behavior value, excluding the SURPRISE bit
            Behaviour GetBehavior() { return Behaviour(Behavior & ~(1<<7)); }

            void Act(BlockAction action, int x, int y);

            // Turn on the SURPRISE bit
            void SetSurprise(bool on) {
                if (on) Behavior |= 1<<7;
                else Behavior &= ~(1<<7); }

            friend std::ostream& operator<<(std::ostream& out, TileObject& o);
            friend std::istream& operator>>(std::istream&  in, TileObject& o);
            void SexSwap();
    };

    std::ostream& operator<<(std::ostream& out, TileObject& o);
    std::istream& operator>>(std::istream&  in, TileObject& o);
}
#endif

