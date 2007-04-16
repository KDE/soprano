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

#include "StatementTest.h"

#include <QtTest/QTest>
#include <QtCore/QUrl>
#include <QtCore/QCoreApplication>

#include <soprano/soprano.h>

using namespace Soprano;

void StatementTest::testCreateEmptyStatement()
{
  Statement st;
  QVERIFY( !st.isValid() );
}

void StatementTest::testCreateStatement()
{
  Node subject(QUrl("uri:soprano:test"));
  Node predicate(QUrl("uri:soprano:predicate"));
  Node object(QString("Literal value"));

  Statement st(subject, predicate, object);

  QVERIFY( st.isValid() );
}

void StatementTest::testStatementInvariant()
{
  Node subject(QUrl("uri:soprano:test"));
  Node predicate(QUrl("uri:soprano:predicate"));
  Node object(QString("Literal value"));

  Statement st(subject, predicate, object);

  QVERIFY( st.isValid() );

  Node s;
  st.setSubject( s );
  QVERIFY( !st.isValid() );

  QVERIFY( st.subject() == s );
  QVERIFY( st.predicate() == predicate );
  QVERIFY( st.object() == object );
}

QTEST_MAIN(StatementTest)

#include "StatementTest.moc"
