
#ifndef WORLD_H
#define WORLD_H

#include <redland.h>

namespace RDF
{

class World
{
public:
  World();
  ~World();
  librdf_world* worldPtr();
private:
  static librdf_world *_world;
  static int _refcount;
};


}

#endif

