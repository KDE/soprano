
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include <redland.h>

void main(int argc, char *argv[]) 
{
  librdf_world* world;
  world=librdf_new_world();
  librdf_world_open(world);
  librdf_free_world(world);
}