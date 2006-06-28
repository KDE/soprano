
#ifndef STREAM_H
#define STREAM_H

#include <redland.h>

namespace RDF
{

class Stream
{
public:
  Stream( librdf_stream *stream );
  ~Stream();
  librdf_stream* streamPtr();
private:
  class Private;
  Private *d;
};


}

#endif

