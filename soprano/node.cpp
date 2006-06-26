
#include <QtGlobal>
#include "node.h"

using namespace RDF;

struct Node::Private
{
  Private() : node(0L)
  {}
  librdf_node *node;
};

Node::Node(const Node &rhs)
{
  d = new Private;
  d->node = librdf_new_node_from_node(rhs.nodePtr());
  Q_ASSERT(d->node != NULL);
}

Node::Node(World *world, const QUrl &url)
{
  d = new Private;
  d->node = librdf_new_node_from_uri_string(world->worldPtr(), (const unsigned char*) url.toString().toLatin1().constData());
  Q_ASSERT(d->node != NULL);
}

Node::Node(World *world, const QString &literal)
{
  d = new Private;
  d->node = librdf_new_node_from_literal(world->worldPtr(), (unsigned unsigned char*) literal.toLatin1().constData(), NULL, 0);
  Q_ASSERT(d->node != NULL);
}

Node::~Node()
{
  librdf_free_node(d->node);
}

librdf_node* Node::nodePtr() const
{
  return d->node;
}

bool Node::isLiteral() const
{
  return ( librdf_node_is_literal(d->node) != 0 );
}

bool Node::isResource() const
{
  return ( librdf_node_is_resource(d->node) != 0 );
}

bool Node::isBlank() const
{
  return ( librdf_node_is_blank(d->node) != 0 );
}

QString Node::toString() const
{
  return QString( (const char *) librdf_node_to_string(d->node));
}

Node::NodeType Node::nodeType() const
{
  switch (librdf_node_get_type (d->node))
  {
    case LIBRDF_NODE_TYPE_RESOURCE:
    return TypeResource;
    break;
    case LIBRDF_NODE_TYPE_LITERAL:
    return TypeLiteral;
    break;
    case LIBRDF_NODE_TYPE_BLANK:
    return TypeBlank;
    break;
    default:
    return TypeUnknown; 
  }
}

QUrl Node::url() const
{
  return QUrl( (const char *) librdf_uri_as_string(librdf_node_get_uri (d->node)));
}

QString Node::literal() const
{
  return QString( (const char *) librdf_node_get_literal_value(d->node) );
}