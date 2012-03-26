/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007-2011 Sebastian Trueg <trueg@kde.org>
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

#define USING_SOPRANO_NRLMODEL_UNSTABLE_API
#include "nrlmodel.h"
#undef USING_SOPRANO_NRLMODEL_UNSTABLE_API
#include "error.h"
#include "vocabulary/nrl.h"
#include "vocabulary/nao.h"
#include "vocabulary/rdf.h"
#include "vocabulary/rdfs.h"
#include "vocabulary/xsd.h"
#include "queryresultiterator.h"
#include "statementiterator.h"
#include "nodeiterator.h"
#include "node.h"
#include "statement.h"

#include <QtCore/QUuid>
#include <QtCore/QDateTime>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtCore/QDebug>


namespace {
    // TODO: make the "nepomuk" part configurable, maybe using setCreatorName or something
    QUrl createGraphUri() {
        return QUrl( "urn:nepomuk:local:" + QUuid::createUuid().toString().remove( QRegExp( "[\\{\\}]" ) ) );
    }
}


class Soprano::NRLModel::Private
{
public:
    Private()
        : ignoreContext( true ),
          m_expandQueryPrefixes( false ),
          m_prefixMapMutex( QMutex::Recursive ) {
    }

    /**
     * Get all prefixes stored in the model
     */
    void buildPrefixMap()
    {
        QMutexLocker lock( &m_prefixMapMutex );

        m_prefixes.clear();

        // fixed prefixes
        m_prefixes.insert( "rdf", Soprano::Vocabulary::RDF::rdfNamespace() );
        m_prefixes.insert( "rdfs", Soprano::Vocabulary::RDFS::rdfsNamespace() );
        m_prefixes.insert( "xsd", Soprano::Vocabulary::XMLSchema::xsdNamespace() );
        m_prefixes.insert( "nrl", Soprano::Vocabulary::NRL::nrlNamespace() );
        m_prefixes.insert( "nao", Soprano::Vocabulary::NAO::naoNamespace() );

        // get prefixes from nepomuk
        Soprano::QueryResultIterator it =
            q->executeQuery( QString( "select ?ns ?ab where { "
                                      "?g %1 ?ns . "
                                      "?g %2 ?ab . }" )
                             .arg( Soprano::Node::resourceToN3( Soprano::Vocabulary::NAO::hasDefaultNamespace() ) )
                             .arg( Soprano::Node::resourceToN3( Soprano::Vocabulary::NAO::hasDefaultNamespaceAbbreviation() ) ),
                             Soprano::Query::QueryLanguageSparql );
        while ( it.next() ) {
            QString ab = it["ab"].toString();
            QUrl ns = it["ns"].toString();
            if ( !m_prefixes.contains( ab ) ) {
                m_prefixes.insert( ab, ns );
            }
        }
    }

    bool ignoreContext;
    bool m_expandQueryPrefixes;

    // cache of all prefixes that are supported
    QHash<QString, QUrl> m_prefixes;

    NRLModel* q;

    QMutex m_prefixMapMutex;
};

Soprano::NRLModel::NRLModel()
    : FilterModel(),
      d( new Private() )
{
    d->q = this;
}


Soprano::NRLModel::NRLModel( Model* parent )
    : FilterModel( parent ),
      d( new Private() )
{
    d->q = this;
}


Soprano::NRLModel::~NRLModel()
{
    delete d;
}


void Soprano::NRLModel::setIgnoreContext( bool b )
{
    d->ignoreContext = b;
}


bool Soprano::NRLModel::ignoreContext() const
{
    return d->ignoreContext;
}


void Soprano::NRLModel::setEnableQueryPrefixExpansion( bool enable )
{
    if ( enable != d->m_expandQueryPrefixes ) {
        d->m_expandQueryPrefixes = enable;
        if ( enable )
            d->buildPrefixMap();
        else
            d->m_prefixes.clear();
    }
}


bool Soprano::NRLModel::queryPrefixExpansionEnabled() const
{
    return d->m_expandQueryPrefixes;
}


