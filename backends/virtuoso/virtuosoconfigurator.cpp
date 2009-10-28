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
    QStringList normalizeIndexNames( const QStringList& indexes ) {
        QStringList normalized;
        foreach( const QString& index, indexes )
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

    QStringList defaultIndexes() {
        return QStringList() << QLatin1String( "SPOG" )
                             << QLatin1String( "POSG" )
                             << QLatin1String( "OPSG" )
                             << QLatin1String( "GSPO" )
                             << QLatin1String( "GPOS" );
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
    // update indexes
    QString indexes = valueInSettings( settings, "indexes", "OPGS,POGS,GPOS" ).toString();
    if ( !indexes.isEmpty() ) {
        if ( !updateIndexes( indexes ) )
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


bool Soprano::Virtuoso::DatabaseConfigurator::updateIndexes( const QString& indexes_ )
{
    qDebug() << Q_FUNC_INFO << indexes_;

    Q_ASSERT( !indexes_.isEmpty() );

    QStringList indexes = normalizeIndexNames( indexes_.split( ',', QString::SkipEmptyParts ) );
    QStringList currentIndexes = configuredIndexes();

    if ( indexes.isEmpty() )
        indexes = defaultIndexes();

    // sort them for comparision
    qSort( indexes );
    qSort( currentIndexes );

    QStringList newIndexes = ( currentIndexes.toSet() - indexes.toSet() ).toList();

    if ( currentIndexes.isEmpty() || indexes != currentIndexes ) {
        qDebug() << "indexes need updating";

        // drop all indexes on the RDF_QUAD table which are no longer needed
        foreach( const QString& index, newIndexes ) {
            m_connection->executeCommand( QLatin1String( "DROP INDEX RDF_QUAD_" ) + index.toUpper() );
        }

        // add the new indexes
        // TODO: convert the data if necessary (I think that is if indexes are dropped)
        foreach( const QString& index, indexes.toSet() - currentIndexes.toSet() ) {
            if ( verifyIndex( index ) ) {
                newIndexes << index;
                // bitmap indexes cannot end with O
                m_connection->executeCommand( QString( "create %6 index RDF_QUAD_%1 on DB.DBA.RDF_QUAD "
                                                       "(%2, %3, %4, %5) partition (O varchar (-1, 0hexffff))" )
                                              .arg( index )
                                              .arg( index[0] )
                                              .arg( index[1] )
                                              .arg( index[2] )
                                              .arg( index[3] )
                                              .arg( index[3] != 'O' ? QString( QLatin1String( "bitmap" ) ) : QString() ) );
            }
            else {
                qDebug() << "Invalid index specified:" << index;
            }
        }
    }

    return true;
}


QStringList Soprano::Virtuoso::DatabaseConfigurator::configuredIndexes()
{
    QStringList indexes;

    QString query = QLatin1String( "SELECT DISTINCT SUBSTRING(ISS_KEY_NAME,10,4) "
                                   "FROM SYS_INDEX_SPACE_STATS WHERE "
                                   "ISS_KEY_TABLE='DB.DBA.RDF_QUAD' AND "
                                   "ISS_KEY_NAME LIKE 'RDF_QUAD_*'" );
    ODBC::QueryResult* result = m_connection->executeQuery( query );
    if ( result ) {
        while ( result->fetchRow() ) {
            indexes << result->getData( 1 ).toString();
        }
    }

    qDebug() << Q_FUNC_INFO << indexes;

    return indexes;
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
    QString query = QLatin1String( "SELECT ROFR_REASON FROM DB.DBA.RDF_OBJ_FT_RULES WHERE ROFR_G=null AND ROFR_P=null" );

    bool haveRule = false;

    QString ruleName = QLatin1String( "Soprano" );
    ODBC::QueryResult* result = m_connection->executeQuery( query );
    if ( result && result->fetchRow() ) {
        haveRule = true;
        QString name = result->getData( 0 ).toString();
        if ( !name.isEmpty() ) {
            qDebug() << Q_FUNC_INFO << "Found existing rule with name" << name;
            ruleName = name;
        }
        else {
            qDebug() << Q_FUNC_INFO << "empty rule name!";
        }
    }

    if ( enable && !haveRule ) {
        return m_connection->executeCommand( QString( "DB.DBA.RDF_OBJ_FT_RULE_ADD( null, null, '%1' )" ).arg( ruleName ) ) == Error::ErrorNone;
    }
    else if ( !enable && haveRule ) {
        return m_connection->executeCommand( QString( "DB.DBA.RDF_OBJ_FT_RULE_DEL( null, null, '%1' )" ).arg( ruleName ) ) == Error::ErrorNone;
    }
    else {
        return true;
    }
}
