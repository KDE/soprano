/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007 Sebastian Trueg <trueg@kde.org>
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

#include "storagemodel.h"

#include "statement.h"
#include "statementiterator.h"
#include "pluginmanager.h"
#include "queryresultiterator.h"
#include "query/queryparser.h"
#include "query/queryserializer.h"

#include <QtCore/QList>


class Soprano::StorageModel::Private
{
public:
    const Backend* backend;
};

Soprano::StorageModel::StorageModel( const Backend* backend )
    : d( new Private() )
{
    d->backend = backend;
}

Soprano::StorageModel::~StorageModel()
{
    delete d;
}

const Soprano::Backend* Soprano::StorageModel::backend() const
{
    return d->backend;
}

bool Soprano::StorageModel::isEmpty() const
{
    return statementCount() == 0;
}

bool Soprano::StorageModel::containsStatement( const Statement &statement ) const
{
    if ( !statement.isValid() ) {
        setError( "Cannot check for invalid statement", Error::ERROR_INVALID_ARGUMENT );
        return false;
    }

    return listStatements( statement ).allStatements().contains( statement );
}

bool Soprano::StorageModel::containsAnyStatement( const Statement &statement ) const
{
    return listStatements( statement ).next();
}

Soprano::Error::ErrorCode Soprano::StorageModel::removeAllStatements( const Statement &statement )
{
    QList<Statement> sl = listStatements( statement ).allStatements();
    for ( QList<Statement>::const_iterator it = sl.constBegin(); it != sl.constEnd(); ++it ) {
        Error::ErrorCode r = removeStatement( *it );
        if ( r != Error::ERROR_NONE ) {
            return r;
        }
    }

    return Error::ERROR_NONE;
}


Soprano::QueryResultIterator Soprano::StorageModel::executeQuery( const Query::Query& query ) const
{
    const Query::Serializer* serializer = PluginManager::instance()->discoverQuerySerializerForQueryLanguage( Query::QUERY_LANGUAGE_SPARQL );
    if ( !serializer ) {
        setError( "No SPARQL query serializer plugin found." );
        return QueryResultIterator();
    }
    QString queryString;
    QTextStream s( &queryString );
    if ( serializer->serializeQuery( query, s, Query::QUERY_LANGUAGE_SPARQL ) ) {
        return executeQuery( queryString, Query::QUERY_LANGUAGE_SPARQL );
    }
    else {
        setError( serializer->lastError() );
        return QueryResultIterator();
    }
}


Soprano::QueryResultIterator Soprano::StorageModel::executeQuery( const QString& query, Query::QueryLanguage language, const QString& userQueryLanguage ) const
{
    const Query::Parser* parser = PluginManager::instance()->discoverQueryParserForQueryLanguage( language, userQueryLanguage );
    if ( !parser ) {
        setError( QString( "No query parser plugin for language %1 found." ).arg( Query::queryLanguageToString( language, userQueryLanguage ) ) );
        return QueryResultIterator();
    }
    Query::Query queryObject = parser->parseQuery( query, language, userQueryLanguage );
    if ( queryObject.isValid() ) {
        return executeQuery( queryObject );
    }
    else {
        setError( parser->lastError() );
        return QueryResultIterator();
    }
}

#include "storagemodel.moc"
