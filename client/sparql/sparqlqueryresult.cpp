/*
 * This file is part of Soprano Project.
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

#include "sparqlqueryresult.h"

#include "statement.h"
#include "pluginmanager.h"
#include "statementiterator.h"

#include <QtCore/QDebug>


namespace {
    Soprano::Node literalToNode( Soprano::Client::SparqlParser::Literal l ) {
        if ( l.datatype().isEmpty() )
            return Soprano::LiteralValue::createPlainLiteral( l.data(), l.xml_lang() );
        else
            return Soprano::LiteralValue::fromString( l.data(), l.datatype() );
    }

    Soprano::Node bindingToNode( const Soprano::Client::SparqlParser::Binding& binding ) {
        Soprano::Client::SparqlParser::Binding::BindingType type = binding.type();
        if( type == Soprano::Client::SparqlParser::Binding::URI ) {
            return Soprano::Node::createResourceNode(binding.uri().uri());
        }
        else if ( type == Soprano::Client::SparqlParser::Binding::BNODE ) {
            return Soprano::Node::createBlankNode(binding.bnode().id());
        }
        else {
            return literalToNode( binding.literal() );
        }
    }
 }


Soprano::Client::SparqlQueryResult::SparqlQueryResult( const SparqlParser::Sparql& result )
    : Soprano::QueryResultIteratorBackend(),
      m_result( result ),
      m_currentResultIndex( -1 )
{
    // cache binding names
    foreach( const SparqlParser::Variable& v, m_result.head().variableList() ) {
        m_bindingNames << v.name();
    }
}


Soprano::Client::SparqlQueryResult::~SparqlQueryResult()
{
}


bool Soprano::Client::SparqlQueryResult::next()
{
    if( isBinding()) {
        ++m_currentResultIndex;
        return( m_currentResultIndex < m_result.results().resultList().count() );
    }
    else {
        // boolean result always needs to return true
        return true;
    }
}


Soprano::Statement Soprano::Client::SparqlQueryResult::currentStatement() const
{
    // we do not handle graphs
    return Statement();
}


Soprano::Node Soprano::Client::SparqlQueryResult::binding( const QString& name ) const
{
    if ( m_currentResultIndex != -1 && m_currentResultIndex < m_result.results().resultList().count() ) {
        foreach ( const SparqlParser::Binding& b, m_result.results().resultList()[m_currentResultIndex].bindingList() ) {
            if( b.name() == name ) {
                return bindingToNode( b );
            }
        }
        // non set optional bindings would not be found, too, although it would not be an error
        if ( !m_bindingNames.contains( name ) ) {
            setError( QString::fromLatin1( "Invalid binding name: %1" ).arg( name ), Error::ErrorInvalidArgument );
        }
    }
    else {
        setError( QString::fromLatin1( "Invalid iterator." ) );
    }

    return Soprano::Node();
}


Soprano::Node Soprano::Client::SparqlQueryResult::binding( int offset ) const
{
    if( offset >= 0 && offset < bindingCount() ) {
        // optional bindings are simply left out when not set. Thus, we cannot access the binding list by offset
        return binding( m_bindingNames[offset] );
    }
    else {
        setError( QString::fromLatin1( "Invalid iterator." ) );
    }

    return Soprano::Node();
}


int Soprano::Client::SparqlQueryResult::bindingCount() const
{
    return m_bindingNames.count();
}


QStringList Soprano::Client::SparqlQueryResult::bindingNames() const
{
    return m_bindingNames;
}


bool Soprano::Client::SparqlQueryResult::isGraph() const
{
    return false;
}


bool Soprano::Client::SparqlQueryResult::isBinding() const
{
    return !isBool();
}


bool Soprano::Client::SparqlQueryResult::isBool() const
{
    return m_result.boolean().isValid();
}


bool Soprano::Client::SparqlQueryResult::boolValue() const
{
    return m_result.boolean().value();
}


void Soprano::Client::SparqlQueryResult::close()
{
    // do nothing
}
