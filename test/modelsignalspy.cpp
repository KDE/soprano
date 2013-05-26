/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2006 Sebastian Trueg <strueg@mandriva.com>
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

#include "modelsignalspy.h"
#include "../soprano/model.h"

#include <QtCore/QTextStream>


Soprano::ModelSignalSpy::ModelSignalSpy( QObject* parent )
    : QObject( parent ),
      m_model( 0 )
{
}


Soprano::ModelSignalSpy::~ModelSignalSpy()
{
}


void Soprano::ModelSignalSpy::setModel( Model* model )
{
    if ( m_model != model ) {
        if ( m_model ) {
            m_model->disconnect( this );
        }
        m_model = model;
        if ( model ) {
            connect( model, SIGNAL(statementsAdded()), this, SLOT(slotStatementsAdded()) );
            connect( model, SIGNAL(statementsRemoved()), this, SLOT(slotStatementsRemoved()) );
            connect( model, SIGNAL(statementAdded(Soprano::Statement)), this, SLOT(slotStatementAdded(Soprano::Statement)) );
            connect( model, SIGNAL(statementRemoved(Soprano::Statement)), this, SLOT(slotStatementRemoved(Soprano::Statement)) );
        }
    }
}


void Soprano::ModelSignalSpy::slotStatementsAdded()
{
    QTextStream s( stdout );
    s << "statementsAdded()" << endl;
}


void Soprano::ModelSignalSpy::slotStatementsRemoved()
{
    QTextStream s( stdout );
    s << "statementsRemoved()" << endl;
}


void Soprano::ModelSignalSpy::slotStatementAdded( const Soprano::Statement& statement )
{
    QTextStream s( stdout );
    s << "statementAdded(" << statement << ")" << endl;
}


void Soprano::ModelSignalSpy::slotStatementRemoved( const Soprano::Statement& statement )
{
    QTextStream s( stdout );
    s << "statementRemoved(" << statement << ")" << endl;
}



