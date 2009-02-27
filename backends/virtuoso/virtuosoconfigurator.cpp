/*
 * This file is part of Soprano Project
 *
 * Copyright (C) 2009 Sebastian Trueg <trueg@kde.org>
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

#include "virtuosoconfigurator.h"
#include "odbcconnection.h"
#include "odbcqueryresult.h"

#include <QtCore/QSet>
#include <QtCore/QDebug>

#include "node.h"

namespace {
    QStringList normalizeIndexNames( const QStringList& indices ) {
        QStringList normalized;
        foreach( const QString& index, indices )
            normalized << index.toUpper();
        return normalized;
    }

    bool verifyIndex( const QString& index ) {
        if ( index.length() != 4 )
            return false;
        if ( !index.count( 'S' ) )
            return false;
        if ( !index.count( 'P' ) )
            return false;
        if ( !index.count( 'O' ) )
            return false;
        if ( !index.count( 'G' ) )
            return false;
        return true;
    }

    QStringList defaultIndices() {
        return QStringList() << QLatin1String( "SPOG" )
                             << QLatin1String( "POSG" )
                             << QLatin1String( "OPSG" )
                             << QLatin1String( "GSOP" );
    }
}


Soprano::Virtuoso::DatabaseConfigurator::DatabaseConfigurator( ODBC::Connection* conn )
    : m_connection( conn )
{
}


Soprano::Virtuoso::DatabaseConfigurator::~DatabaseConfigurator()
{
}


bool Soprano::Virtuoso::DatabaseConfigurator::configureServer( const BackendSettings& settings )
{
    // update indices
    QString indices = valueInSettings( settings, BackendOptionUser, "indices" ).toString();
    if ( !indices.isEmpty() ) {
        if ( !updateIndices( indices ) )
            return false;
    }

    // update fulltext index state
    QString fulltextIndex = valueInSettings( settings, BackendOptionUser, "fulltextindex" ).toString();
    if ( !fulltextIndex.isEmpty() ) {
        if ( !updateFulltextIndexState( fulltextIndex ) )
            return false;
    }

    return true;
}


bool Soprano::Virtuoso::DatabaseConfigurator::updateIndices( const QString& indices_ )
{
    qDebug() << Q_FUNC_INFO << indices_;

    Q_ASSERT( !indices_.isEmpty() );

    QStringList indices = normalizeIndexNames( indices_.split( ',', QString::SkipEmptyParts ) );
    QStringList currentIndices = configuredIndices();

    if ( indices.isEmpty() )
        indices = defaultIndices();

    // sort them for comparision
    qSort( indices );
    qSort( currentIndices );

    // FIXME: how about the primary key which defaults to (G,S,P,O)? We could change that, too.

    QStringList newIndices = ( currentIndices.toSet() - indices.toSet() ).toList();

    if ( currentIndices.isEmpty() || indices != currentIndices ) {
        qDebug() << "indices need updating";

        // drop all indices on the RDF_QUAD table which are no longer needed
        foreach( const QString& index, newIndices ) {
            m_connection->executeCommand( QLatin1String( "DROP INDEX RDF_QUAD_" ) + index.toUpper() );
        }

        // add the new indices
        foreach( const QString& index, indices.toSet() - currentIndices.toSet() ) {
            if ( verifyIndex( index ) ) {
                newIndices << index;
                // FIXME: bitmap indices can only end in an IRI, i.e. not in O. Can we use a normal index instead?
                m_connection->executeCommand( QString( "create bitmap index RDF_QUAD_%1 on DB.DBA.RDF_QUAD "
                                                       "(%2, %3, %4, %5) partition (O varchar (-1, 0hexffff))" )
                                              .arg( index )
                                              .arg( index[0] )
                                              .arg( index[1] )
                                              .arg( index[2] )
                                              .arg( index[3] ) );
            }
            else {
                qDebug() << "Invalid index specified:" << index;
            }
        }
    }

    return true;
}


QStringList Soprano::Virtuoso::DatabaseConfigurator::configuredIndices()
{
    QStringList indices;

    QString query = QLatin1String( "SELECT DISTINCT SUBSTRING(ISS_KEY_NAME,10,4) "
                                   "FROM SYS_INDEX_SPACE_STATS WHERE "
                                   "ISS_KEY_TABLE='DB.DBA.RDF_QUAD' AND "
                                   "ISS_KEY_NAME LIKE 'RDF_QUAD_*'" );
    ODBC::QueryResult* result = m_connection->executeQuery( query );
    if ( result ) {
        while ( result->fetchScroll() ) {
            indices << result->getData( 1 ).toString();
        }
    }

    qDebug() << Q_FUNC_INFO << indices;

    return indices;
}


bool Soprano::Virtuoso::DatabaseConfigurator::updateFulltextIndexState( const QString& state )
{
    // determine new state
    bool updateIntervalSpecified = false;
    state.toInt( &updateIntervalSpecified );
    bool syncUpdating = state.toLower() == QLatin1String( "sync" );
    bool enableIndexing = syncUpdating || updateIntervalSpecified;

    // enable/disable the full text index
    if ( !updateFulltextIndexRules( enableIndexing ) )
        return false;

    // set the batch updating behaviour:
    // for sync updating batch updating needs to be disabled (second param "OFF")
    // for batch updating the third parameter specifies the interval in minutes.
    return( m_connection->executeCommand( QString( "DB.DBA.VT_BATCH_UPDATE ('DB.DBA.RDF_OBJ', '%1', %2)" )
                                          .arg( ( enableIndexing && !syncUpdating ) ? QLatin1String( "ON" ) : QLatin1String( "OFF" ) )
                                          .arg( ( enableIndexing && updateIntervalSpecified ) ? state : QLatin1String( "null" ) ) )
            == Error::ErrorNone );
}


bool Soprano::Virtuoso::DatabaseConfigurator::updateFulltextIndexRules( bool enable )
{
    QString query = QLatin1String( "SELECT * FROM DB.DBA.RDF_OBJ_FT_RULES WHERE ROFR_REASON='Soprano'" );

    bool haveRule = false;

    ODBC::QueryResult* result = m_connection->executeQuery( query );
    if ( result && result->fetchScroll() ) {
        haveRule = true;
    }

    if ( enable && !haveRule ) {
        return m_connection->executeCommand( QLatin1String( "DB.DBA.RDF_OBJ_FT_RULE_ADD( null, null, 'Soprano' )" ) ) == Error::ErrorNone;
    }
    else if ( !enable && haveRule ) {
        return m_connection->executeCommand( QLatin1String( "DB.DBA.RDF_OBJ_FT_RULE_DEL( null, null, 'Soprano' )" ) ) == Error::ErrorNone;
    }
    else {
        return true;
    }
}