QHash<QString, QUrl> Soprano::NRLModel::queryPrefixes() const
{
    return d->m_prefixes;
}


Soprano::Error::ErrorCode Soprano::NRLModel::addNrlStatement( const Statement& statement )
{
    // 1. check if any cardinality restrictions are defined for s.predicate()
    // 2. if so -> enforce
    // 3. if not -> check if some for superproperties are defined (optional advanced feature)

    QString query = QString( "select ?min ?max ?c where { { <%1> <%2> ?min } UNION { <%1> <%3> ?max } UNION { <%1> <%4> ?c } }" )
                    .arg( statement.predicate().toString() )
                    .arg( Vocabulary::NRL::minCardinality().toString() )
                    .arg( Vocabulary::NRL::maxCardinality().toString() )
                    .arg( Vocabulary::NRL::cardinality().toString() );

    QueryResultIterator it = FilterModel::executeQuery( query, Query::QueryLanguageSparql );
    if ( !it.isValid() ) {
        setError( QString( "Query failed: '%1'" ).arg( query ) );
        return Error::ErrorUnknown;
    }

    int min = -1;
    int max = -1;
    int c = -1;

    while ( it.next() ) {
        if ( it.binding( "min" ).isLiteral() ) {
            min = it.binding( "min" ).literal().toInt();
        }
        else if ( it.binding( "max" ).isLiteral() ) {
            max = it.binding( "max" ).literal().toInt();
        }
        if ( it.binding( "c" ).isLiteral() ) {
            c = it.binding( "c" ).literal().toInt();
        }
    }

    if ( min >= 0 || max >= 0 || c >= 0 ) {
        qDebug() << "Predicate " << statement.predicate() << " has cardinalities: " << min << "; " << max << "; " << c;

        // the simple case (and also the most frequently used I suppose)
        if ( c == 1 || max == 1 ) {
            if ( ignoreContext() ) {
                FilterModel::removeAllStatements( Statement( statement.subject(), statement.predicate(), Node() ) );
            }
            else {
                FilterModel::removeAllStatements( Statement( statement.subject(), statement.predicate(), Node(), statement.context() ) );
            }
            return FilterModel::addStatement( statement );
        }

        // the general case
        else if ( max > 1 || c > 1 ) {
            if ( c > 1 )
                max = c;

            StatementIterator sit;
            if ( ignoreContext() ) {
                sit = FilterModel::listStatements( Statement( statement.subject(), statement.predicate(), Node() ) );
            }
            else {
                sit = FilterModel::listStatements( Statement( statement.subject(), statement.predicate(), statement.context() ) );
            }

            QList<Statement> matchingStatements;
            while ( sit.next() ) {
                Statement s = *sit;
                if ( ignoreContext() || s.context() == statement.context() ) {
                    matchingStatements.append( s );
                }
            }

            if ( matchingStatements.count() >= max ) {
                qDebug() << "Found " << matchingStatements.count() << " statements that define " << statement.predicate() << " for " << statement.subject() << endl
                         << "  -> need to remove " << ( matchingStatements.count()-max+1 ) << " before adding the new statement.";
                if ( matchingStatements.count() == 1 ) {
                    qDebug() << "Removing one statement is easy...";
                    FilterModel::removeStatement( matchingStatements[0] );
                }
                else {
                    qDebug() << "FIXME: which statements to remove? Random? Best would be to remove the oldest...";
                    setError( "Max cardinality for predicate reached" );
                    return Error::ErrorUnknown;
                }
            }

            return FilterModel::addStatement( statement );
        }
        else {
            qDebug()  << "Predicate " << statement.predicate() << " has unusable cardinality restrictions";
            return FilterModel::addStatement( statement );
        }
    }
    else {
        qDebug() << "Predicate " << statement.predicate() << " has no cardinality restrictions";
        return FilterModel::addStatement( statement );
    }

    // make gcc shut up
    return Error::ErrorNone;
}


