/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2006 Daniele Galdi <daniele.galdi@gmail.com>
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

#include "inferencemodel.h"
#include "rdf.h"
#include "sil.h"
#include "statement.h"
#include "inferencerule.h"
#include "query.h"
#include "statementpattern.h"
#include "nodepattern.h"
#include "queryresultiterator.h"

#include <QtCore/QString>
#include <QtCore/QUuid>


// FIXME: add error handling!


class Soprano::Inference::InferenceModel::Private
{
public:
    QList<Rule> rules;
};


Soprano::Inference::InferenceModel::InferenceModel( Model* parent )
    : FilterModel( parent ),
      d( new Private() )
{
}


Soprano::Inference::InferenceModel::~InferenceModel()
{
    delete d;
}


void Soprano::Inference::InferenceModel::addRule( const Rule& rule )
{
    d->rules.append( rule );
}


void Soprano::Inference::InferenceModel::setRules( const QList<Rule>& rules )
{
    d->rules = rules;
}


Soprano::ErrorCode Soprano::Inference::InferenceModel::addStatement( const Statement& statement )
{
    parentModel()->addStatement( statement );
    if( inferStatement( statement, true ) ) {
        emit statementsAdded();
    }
}

// this is stuff we only need for the temp implementation that is used due to the lack of proper SPARQL support in redland
// -----------------------------------------------------------------------------------------------------------------------
#include <QDebug>
#include <QSet>
#include "statementiterator.h"
uint qHash( const Soprano::Node& node )
{
    return qHash( node.toString() );
}
// -----------------------------------------------------------------------------------------------------------------------

Soprano::ErrorCode Soprano::Inference::InferenceModel::removeStatements( const Statement& statement )
{
    // FIXME: should we check if the statement could match some rule at all and if not do nothing?

    // are there any rules that handle objects? Probably not.
    if ( !statement.object().isLiteral() ) {

        // we need to list statements first and then iterate over all that will be removed
        // since we change the model we also have to cache the statements
        QList<Statement> removedStatements = parentModel()->listStatements( statement ).allStatements();
        for ( QList<Statement>::const_iterator it2 = removedStatements.constBegin();
              it2 != removedStatements.constEnd(); ++it2 ) {
            removeStatement( *it2 );
        }
    }

    return FilterModel::removeStatements( statement );
}


void Soprano::Inference::InferenceModel::removeStatement( const Statement& statement )
{
    QList<Node> graphs = inferedGraphsForStatement( statement );
    for ( QList<Node>::const_iterator it = graphs.constBegin(); it != graphs.constEnd(); ++it ) {
        Node graph = *it;

        // Step 1 remove the infered metadata (and trigger recursive removal) - can be slow
        removeContext( graph );

        // Step 2: remove the source statements of the removed graph (redland probem again: iteraotors are invalidated by remove methods)
        QList<Node> graphSources = parentModel()->listStatements( Statement( graph,
                                                                             Vocabulary::SIL::SOURCE_STATEMENT(),
                                                                             Node(),
                                                                             Vocabulary::SIL::INFERENCE_METADATA() ) ).allObjects();
        for( QList<Node>::const_iterator it = graphSources.constBegin();
             it != graphSources.constEnd(); ++it ) {
            parentModel()->removeStatements( Statement( *it, Node(), Node(), Vocabulary::SIL::INFERENCE_METADATA() ) );
        }

        // Step 3: remove the graph metadata
        parentModel()->removeStatements( Statement( graph, Node(), Node(), Vocabulary::SIL::INFERENCE_METADATA() ) );
    }
}


