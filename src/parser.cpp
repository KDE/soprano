
#include <QtGlobal>
#include "parser.h"

using namespace RDF;

struct Parser::Private
{
  Private() : parser(0L)
  {}
  librdf_parser *parser;
};

Parser::Parser(World *world, const QString &name)
{
  d = new Private;
  d->parser = librdf_new_parser(world->worldPtr(), name.toLatin1().data(), NULL, NULL);
  Q_ASSERT(d->parser != NULL);
}

Parser::~Parser()
{
  librdf_free_parser(d->parser);
}

librdf_parser* Parser::parserPtr()
{
  return d->parser;
}

