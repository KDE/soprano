
#include <QUrl>

#include <world.h>
#include <storage.h>
#include <node.h>

using namespace RDF;

int
main(int argc, char *argv[]) 
{
  Node *node = new Node( QUrl("http://purl.org/dc/elements/1.1/title"));
  delete node;
  //Storage *storage = new Storage( context, "memory", "test", NULL);
}

