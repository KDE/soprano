
#include <redland.h>
#include "model.h"
#include "world.h"
#include "statement.h"

using namespace RDF;

struct Model::Private
{
  Private() : model(0L)
  {}
  librdf_model* model;
};

Model::Model(World *world, Storage *storage, const QString &options )
{
  d = new Private;
  d->model = librdf_new_model(world->worldPtr(), storage->storagePtr(), options.toLatin1().data());
  Q_ASSERT(d->model != NULL);
   
}

Model::~Model()
{
  librdf_free_model(d->model);
}

bool Model::containsStatement( Statement *s ) const
{
  return ( librdf_model_contains_statement(d->model, s->statementPtr()) != 0 );
}

void Model::addStringLiteralStatement( Node *subject, Node *predicate, const QString &literal )
{
  librdf_model_add_string_literal_statement ( d->model, subject->nodePtr(), predicate->nodePtr(), (const unsigned char*) literal.toLatin1().constData(), NULL, 0);
}

int Model::size() const
{
  return librdf_model_size(d->model);
}