QList<Soprano::Node> Soprano::Inference::InferenceModel::inferedGraphsForStatement( const Statement& statement ) const
{
    QList<Soprano::Node> graphs;

    // sadly redland does not seem to support even the most simple queries :(
#if 0
    // check if our statement is source statement for any infered graph
    QString query = QString( "PREFIX rdf: <%1> "
                             "SELECT ?s WHERE { "
                             "?s rdf:type rdf:Statement . "
                             "?s rdf:subject <%2> . "
                             "?s rdf:predicate <%3> . "
                             "?s rdf:object <%4> ." )
                    .arg( Vocabulary::RDF::NAMESPACE().toString() )
                    .arg( statement.subject().toString() )
                    .arg( statement.predicate().toString() )
                    .arg( statement.object().toString() );

    if ( statement.context().isValid() ) {
        query += QString( "?s <%1> <%2> ." )
                 .arg( Vocabulary::SIL::CONTEXT().toString() )
                 .arg( statement.context().toString() );
    }

    query += " }";

    QueryResultIterator it = parentModel()->executeQuery( Query( query, Query::SPARQL ) );
    while ( it.next() ) {
        // Step 2: Check for which graph it is source statement
        query = QString( "SELECT ?g WHERE { "
                         "GRAPH <%1> { "
                         "?g <%2> <%3> . "
                         "?g <%4> <%5> . } }" )
                .arg( Vocabulary::SIL::INFERENCE_METADATA().toString() )
                .arg( Vocabulary::SIL::SOURCE_STATEMENT().toString() )
                .arg( it.binding( 0 ).toString() )
                .arg( Vocabulary::RDF::TYPE().toString() )
                .arg( Vocabulary::SIL::INFERENCE_GRAPH().toString() );

        QueryResultIterator it2 = parentModel()->executeQuery( Query( query, Query::SPARQL ) );
        while ( it2.next() ) {
            // Step 3: remove the whole infered graph and its metadata
            graphs += it2.binding( 0 );
        }
    }
#endif

    // since redland cannot handle our query we have to do it the ugly way
    QSet<Node> sourceStatements;
    StatementIterator it = parentModel()->listStatements( Statement( Node(), Vocabulary::RDF::SUBJECT(), statement.subject() ) );
    sourceStatements = it.allSubjects().toSet();
    it = parentModel()->listStatements( Statement( Node(), Vocabulary::RDF::PREDICATE(), statement.predicate() ) );
    sourceStatements &= it.allSubjects().toSet();
    it = parentModel()->listStatements( Statement( Node(), Vocabulary::RDF::OBJECT(), statement.object() ) );
    sourceStatements &= it.allSubjects().toSet();

    // now sourceStatements should contain what our nice first query above returns
    // and we use a siplyfied version of the query above to redland won't get confused :(
    Q_FOREACH( Node node, sourceStatements ) {
        // Step 2: Check for which graph it is source statement
        QString query = QString( "SELECT ?g WHERE { "
                                 "?g <%1> <%2> . }" )
                        .arg( Vocabulary::SIL::SOURCE_STATEMENT().toString() )
                        .arg( node.toString() );

        QueryResultIterator it2 = parentModel()->executeQuery( Query( query, Query::SPARQL ) );
        while ( it2.next() ) {
            // Step 3: remove the whole infered graph and its metadata
            graphs += it2.binding( 0 );
        }
    }

    return graphs;
}


void Soprano::Inference::InferenceModel::performInference()
{
    for ( QList<Rule>::const_iterator it = d->rules.constBegin();
          it != d->rules.constEnd(); ++it ) {
        inferRule( *it, true );
    }
}


void Soprano::Inference::InferenceModel::clearInference()
{
    // remove all infered statements
    Query query( QString( "select ?g where { ?g <%1> <%2> . }" )
                 .arg( Vocabulary::RDF::TYPE().toString() )
                 .arg( Vocabulary::SIL::INFERENCE_GRAPH().toString() ),
                 Query::SPARQL );

    QueryResultIterator it = parentModel()->executeQuery( query );
    while ( it.next() ) {
        parentModel()->removeContext( it.binding( 0 ) );
    }

    // remove infered graph metadata
    parentModel()->removeContext( Vocabulary::SIL::INFERENCE_METADATA() );
}


// FIXME: maybe move these methods to Rule?
static Soprano::Node bindPattern( const Soprano::QueryResultIterator& it, const Soprano::Inference::NodePattern& pattern )
{
    if ( pattern.isVariable() ) {
        return it.binding( pattern.variableName() );
    }
    else {
        return pattern.resource();
    }
}


static Soprano::Statement bindPattern( const Soprano::QueryResultIterator& it, const Soprano::Inference::StatementPattern& pattern )
{
    return Soprano::Statement( bindPattern( it, pattern.subjectPattern() ),
                               bindPattern( it, pattern.predicatePattern() ),
                               bindPattern( it, pattern.objectPattern() ) );
}


