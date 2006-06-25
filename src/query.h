
#ifndef QUERY_H
#define QUERY_H

#include <redland.h>

namespace RDF
{

class Query
{
public:
  Query();
  ~Query();
  librdf_query* queryPtr();
private:
  class Private;
  Private *d;
};


}

#endif

