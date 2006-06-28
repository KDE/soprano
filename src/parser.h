
#ifndef PARSER_H
#define PARSER_H

#include <QString>
#include <redland.h>
#include "world.h"

namespace RDF
{

class Parser
{
public:
  Parser(const QString &name);
  ~Parser();
  librdf_parser* parserPtr();
private:
  class Private;
  Private *d;
};


}

#endif

