/*
 * This file is part of the Soprano Project.
 *
 * Copyright (C) 2007 Rajeev J Sebastian <rajeev.sebastian@gmail.com>
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

#include "sparqlmodel.h"
#include "sparqlqueryresult.h"
#include "synchonoussparqlprotocol.h"

#include "queryresultiterator.h"
#include "statementiterator.h"
#include "nodeiterator.h"
#include "sopranotypes.h"
#include "parser.h"
#include "pluginmanager.h"

#include <QtCore/QDebug>


// FIXME: how about a method in Node that creates the SPARQL representation (or is called N3)?

namespace {
    QString nodeToN3( const Soprano::Node& node )
    {
        if ( node.isLiteral() ) {
            Soprano::LiteralValue v = node.literal();
            return QString( "\"%1\"^^<%2>" ).arg( v.toString() ).arg( v.dataTypeUri().toString() );
        }
        else if ( node.isResource() ) {
            return QString( "<%1>" ).arg( QString::fromAscii( node.uri().toEncoded() ) );
        }
        else if ( node.isBlank() ) {
            return "_:" + node.identifier();
        }
        else {
            return QString();
        }
    }

    QString statementToConstructGraphPattern( const Soprano::Statement& s, bool withContext = false )
    {
        QString query;

        if ( withContext ) {
            query += "graph { ";
            if ( s.context().isValid() ) {
                query += "<" + QString::fromAscii( s.context().uri().toEncoded() ) + ">";
            }
            else {
                query += "?g";
            }
            query += " { ";
        }

        if ( s.subject().isValid() ) {
            query += nodeToN3( s.subject() ) + ' ';
        }
        else {
            query += "?s ";
        }

        if ( s.predicate().isValid() ) {
            query += nodeToN3( s.predicate() ) + ' ';
        }
        else {
            query += "?p ";
        }

        if ( s.object().isValid() ) {
            query += nodeToN3( s.object() );
        }
        else {
            query += "?o";
        }

        if ( withContext ) {
            query += " }";
        }

        return query;
    }
}


class Soprano::Client::SparqlModel::Private
{
public:
    SynchronousSparqlProtocol* client;
};


Soprano::Client::SparqlModel::SparqlModel( const QString& endpoint,
                               quint16 port,
                               const QString& user,
                               const QString& password )
    : Soprano::Model(),
      d( new Private() )
{
    d->client = new SynchronousSparqlProtocol( this );
    d->client->setHost( endpoint, port );
    d->client->setUser( user, password );
}


Soprano::Client::SparqlModel::~SparqlModel()
{
    delete d;
}


void Soprano::Client::SparqlModel::setHost( const QString& hostname, quint16 port )
{
    d->client->setHost( hostname, port );
}


void Soprano::Client::SparqlModel::setUser( const QString& userName, const QString& password )
{
    d->client->setUser( userName, password );
}


// QString Soprano::Client::SparqlModel::host() const
// {
//     return
// }


// quint16 Soprano::Client::SparqlModel::port() const
// {
// }


// QString Soprano::Client::SparqlModel::userName() const
// {
// }


// QString Soprano::Client::SparqlModel::password() const
// {
// }


Soprano::Error::ErrorCode Soprano::Client::SparqlModel::addStatement( const Statement& statement )
{
    if( !statement.isValid() ) {
        setError( "Cannot add invalid statement.", Error::ErrorInvalidArgument );
        return Error::ErrorInvalidArgument;
    }

    // FIXME: what if the context is not defined?
    if ( !statement.context().isValid() ) {
        setError( "No support for the default context.", Error::ErrorInvalidArgument );
        return Error::ErrorInvalidArgument;
    }

    // FIXME: This looks like SPARUL. How can we know if its supported?
    QString insert = QString("INSERT INTO %1").arg( statementToConstructGraphPattern( statement, true ) );
    d->client->query( insert );

    setError( d->client->lastError() );

    return Error::convertErrorCode( d->client->lastError().code() );
}


Soprano::NodeIterator Soprano::Client::SparqlModel::listContexts() const
{
    QueryResultIterator r = executeQuery("select distinct ?g where { graph ?g {?s ?p ?o}}", Query::QueryLanguageSparql, QString());
    return r.iterateBindings("g");
}


bool Soprano::Client::SparqlModel::containsStatement( const Statement& statement ) const
{
    // FIXME: fail on invalid statements and somehow handle the default context
    QString query = QString( "ask { %1 }" ).arg( statementToConstructGraphPattern( statement, true ) );
    return executeQuery( query, Query::QueryLanguageSparql ).boolValue();
}


bool Soprano::Client::SparqlModel::containsAnyStatement( const Statement& statement ) const
{
    QString query = QString( "ask { %1 }" ).arg( statementToConstructGraphPattern( statement, true ) );
    return executeQuery( query, Query::QueryLanguageSparql ).boolValue();
}


namespace {
    class StatementIteratorQueryResultBackend : public Soprano::QueryResultIteratorBackend
    {
    public:
        StatementIteratorQueryResultBackend( Soprano::StatementIterator it )
            : QueryResultIteratorBackend(),
              m_iterator( it ) {
        }

        bool next() {
            return m_iterator.next();
        }

        Soprano::BindingSet current() const {
            return Soprano::BindingSet();
        }

        Soprano::Statement currentStatement() const {
            return m_iterator.current();
        }

        Soprano::Node binding( const QString& ) const {
            return Soprano::Node();
        }

        Soprano::Node binding( int ) const {
            return Soprano::Node();
        }

        int bindingCount() const {
            return 0;
        }

        QStringList bindingNames() const {
            return QStringList();
        }

        bool isGraph() const {
            return true;
        }

        bool isBinding() const {
            return false;
        }

        bool isBool() const {
            return false;
        }

        bool boolValue() const {
            return false;
        }

        void close() {
            m_iterator.close();
        }

    private:
        Soprano::StatementIterator m_iterator;
    };
}


Soprano::QueryResultIterator Soprano::Client::SparqlModel::executeQuery( const QString& query, Query::QueryLanguage language, const QString& userQueryLanguage ) const
{
    if ( language != Query::QueryLanguageSparql ) {
        setError( "Unsupported query language: " + Query::queryLanguageToString( language, userQueryLanguage ), Error::ErrorInvalidArgument );
    }
    else {
        QByteArray response = d->client->query( query );
        setError( d->client->lastError() );

        if( !lastError() && !response.isEmpty() ) {

            // try parsing the response
            bool ok = false;
            SparqlParser::Sparql queryResult = SparqlParser::Sparql::parseByteArray( &response, &ok );

            // seems to be a binding or boolean result
            if ( ok ) {
                return new Soprano::Client::SparqlQueryResult( queryResult );
            }

            // try parsing it as graph
            else if ( const Soprano::Parser* parser = PluginManager::instance()->discoverParserForSerialization( SerializationRdfXml ) ) {
                StatementIterator it = parser->parseString( QString( response ), QUrl(), SerializationRdfXml );
                if ( it.isValid() ) {
                    // graph result
                    return new StatementIteratorQueryResultBackend( it );
                }
            }

            // neither sparql xml result nor graph
            else {
                setError( "Failed to parse query result." );
                return QueryResultIterator();
            }
        }
    }

    // client returned error
    return QueryResultIterator();
}


namespace {
    class QueryResultStatementIteratorBackend : public Soprano::IteratorBackend<Soprano::Statement>
    {
    public:
        QueryResultStatementIteratorBackend( const Soprano::QueryResultIterator& r, const Soprano::Statement& s )
            : m_result( r ),
              m_statement(s) {
        }

        ~QueryResultStatementIteratorBackend() {
        }

        bool next() {
            return m_result.next();
        }

        Soprano::Statement current() const {
            Soprano::Statement s( m_statement );

            if( !s.context().isValid() ) {
                s.setContext( m_result.binding("g") );
            }

            if( !s.subject().isValid()) {
                s.setSubject( m_result.binding("s") );
            }

            if( !s.predicate().isValid()) {
                s.setPredicate( m_result.binding("p") );
            }
            if( !s.object().isValid()) {
                s.setObject( m_result.binding("o") );
            }

            return s;
        }

        void close() {
            m_result.close();
        }

        Soprano::Error::Error lastError() const {
            return m_result.lastError();
        }

    private:
        Soprano::QueryResultIterator m_result;
        Soprano::Statement m_statement;
    };
}


Soprano::StatementIterator Soprano::Client::SparqlModel::listStatements( const Statement& partial ) const
{
    QString query = QString( "select * where { %1 }" ).arg( statementToConstructGraphPattern( partial, true ) );
    qDebug() << "List Statements Query" << query;
    QueryResultIterator r = executeQuery( query, Query::QueryLanguageSparql );
    return Soprano::StatementIterator( new QueryResultStatementIteratorBackend( r, partial ) );
}


Soprano::Error::ErrorCode Soprano::Client::SparqlModel::removeStatement( const Statement& statement )
{
    if ( !statement.context().isValid() ) {
        setError( "No support for the default context.", Error::ErrorInvalidArgument );
        return Error::ErrorInvalidArgument;
    }

    // FIXME: SPARUL?
    QString query = QString( "delete from %2" ).arg( statementToConstructGraphPattern( statement, true ) );
    d->client->query( query );
    setError( d->client->lastError() );
    return Error::convertErrorCode( d->client->lastError().code() );
}


Soprano::Error::ErrorCode Soprano::Client::SparqlModel::removeAllStatements( const Statement& statement )
{
    setError( "No removeAllStatements support.", Error::ErrorNotSupported );
    return Error::ErrorNotSupported;
}


int Soprano::Client::SparqlModel::statementCount() const
{
    setError( "No statement count supported.", Error::ErrorNotSupported );
    return -1;
}


bool Soprano::Client::SparqlModel::isEmpty() const
{
    setError( "No isEmpty support.", Error::ErrorNotSupported );
    return false;
}


Soprano::Node Soprano::Client::SparqlModel::createBlankNode()
{
    setError( "Creating blank nodes not supported.", Error::ErrorNotSupported );
    return Node();
}

#include "sparqlmodel.moc"