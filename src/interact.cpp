#include "interact.h"
#include "tileobject.h"
#include "mover.h"
#include "game.h"
#include <list>
using namespace std;
namespace Game
{
  typedef list <TilePlane*> :: iterator IPlane;
  typedef list <InteractionResult*> :: iterator IResult;
  list<InteractionResult *> results;
  int getPriorityOf(Interaction i)
  {
    switch(i)
    {
      case I_STOP_ABOVE:
      case I_STOP_BELOW:
      case I_STOP_LEFT:
      case I_STOP_RIGHT:
        return 20;
      case I_HURT:
      case I_KILL:
      case I_SPINBREAK_ABOVE:
      case I_BIGBREAK_BELOW:
      case I_BOUNCE_ABOVE:
      case I_BOUNCE_LEFT:
      case I_BOUNCE_RIGHT:
      case I_BOUNCE_BELOW:
      case I_FLIP_BELOW:
      case I_ITEM_BELOW:
        return 10;
      case I_EJECT:
      case I_COIN:
      case I_NONE:
        return 0;
      default:
        return 0;
    }
    return 0;
  }
  InteractionResult getMostImportant(void)
  {
    //find most important interaction in interaction list
    InteractionResult res = **results.begin();
    for (IResult i=results.begin();i!=results.end();i++)
    {
      //yeah, wastes one iteration
      if (getPriorityOf((*i)->i)>getPriorityOf(res.i))
      {
        res=**i;
      }
      delete *i;
    }
    results.clear();
    return res;
  }
  void interactAt (Mover * P, std::list <Game::TilePlane *> * PPlanes)
  {
    Interaction ires;
    results.clear(); // yeah this might leak if you're dumb.
    
    //first add results from blocks we're already on.
    //these won't show up in crossings except for blocks we're in the process of leaving already.
    for (IPlane i=PPlanes->begin();i!=PPlanes->end();i++)
    {
      TileObject * obj;
      for (int xp=int((P->x)/16);xp<=int((P->x+P->w-1)/16);xp++)
      {
        for (int yp=int((P->y-P->h+1)/16);yp<=int((P->y)/16);yp++)
        {
          if (yp<(*i)->h && yp>=0 && xp <(*i)->w && xp>=0)
          {
                //surprise hack
            obj=&((*i)->grid[xp+yp*((*i)->w)]);
            if (!obj->Surprise())
            {
              ires=P->Interact(obj->GetBehavior(),D_NONE);
              if (ires!=I_NONE)
              {
                results.push_back(new InteractionResult(ires,xp,yp,obj));
              }
            }
          }
        }
      }
    }
  }
  void interactIntoX (Mover * P, double nx, std::list <Game::TilePlane *> * PPlanes)
  {
    Interaction ires;
    results.clear(); // yeah this might leak if you're dumb.
    
    double ny=P->y;
    if (int(nx)!=int(P->x))
    {
      if ((int(nx)+P->w-1)/16>(int(P->x)+P->w-1)/16)
      {
        //we've moved across a tile boundary.
        //only interact with tiles that are in the Y range of ny to ny-P_HEIGHT
        for (IPlane i=PPlanes->begin();i!=PPlanes->end();i++)
        {
          TileObject * obj;
          int xp=int((nx+P->w-1)/16);
          for (int yp=int((ny-P->h+1)/16);yp<=int((ny)/16);yp++)
          {
            if (yp<(*i)->h && yp>=0 && xp <(*i)->w && xp>=0)
            {
              //surprise hack
              obj=&((*i)->grid[xp+yp*((*i)->w)]);
              if (!obj->Surprise())
              {
                ires=P->Interact(obj->GetBehavior(),D_LEFT);
                if (ires!=I_NONE)
                {
                  results.push_back(new InteractionResult(ires,xp,yp,obj));
                }
              }
            }
          }
        }
      }
      if (int(nx)/16<int(P->x)/16)
      {
        //we've moved across a tile boundary.
        //only interact with tiles that are in the Y range of ny to ny-P_HEIGHT
        for (IPlane i=PPlanes->begin();i!=PPlanes->end();i++)
        {
          TileObject * obj;
          int xp=int((nx)/16);
          for (int yp=int((ny-P->h+1)/16);yp<=int((ny)/16);yp++)
          {
            if (yp<(*i)->h && yp>=0 && xp <(*i)->w && xp>=0)
            {
              //surprise hack
              obj=&((*i)->grid[xp+yp*((*i)->w)]);
              if (!obj->Surprise())
              {
                ires=P->Interact(obj->GetBehavior(),D_RIGHT);
                if (ires!=I_NONE)
                {
                  results.push_back(new InteractionResult(ires,xp,yp,obj));
                }
              }
            }
          }
        }
      }
    }
  }
  void interactIntoY (Mover * P, double ny, std::list <Game::TilePlane *> * PPlanes)
  {
    Interaction ires;
    results.clear(); // yeah this might leak if you're dumb.
    
    double nx=P->x;
    //interactintoy
    if (int(ny)!=int(P->y))
    {
      if ((int(ny))/16>(int(P->y))/16)
      {
        //we've moved across a tile boundary.
        for (IPlane i=PPlanes->begin();i!=PPlanes->end();i++)
        {
          TileObject * obj;
          int yp=int((ny)/16);
          for (int xp=int(nx/16);xp<=int((nx+P->w-1)/16);xp++)
          {
            if (yp<(*i)->h && yp>=0 && xp <(*i)->w && xp>=0)
            {
              //surprise hack
              obj=&((*i)->grid[xp+yp*((*i)->w)]);
              if (!obj->Surprise())
              {
                ires=P->Interact(obj->GetBehavior(),D_ABOVE);
                if (ires!=I_NONE)
                {
                  results.push_back(new InteractionResult(ires,xp,yp,obj));
                }
              }
            }
          }
        }
      }
      if ((int(ny)-P->h+1)/16<(int(P->y)-P->h+1)/16)
      {
        //we've moved across a tile boundary.
        //only interact with tiles that are in the Y range of ny to ny-P_HEIGHT
        for (IPlane i=PPlanes->begin();i!=PPlanes->end();i++)
        {
          TileObject * obj;
          int yp=int((ny-P->h+1)/16);
          for (int xp=int(nx/16);xp<=int((nx+P->w-1)/16);xp++)
          {
            if (yp<(*i)->h && yp>=0 && xp <(*i)->w && xp>=0)
            {
              //surprise hack
              obj=&((*i)->grid[xp+yp*((*i)->w)]);
              ires=P->Interact(obj->GetBehavior(),D_BELOW);
              if (ires!=I_NONE)
                results.push_back(new InteractionResult(ires,xp,yp,obj));
            }
          }
        }
      }
    }
  }
  void moveAndInteract(Mover * P, double nx, double ny, std::list <Game::TilePlane *> * PPlanes)
  {
    interactAt(P,PPlanes);
    for (IResult i=results.begin();i!=results.end();i++)
    {
      BlockAction res=P->Act((*i)->i,(*i)->x,(*i)->y,&nx,&ny);
      if (res!=A_NONE)
      {
        (*i)->o->Act(res,(*i)->x,(*i)->y);
      }
      delete *i;
    }
    results.clear();
    
    //any interactAt results have to ALL be handled.
    double bullshit;
    while(true)
    {
      interactIntoX (P, nx, PPlanes);
      if (results.empty()) break;
      InteractionResult r = getMostImportant();
      BlockAction res=P->Act(r.i,r.x,r.y,&nx,&bullshit);
      if (res!=A_NONE)
      {
        r.o->Act(res,r.x,r.y);
        if (r.o->Surprise())
          r.o->SetSurprise(false);
      }
    }
    //P->x=nx;
    P->StepTo(nx,P->y);
    while(true)
    {
      interactIntoY (P, ny, PPlanes);
      if (results.empty()) break;
      InteractionResult r = getMostImportant();
      BlockAction res=P->Act(r.i,r.x,r.y,&bullshit,&ny);
      if (res!=A_NONE)
      {
        r.o->Act(res,r.x,r.y);
        if (r.o->Surprise())
          r.o->SetSurprise(false);
      }
    }
  
    //P->x=nx;
    P->StepTo(P->x,ny);
    //if (int(ny)!=int(P->y)) P->jumping=true;
    //P->y=ny;
    
  }
}
