
#include <QtGlobal>
#include "parser.h"

using namespace RDF;

struct Parser::Private
{
  Private() : parser(0L)
  {}
  librdf_parser *parser;
  World world;
};

Parser::Parser(const QString &name)
{
  d = new Private;
  d->parser = librdf_new_parser(d->world.worldPtr(), name.toLatin1().data(), NULL, NULL);
  Q_ASSERT(d->parser != NULL);
}

Parser::~Parser()
{
  librdf_free_parser(d->parser);
  delete d;
}

librdf_parser* Parser::parserPtr()
{
  return d->parser;
}