static QList<Soprano::Statement> bindPatterns( const Soprano::QueryResultIterator& it, const QList<Soprano::Inference::StatementPattern>& patterns )
{
    QList<Soprano::Statement> sl;
    Q_FOREACH( Soprano::Inference::StatementPattern sp, patterns ) {
        sl.append( bindPattern( it, sp ) );
    }
    return sl;
}


int Soprano::Inference::InferenceModel::inferStatement( const Statement& statement, bool recurse )
{
    for ( QList<Rule>::const_iterator it = d->rules.constBegin();
          it != d->rules.constEnd(); ++it ) {
        const Rule& rule = *it;
        if( rule.match( statement) ) {
            inferRule( rule, recurse );
        }
    }
}


static QUrl createRandomUri()
{
    // FIXME: check if the uri already exists
    QString uid = QUuid::createUuid().toString();
    uid = uid.mid( 1, uid.length()-2 );
    return QUrl( "inference://localhost#" + uid );
}


int Soprano::Inference::InferenceModel::inferRule( const Rule& rule, bool recurse )
{
    Query q( rule.createSparqlQuery(), Query::SPARQL );

    int inferedStatementsCount = 0;
    QueryResultIterator it = parentModel()->executeQuery( q );
    while ( it.next() ) {
        Statement inferedStatement = bindPattern( it, rule.effect() );

        // we only add infered statements if they are not already present (in any named graph, aka. context)
        if ( !parentModel()->containsStatements( inferedStatement ) ) {
            ++inferedStatementsCount;

            QUrl inferenceGraphUrl = createRandomUri();

            // write the actual infered statement
            inferedStatement.setContext( inferenceGraphUrl );
            parentModel()->addStatement( inferedStatement );

            // write the metadata about the new inference graph into the inference metadata graph
            // type of the new graph is sil:InferenceGraph
            parentModel()->addStatement( Statement( inferenceGraphUrl,
                                                    Vocabulary::RDF::TYPE(),
                                                    Vocabulary::SIL::INFERENCE_GRAPH(),
                                                    Vocabulary::SIL::INFERENCE_METADATA() ) );

            // add sourceStatements
            // FIXME: try to reuse source statements
            QList<Statement> sourceStatements = bindPatterns( it, rule.preconditions() );
            for ( QList<Statement>::const_iterator it = sourceStatements.constBegin();
                  it != sourceStatements.constEnd(); ++it ) {
                const Statement& sourceStatement = *it;

                QUrl sourceStatementUri = createRandomUri();
                parentModel()->addStatement( Statement( sourceStatementUri,
                                                        Vocabulary::RDF::TYPE(),
                                                        Vocabulary::RDF::STATEMENT(),
                                                        Vocabulary::SIL::INFERENCE_METADATA() ) );

                parentModel()->addStatement( Statement( sourceStatementUri,
                                                        Vocabulary::RDF::SUBJECT(),
                                                        sourceStatement.subject(),
                                                        Vocabulary::SIL::INFERENCE_METADATA() ) );
                parentModel()->addStatement( Statement( sourceStatementUri,
                                                        Vocabulary::RDF::PREDICATE(),
                                                        sourceStatement.predicate(),
                                                        Vocabulary::SIL::INFERENCE_METADATA() ) );
                parentModel()->addStatement( Statement( sourceStatementUri,
                                                        Vocabulary::RDF::OBJECT(),
                                                        sourceStatement.object(),
                                                        Vocabulary::SIL::INFERENCE_METADATA() ) );
                if ( sourceStatement.context().isValid() ) {
                    parentModel()->addStatement( Statement( sourceStatementUri,
                                                            Vocabulary::SIL::CONTEXT(),
                                                            sourceStatement.context(),
                                                            Vocabulary::SIL::INFERENCE_METADATA() ) );
                }

                // remember the source statement as a source for our graph
                parentModel()->addStatement( Statement( inferenceGraphUrl,
                                                        Vocabulary::SIL::SOURCE_STATEMENT(),
                                                        sourceStatementUri,
                                                        Vocabulary::SIL::INFERENCE_METADATA() ) );
            }

            if ( recurse ) {
                inferedStatementsCount += inferStatement( inferedStatement, true );
            }
        }
    }

    return inferedStatementsCount;
}

#include "inferencemodel.moc"
