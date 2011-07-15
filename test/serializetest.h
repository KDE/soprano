/* 
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007-2008 Sebastian Trueg <strueg@mandriva.com>
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

#ifndef SOPRANO_SERIALIZER_TEST_H
#define SOPRANO_SERIALIZER_TEST_H

#include <QtCore/QObject>
#include <QtCore/QList>

#include "statement.h"


/* Workflow:
 * 1) Prepare one QList<Statement> with data, used as reference
 * 2) For each serializer:
 *    2.1) Serialize reference to memory
 *    2.2) Parse this memory back to QList<Statement> 
 *    2.3) Compare with reference
 */
class SerializerTest: public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void init();
    void testSerializer_data();
    void testSerializer();
    //void testEncoding();
    private:
        //QList<Soprano::Statement> referenceStatements;
};

#endif

