
#include <QtGlobal>
#include "stream.h"

using namespace RDF;

struct Stream::Private
{
  Private() : stream(0L)
  {}
  librdf_stream *stream;
};

Stream::Stream()
{
  d = new Private;
  d->stream = librdf_new_stream();
  Q_ASSERT(d->stream != NULL);
  librdf_stream_open(d->stream);
}

Stream::~Stream()
{
  librdf_free_stream(d->stream);
}

librdf_stream* Stream::streamPtr()
{
  return d->stream;
}

