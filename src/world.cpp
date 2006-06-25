
#include <QtGlobal>
#include "world.h"

using namespace RDF;

struct World::Private
{
  Private() : world(0L)
  {}
  librdf_world *world;
};

World::World()
{
  d = new Private;
  d->world = librdf_new_world();
  Q_ASSERT(d->world != NULL);
  librdf_world_open(d->world);
}

World::~World()
{
  librdf_free_world(d->world);
}





librdf_world* World::worldPtr()
{
  return d->world;
}