QUrl Soprano::NRLModel::createGraph( const QUrl& type, QUrl* metadataGraph )
{
    QUrl graph = createGraphUri();
    QUrl metadatagraph = createGraphUri();

    FilterModel::addStatement( metadatagraph, Soprano::Vocabulary::NRL::coreGraphMetadataFor(), graph, metadatagraph );
    FilterModel::addStatement( metadatagraph, Soprano::Vocabulary::RDF::type(), Soprano::Vocabulary::NRL::GraphMetadata(), metadatagraph );
    FilterModel::addStatement( graph, Soprano::Vocabulary::RDF::type(), type, metadatagraph );
    FilterModel::addStatement( graph, Soprano::Vocabulary::NAO::created(), Soprano::LiteralValue( QDateTime::currentDateTime() ), metadatagraph );

    if ( metadataGraph )
        *metadataGraph = metadatagraph;

    return graph;
}


Soprano::Error::ErrorCode Soprano::NRLModel::removeGraph( const QUrl& graph )
{
    QList<Node> metadataGraphs = FilterModel::executeQuery( QString("select ?mg where { ?mg %1 %2 . }")
                                                            .arg(Node::resourceToN3(Soprano::Vocabulary::NRL::coreGraphMetadataFor()) )
                                                            .arg(Node::resourceToN3(graph)),
                                                            Query::QueryLanguageSparql ).iterateBindings(0).allNodes();
    foreach ( const Node& mg, metadataGraphs ) {
        // we can only use removeAllStatements(Statement) here since all other variants will come back to
        // our version below which in turn calls us again
        FilterModel::removeAllStatements( Statement( Node(), Node(), Node(), mg ) );
    }
    // this is where we do not want to be called recursively
    return FilterModel::removeAllStatements( Statement( Node(), Node(), Node(), graph ) );
}


Soprano::QueryResultIterator Soprano::NRLModel::executeQuery( const QString& query, Query::QueryLanguage language, const QString& userQueryLanguage ) const
{
    QString expandedQuery( query );

    if ( language == Query::QueryLanguageSparql &&
         d->m_expandQueryPrefixes ) {
        d->m_prefixMapMutex.lock();
        QHash<QString, QUrl> prefixes = d->m_prefixes;
        d->m_prefixMapMutex.unlock();

        // find position in the query to add the prefixes to: directly before the actual query start
        // certain backends like the virtuoso one support SPARQL extensions which need to be before the
        // prefixes
        const int pos = expandedQuery.indexOf(QRegExp(QLatin1String("select|describe|construct|ask"), Qt::CaseInsensitive));
        if(pos >= 0) {
            for ( QHash<QString, QUrl>::const_iterator it = prefixes.constBegin();
                  it != prefixes.constEnd(); ++it ) {
                QString prefix = it.key();
                QUrl ns = it.value();

                // very stupid check for the prefix usage
                if ( expandedQuery.contains( prefix + ':' ) ) {
                    // if the prefix is not defined add it
                    if ( !expandedQuery.contains( QRegExp( QString::fromLatin1( "[pP][rR][eE][fF][iI][xX]\\s*%1\\s*:\\s*<%2>" )
                                                           .arg( prefix )
                                                           .arg( QRegExp::escape( ns.toString() ) ) ) ) ) {
                        expandedQuery.insert( pos, QString( "prefix %1: <%2> " ).arg( prefix ).arg( ns.toString() ) );
                    }
                }
            }
        }
    }

    return FilterModel::executeQuery( expandedQuery, language, userQueryLanguage );
}


Soprano::Error::ErrorCode Soprano::NRLModel::removeAllStatements( const Statement& statement )
{
    if( statement.context().isValid() &&
        !statement.subject().isValid() &&
        !statement.predicate().isValid() &&
        !statement.object().isValid() ) {
        return removeGraph( statement.context().uri() );
    }
    else {
        return FilterModel::removeAllStatements( statement );
    }
}

#include "nrlmodel.moc"
