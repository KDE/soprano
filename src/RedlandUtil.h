/* This file is part of Soprano
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

#ifndef SOPRANO_REDLAND_UTIL_H
#define SOPRANO_REDLAND_UTIL_H

#include <redland.h>

namespace Soprano
{

class Node;
class Query;
class Statement;

namespace Redland
{
  Node createNode( librdf_node *node );

  librdf_node *createNode( const Node &node );

  Statement createStatement( librdf_statement *st );
  
  librdf_statement *createStatement( const Statement &statement );

  const char *queryType( const Query &query );
}

}

#endif // SOPRANO_REDLAND_UTIL_H
