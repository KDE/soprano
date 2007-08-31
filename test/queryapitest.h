/* 
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007 Daniele Galdi <daniele.galdi@gmail.com>
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

#ifndef QUERY_API_TEST_H
#define QUERY_API_TEST_H

#include <QObject>
#include <QString>

class Soprano::Error::Locator;

class QueryAPITest: public QObject
{
Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();

    void testQuery_1();    

    void testSyntaxError();

    void syntaxErrorFounded( const Soprano::Error::Locator &locator, const QString& message );

private:
    Soprano::PluginManager *m_manager;
};

#endif // QUERY_API_TEST_H

