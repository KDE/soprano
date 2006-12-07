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
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef SOPRANO_MODEL_H
#define SOPRANO_MODEL_H

#include <QList>

class QString;
class QTextStream;
class QUrl;

namespace Soprano
{

class Node;
class Query;
class QueryResult;
class Statement;
class StatementIterator;

class Model
{
public:
  enum ExitCode {
    ERROR_EXIT = 0,
    SUCCESS_EXIT
  };

  virtual ~Model();

  /**
   * Add all the statements in another model to this model.
   *
   * \param model The Model to add.
   */
  Model::ExitCode add( const Model &model );

  /**
   * Add all the statements to the Model, using through the bulk
   * update interface.
   *
   * \param iter An iterator over Statements.
   */
  Model::ExitCode add( const StatementIterator &iter );

  /**
   * Add all the statements to the Model.
   *
   * \param statements A list of Statement.
   */
  Model::ExitCode add( const QList<Statement> statements );

  /**
   * Adds the Statement to the Model.
   *
   * \param st The Statement to add.
   */
  virtual Model::ExitCode add( const Statement &st ) = 0;

  /**
   * Create a Predicate with the given namespace and value.
   * es. createPredicate("http://www.kde.org", "predicate") returns
   *     a predicate node with this value 'http://www.kde.org#predicate'
   * If the ns is empty build the predicate with only the given values.
   *
   * \return The predicate Node.
   */
  Node createPredicate( const QString &ns, const QString &value );

  /**
   * \return The predicate Node.
   */
  Node createPredicate( const QUrl &uri );

  /**
   * \return The blank Node.
   */
  Node createBlank( const QString &uri );

  /**
   * \return The resource Node.
   */
  Node createResource( const QUrl &uri );

  /**
   * \return The literal Node.
   */
  Node createLiteral( const QString &literal );

  /**
   * \return true if the Model doesn't contains any Statement.
   */
  bool isEmpty() const;

  /**
   * \return true if the Model contains the given Statement.
   */
  virtual bool contains( const Statement &statement ) const = 0;
  
  /**
   * Execute the given query over the Model.
   *
   * \return All the Statements that match the query. NULL on error.
   */
  // FIXME: returning a pointer is not a good idea becasue this way the user
  //        has to delete it manually
  virtual QueryResult *executeQuery( const Query &query ) const = 0;

  /**
   * \return An iterator for all the contained Statements. NULL on error.
   */ 
  // FIXME: returning a pointer is not a good idea becasue this way the user
  //        has to delete it manually
  StatementIterator *listStatements() const;

  /**
   * Return an iterator over the Models statements that "partial"
   * match the input Statement.
   *
   * \param partial The partial Statement to match.
   *
   * \return An iterator for all the matched Statements. NULL on error.
   */
  // FIXME: returning a pointer is not a good idea becasue this way the user
  //        has to delete it manually
  virtual StatementIterator *listStatements( const Statement &partial ) const = 0;

  /**
   * Return an iterator over the Models statements that "partial"
   * match the input parameter.
   *
   * \param subject The Subject node (can be empty)
   * \param predicate The Predicate node (can be empty)
   * \param object The Object node (can be empty)
   *
   * \return An iterator for all the matched Statements. NULL on error.
   */
  // FIXME: returning a pointer is not a good idea becasue this way the user
  //        has to delete it manually
  StatementIterator *listStatements( const Node &subject, const Node &predicate, const Node &object ) const;

  /**
   * Remove the given Statement from the Model.
   *
   * \param st The Statement to remove.
   */
  virtual Model::ExitCode remove( const Statement &st ) = 0;

  /**
   * Remove all the statements matching (s, p, o) from this model.
   *
   * \param subject The Subject node (can be empty)
   * \param predicate The Predicate node (can be empty)
   * \param object The Object node (can be empty)
   */
  Model::ExitCode removeAll( const Node &subject, const Node &predicate, const Node &object );

  /**
   * Remove all the statements from this model.
   */
  Model::ExitCode removeAll();

  /**
   * \return The size of the Model (number of Stamenent)
   */
  virtual int size() const = 0;

  /**
   * Write a model to a textstream.
   *
   * \param The stream.
   *
   * \return Model::ERROR_EXIT if not implemented or an Error occurred. 
   */
  virtual Model::ExitCode write( QTextStream &os ) const = 0;

  /**
   * Print the Model to the stdout.
   *
   * \return Model::ERROR_EXIT if not implemented or an Error occurred. 
   */
  virtual Model::ExitCode print() const = 0;
};

}

#endif // SOPRANO_MODEL_H

