
#ifndef MODEL_H
#define MODEL_H

#include "storage.h"

namespace RDF
{

class Statement;
class Node;

class Model
{
public:
  Model(const Model &rhs);
  Model( Storage *storage, const QString &options );
  ~Model();
  bool containsStatement( Statement *s ) const;
  void addStringLiteralStatement( Node *subject, Node *predicate, const QString &literal );
  int size() const;
  librdf_model* modelPtr() const;
private:
  class Private;
  Private *d;
};


}

#endif

