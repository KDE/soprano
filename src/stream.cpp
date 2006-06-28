
#include <QtGlobal>
#include "stream.h"

using namespace RDF;

struct Stream::Private
{
  Private() : stream(0L)
  {}
  librdf_stream *stream;
};

Stream::Stream( librdf_stream *stream )
{
  Q_ASSERT(d->stream != NULL);
  d = new Private;
  d->stream = stream;
}

Stream::~Stream()
{
  librdf_free_stream(d->stream);
  delete d;
}

librdf_stream* Stream::streamPtr()
{
  return d->stream;
}

