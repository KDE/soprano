
#ifndef STATEMENT_H
#define STATEMENT_H

#include <QString>
#include <QUrl>

#include <redland.h>
#include "world.h"
#include "node.h"

namespace RDF
{

class Statement
{
public:
  Statement( const Statement &rhs );
  Statement(Node *subject, Node *predicate, Node *object);
  ~Statement();

  bool equals( Statement *s );

  void setSubject( Node *node );
  void setPredicate( Node *node );
  void setObject( Node *node );

  Node* subject() const;
  Node* predicate() const;
  Node* object() const;

  void clear();

  librdf_statement* statementPtr() const;
  QString toString() const;
private:
  class Private;
  Private *d;
};


}

#endif

