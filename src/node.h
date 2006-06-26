
#ifndef NODE_H
#define NODE_H

#include <QString>
#include <QUrl>
#include <redland.h>
#include "world.h"

namespace RDF
{

class Node
{
public:

  enum NodeType {
    TypeUnknown = 0,
    TypeResource = 1,
    TypeLiteral = 2,
    TypeBlank = 4,
    TypeLast = TypeBlank
  };

  Node(const Node &rhs);
  Node(World *world, const QUrl &url);
  Node(World *world, const QString &literal);
  Node::NodeType nodeType() const;
  bool isLiteral() const;
  bool isResource() const;
  bool isBlank() const;

  QUrl url() const;
  QString literal() const;

  QString toString() const;
  ~Node();
  librdf_node* nodePtr() const;
private:
  class Private;
  Private *d;
};


}

#endif


