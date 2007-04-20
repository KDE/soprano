/* This file is part of Soprano Project.
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
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef SOPRANO_STATEMENT_H
#define SOPRANO_STATEMENT_H

#include <QtCore/QSharedDataPointer>


#include <soprano/node.h>
#include <soprano/soprano_export.h>

namespace Soprano
{

class SOPRANO_EXPORT Statement
{
public:

  /**
   * Default Constructor, build an Empty(not valid) Statement.
   */
  Statement();

  /**
   * Build a Statement with the given subject, predicate and object.
   *
   * \param subject The subject.
   *
   * \param predicate The predicate.
   *
   * \param object The object.
   *
   * \param context The context node.
   */
  Statement( const Node &subject, const Node &predicate, const Node &object, const Node &context = Node() );

  Statement( const Statement &other );

  virtual ~Statement();

  Statement& operator=( const Statement& other );

  bool operator==( const Statement& other );

  /**
   * Change the Statement subject.
   *
   * \param subject The new subject.
   */
  void setSubject( const Node &subject );

  /**
   * \return The subject.
   */
  Node subject() const;

  /**
   * Change the Statement predicate.
   *
   * \param predicate The new predicate.
   */
  void setPredicate( const Node &predicate );

  /**
   * \return The predicate.
   */
  Node predicate() const;

  /**
   * Change the Statement object.
   *
   * \param object The new object.
   */
  void setObject( const Node &object );

  /**
   * \return The object.
   */
  Node object() const;

  /**
   * Change the Statement context.
   *
   * \param context The new Context.
   */
  void setContext( const Node &context );

  /**
   * \return The Context node.
   */
  const Node &context() const;

  /**
   * A Statement is valid if subject, predicate and object are valid.
   *
   * \return @p true if the Statement is valid, @p false otherwise
   */
  bool isValid() const;

private:
  class Private;
  QSharedDataPointer<Private> d;
};

}

SOPRANO_EXPORT QDebug operator<<( QDebug s, const Soprano::Statement& );

#endif // SOPRANO_STATEMENT_H

