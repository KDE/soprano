
#include <QtGlobal>

#include "world.h"
#include "storage.h"

using namespace RDF;

struct Storage::Private
{
  librdf_storage* storage;
  World world;
};

Storage::Storage(const QString &type, const QString &name, const QString &options )
{
  d = new Private;
  d->storage = librdf_new_storage(d->world.worldPtr(), type.toLatin1().data(), name.toLatin1().data(), options.toLatin1().data());
  Q_ASSERT(d->storage != NULL);
   
}

Storage::~Storage()
{
  librdf_free_storage(d->storage);
  delete d;
}

librdf_storage* Storage::storagePtr()
{
  return d->storage;
}

