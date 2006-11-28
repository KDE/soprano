/* 
 * This file is part of Soprano Project
 *
 * Copyright (C) 2006 Daniele Galdi <daniele.galdi@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; see the file COPYING.LIB.  If not, 
 * write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301, USA.
 */

#ifndef SOPRANO_BACKEND_NEPOMUK_UTIL_H
#define SOPRANO_BACKEND_NEPOMUK_UTIL_H

#include <knep/services/rdf.h>
using namespace Nepomuk::Backbone::Services;

namespace Soprano
{

class Statement;
class Node;

namespace Backend 
{
namespace Nepomuk
{

class Util
{
public:
  static Soprano::Statement createStatement( const RDF::Statement &st );

  static RDF::Statement createStatement( const Soprano::Statement &st );

  static Soprano::Node createNode( const RDF::Node &node );

  static RDF::Node createNode( const Soprano::Node &node );
};

}
}
}

#endif
