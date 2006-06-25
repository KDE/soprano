
#include <QUrl>

#include <world.h>
#include <storage.h>
#include <node.h>

using namespace RDF;

int
main(int argc, char *argv[]) 
{
  World *world = new World();
  Node *node = new Node( world, QUrl("http://purl.org/dc/elements/1.1/title"));
  //Storage *storage = new Storage( context, "memory", "test", NULL);
}

