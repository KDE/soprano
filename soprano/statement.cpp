
#include <QtGlobal>
#include "statement.h"

using namespace RDF;

struct Statement::Private
{
  Private() : statement(0L)
  {}
  librdf_statement *statement;
};


Statement::Statement( const Statement &rhs )
{
  d = new Private;
  d->statement = librdf_new_statement_from_statement( rhs.statementPtr() );
  Q_ASSERT(d->statement != NULL);
}

Statement::Statement(World *world, Node *subject, Node *predicate, Node *object)
{
  d = new Private;
  d->statement = librdf_new_statement_from_nodes(world->worldPtr(), subject->nodePtr(), predicate->nodePtr(), object->nodePtr());
  Q_ASSERT(d->statement != NULL);
}

Statement::~Statement()
{
  librdf_free_statement(d->statement);
}

bool Statement::equals( Statement *s )
{
  return ( librdf_statement_equals(d->statement, s->statementPtr()));
}

librdf_statement* Statement::statementPtr() const
{
  return d->statement;
}

QString Statement::toString() const
{
  return QString( (const char *) librdf_statement_to_string (d->statement));
}

void Statement::setSubject( Node *node )
{
  librdf_statement_set_subject(d->statement, node->nodePtr());
}

void Statement::setPredicate( Node *node )
{
  librdf_statement_set_predicate(d->statement, node->nodePtr());
}

void Statement::setObject( Node *node )
{
  librdf_statement_set_object(d->statement, node->nodePtr());
}

Node* Statement::subject() const
{
  return 0L;
}

Node* Statement::predicate() const
{
  return 0L;
}

Node* Statement::object() const
{
  return 0L;
}

void Statement::clear()
{
  librdf_statement_clear(d->statement);
}