/* This file is part of QRDF
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
 * write to the Free Software Foundation, Inc., 51 Franklin Street,
 * Fifth
 * Floor, Boston, MA 02110-1301, USA.
 */

#ifndef RDF_REDLAND_UTIL_H
#define RDF_REDLAND_UTIL_H

#include <redland.h>
#include "Node.h"
#include "Statement.h"
#include "Query.h"

namespace RDF
{

namespace Redland
{
  Node *createNode( librdf_node *node );

  librdf_node *createNode( librdf_world *world, const Node &node );

  librdf_statement *createStatement( librdf_world *world, const Statement &statement );

  const char *queryType( const Query &query );
}

}

#endif // RDF_REDLAND_UTIL_H
