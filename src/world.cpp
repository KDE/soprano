
#include <QtGlobal>
#include "world.h"
#include <iostream>

using namespace RDF;
using namespace std;

librdf_world *World::_world = 0L;
int World::_refcount = 0;

World::World()
{
  cout << "ref:" << _refcount << std::endl;
  if ( _refcount == 0 )
  {
    cout << "Creating world.." << std::endl;
    _world = librdf_new_world();
    librdf_world_open(_world);
  }
  _refcount++;
  Q_ASSERT(_world != NULL);
}

World::~World()
{
  cout << "~ ref:" << _refcount << std::endl;
  _refcount--;
  if ( _refcount == 0 )
  {
    cout << "Destroying world.." << std::endl;
    librdf_free_world(_world);
  }
}

librdf_world* World::worldPtr()
{
  return _world;
}

