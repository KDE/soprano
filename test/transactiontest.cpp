/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2008 Sebastian Trueg <trueg@kde.org>
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

#include "transactiontest.h"
#include "soprano.h"

#include <QtCore/QUuid>
#include <QtTest>


using namespace Soprano;

namespace {
    QUrl createRandomUri()
    {
        // FIXME: check if the uri already exists
        QString uid = QUuid::createUuid().toString();
        uid = uid.mid( 1, uid.length()-2 );
        return QUrl( "http://soprano.org/test#" + uid );
    }


    QList<Statement> createTestData( const Statement& s, int num )
    {
        QList<Statement> sl;
        for( int i = 0; i < num; ++i ) {
            sl.append( Statement( s.subject().isEmpty() ? Node( createRandomUri() ) : s.subject(),
                                  s.predicate().isEmpty() ? Node( createRandomUri() ) : s.predicate(),
                                  s.object().isEmpty() ? Node( createRandomUri() ) : s.object(),
                                  s.context() ) );
        }
        return sl;
    }

    bool compareLists( const QList<Statement> l1, const QList<Statement>& l2 ) {
        foreach( const Statement& s, l1 ) {
            if( !l2.contains( s ) ) {
                qDebug() << s << "not in list 2";
                return false;
            }
        }
        foreach( const Statement& s, l2 ) {
            if( !l1.contains( s ) ) {
                qDebug() << s << "not in list 1";
                return false;
            }
        }
        return true;
    }
}


void TransactionTest::deleteModel( Soprano::Model* m )
{
    delete m;
}


void TransactionTest::testStartTransaction()
{
    Model* model = createModel();
    QVERIFY( model );
    Transaction* t = model->startTransaction();
    QVERIFY( t );
    delete t;
}


void TransactionTest::testAddStatement()
{
    Model* model = createModel();
    QVERIFY( model );
    Transaction* t = model->startTransaction();
    QVERIFY( t );

    Statement s = createTestData( Statement(),  1 ).first();
    t->addStatement( s );
    QVERIFY( t->containsStatement( s ) );
    QVERIFY( !model->containsStatement( s ) );

    t->commit();
    QVERIFY( model->containsStatement( s ) );

    delete t;
    deleteModel( model );
}


void TransactionTest::testRemoveStatement()
{
    Model* model = createModel();
    QVERIFY( model );
    Transaction* t = model->startTransaction();
    QVERIFY( t );

    Statement s = createTestData( Statement(),  1 ).first();
    model->addStatement( s );

    t->removeStatement( s );
    QVERIFY( !t->containsStatement( s ) );
    QVERIFY( model->containsStatement( s ) );

    t->commit();
    QVERIFY( !model->containsStatement( s ) );

    delete t;
    deleteModel( model );
}


void TransactionTest::testListStatements()
{
    Model* model = createModel();
    QVERIFY( model );
    Transaction* t = model->startTransaction();
    QVERIFY( t );

    // add some statement to the model and the transaction
    QList<Statement> msl = createTestData( Statement(), 5 );
    QList<Statement> tsl = createTestData( Statement(), 5 );

    QVERIFY( model->addStatements( msl ) == Error::ErrorNone );
    QVERIFY( t->addStatements( tsl ) == Error::ErrorNone );

    QList<Statement> tslisted = t->listStatements().allStatements();
    QCOMPARE( tslisted.count(), ( msl + tsl ).count() );
    QVERIFY( compareLists( tslisted, msl + tsl ) );
}

#include "transactiontest.moc"
