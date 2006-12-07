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

#ifndef SOPRANO_MODEL_FACTORY_H
#define SOPRANO_MODEL_FACTORY_H

#include <QString>

namespace Soprano
{

class Model;
class Parser;

class ModelFactory
{
public:
  virtual ~ModelFactory();

  /**
   * Create a new Model stored into the memory.
   *
   * \param name
   *            The Model name
   *
   * \return a new Model or NULL on error
   */
  virtual Model *createMemoryModel( const QString &name ) const = 0;

  /**
   * Create a new Model stored to a persistent layer.
   *
   * \param name
   *            The Model name
   *
   * \return a new Model or NULL on error
   */
  virtual Model *createPersistentModel( const QString &name, const QString &path="." ) const = 0;

  /**
   * Create a new Parser
   */
  virtual Parser *createParser() const = 0;
};

}

#endif // SOPRANO_MODEL_FACTORY_H

