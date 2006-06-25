
#include <QtGlobal>
#include "query.h"

using namespace RDF;

struct Query::Private
{
  Private() : query(0L)
  {}
  librdf_query *query;
};

Query::Query()
{
  d = new Private;
  d->query = librdf_new_query();
  Q_ASSERT(d->query != NULL);
  librdf_query_open(d->query);
}

Query::~Query()
{
  librdf_free_query(d->query);
}





librdf_query* Query::queryPtr()
{
  return d->query;
}