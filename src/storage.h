
#ifndef STORAGE_H
#define STORAGE_H

#include <QString>

#include <redland.h>

namespace RDF
{

class Storage
{
public:
  Storage(const QString &type, const QString &name, const QString &options );
  ~Storage();
  librdf_storage* storagePtr();
private:
  class Private;
  Private *d;
};


}

#endif

