/*
 * This file is part of Soprano Project.
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

#include <QtCore>
#include <soprano/soprano_export.h>

namespace Soprano
{

class Node;

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
   */
  Statement( const Node &subject, const Node &predicate, const Node &object );

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
  const Node &subject() const;

  /**
   * Change the Statement predicate.
   *
   * \param predicate The new predicate.
   */
  void setPredicate( const Node &predicate );

  /**
   * \return The predicate.
   */
  const Node &predicate() const;

  /**
   * Change the Statement object.
   *
   * \param object The new object.
   */
  void setObject( const Node &object );

  /**
   * \return The object.
   */
  const Node &object() const;

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

#endif // SOPRANO_STATEMENT_H

