/*
 * This file is part of Soprano Project
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

#include "raptorparserstatementiteratorbackend.h"
#include "raptorparserwrapper.h"
#include "raptor-config.h"

#include <QtCore/QDebug>


namespace {
    Soprano::Node convertNode( const void* data, raptor_identifier_type type,
                               raptor_uri* objectLiteralDatatype = 0, const unsigned char* objectLiteralLanguage = 0 )
    {
        switch( type ) {
        case RAPTOR_IDENTIFIER_TYPE_RESOURCE:
        case RAPTOR_IDENTIFIER_TYPE_PREDICATE:
        case RAPTOR_IDENTIFIER_TYPE_ORDINAL:
            return Soprano::Node::createResourceNode( QString::fromUtf8( ( char* )raptor_uri_as_string( ( raptor_uri* )data ) ) ); // FIXME: are raptor uri strings always encoded? Should we use QUrl::fromEncoded?

        case RAPTOR_IDENTIFIER_TYPE_ANONYMOUS:
            return Soprano::Node::createBlankNode( QString::fromUtf8( ( const char* )data ) );

        case RAPTOR_IDENTIFIER_TYPE_LITERAL:
        case RAPTOR_IDENTIFIER_TYPE_XML_LITERAL:
            return Soprano::Node::createLiteralNode( Soprano::LiteralValue::fromString( QString::fromUtf8( ( const char* )data ),
                                                                                        objectLiteralLanguage
                                                                                        ? QUrl( QString::fromUtf8( ( char* )raptor_uri_as_string( objectLiteralDatatype ) ) )
                                                                                        : QUrl() ),
                                                     QString::fromUtf8( ( const char* )objectLiteralLanguage ) );

        default:
            return Soprano::Node();
        }
    }


    Soprano::Statement convertTriple( const raptor_statement* triple )
    {
        return Soprano::Statement( convertNode( triple->subject, triple->subject_type ),
                                   convertNode( triple->predicate, triple->predicate_type ),
                                   convertNode( triple->object, triple->object_type,
                                                triple->object_literal_datatype,
                                                triple->object_literal_language ) );
    }
}


Soprano::Raptor::ParserStatementIteratorBackend::ParserStatementIteratorBackend( QTextStream* s, bool deleteStream, ParserWrapper* parser )
    : IteratorBackend<Statement>(),
      m_parser( parser ),
      m_stream( s ),
      m_ioDevice( 0 ),
      m_deleteSource( deleteStream )
{
    initStatementHandler();
}


Soprano::Raptor::ParserStatementIteratorBackend::ParserStatementIteratorBackend( QIODevice* d, bool deleteDevice, ParserWrapper* parser )
    : IteratorBackend<Statement>(),
      m_parser( parser ),
      m_stream( 0 ),
      m_ioDevice( d ),
      m_deleteSource( deleteDevice )
{
    initStatementHandler();
}


Soprano::Raptor::ParserStatementIteratorBackend::ParserStatementIteratorBackend( const QString& s, ParserWrapper* parser )
    : IteratorBackend<Statement>(),
      m_parser( parser ),
      m_stream( 0 ),
      m_ioDevice( 0 ),
      m_dataString( s )
{
    m_stream = new QTextStream( &m_dataString );
    m_deleteSource = true;
    initStatementHandler();
}


Soprano::Raptor::ParserStatementIteratorBackend::~ParserStatementIteratorBackend()
{
    close();
}


void Soprano::Raptor::ParserStatementIteratorBackend::initStatementHandler()
{
    raptor_set_statement_handler( m_parser->parser(), this, raptorTriplesHandler );
#ifdef RAPTOR_HAVE_TRIG
    raptor_set_graph_handler( m_parser->parser(), this, raptorGraphHandler );
#endif
}


bool Soprano::Raptor::ParserStatementIteratorBackend::next()
{
    // advance our parsed statement queue
    if( !m_parsedStatementsQueue.isEmpty() )
        m_parsedStatementsQueue.dequeue();

    static const int bufSize = 1024;

    // if possible let raptor do the decoding (using QTextStream in the Parser API was a bad design decsision,
    // sadly this cannot be changed before 3.0 (KDE 5!) to keep BC)
    if( m_parsedStatementsQueue.isEmpty() && m_parser ) {
        QIODevice* dev = m_ioDevice ? m_ioDevice : m_stream->device();
        if ( dev ) {
            QByteArray buf( bufSize, 0 );
            while ( !dev->atEnd() && m_parsedStatementsQueue.isEmpty() ) {
                qint64 r = dev->read( buf.data(), buf.size() );
                if ( r <= 0 ||
                     raptor_parse_chunk( m_parser->parser(), ( const unsigned char* )buf.data(), r, 0 ) ) {
                    setError( m_parser->lastError() );
                    close();
                    return false;
                }
            }

            // Once we read all data, we need to finalize parsing.
            if( dev->atEnd() ) {
                raptor_parse_chunk( m_parser->parser(), 0, 0, 1 );
                close();
            }
        }
        else {
            while ( !m_stream->atEnd() && m_parsedStatementsQueue.isEmpty() ) {
                QString buf = m_stream->read( bufSize );
                QByteArray utf8Data = buf.toUtf8();
                if ( raptor_parse_chunk( m_parser->parser(), ( const unsigned char* )utf8Data.data(), utf8Data.length(), 0 ) ) {
                    setError( m_parser->lastError() );
                    close();
                    return false;
                }
            }

            // Once we read all data, we need to finalize parsing.
            if( m_stream->atEnd() ) {
                raptor_parse_chunk( m_parser->parser(), 0, 0, 1 );
                close();
            }
        }
//        qDebug() << m_parsedStatementsQueue.count() << "statements in parse queue";
    }

    return !m_parsedStatementsQueue.isEmpty();
}


Soprano::Statement Soprano::Raptor::ParserStatementIteratorBackend::current() const
{
    if( !m_parsedStatementsQueue.isEmpty() ) {
        return m_parsedStatementsQueue.head();
    }
    else {
        return Statement();
    }
}


void Soprano::Raptor::ParserStatementIteratorBackend::close()
{
    delete m_parser;
    m_parser = 0;
    if( m_deleteSource ) {
        delete m_stream;
        m_stream = 0;
        delete m_ioDevice;
        m_ioDevice = 0;
    }
}


void Soprano::Raptor::ParserStatementIteratorBackend::raptorTriplesHandler( void* userData, const raptor_statement* triple )
{
    Soprano::Raptor::ParserStatementIteratorBackend* it = static_cast<Soprano::Raptor::ParserStatementIteratorBackend*>( userData );
    Soprano::Statement s = convertTriple( triple );
//        qDebug() << "got triple: " << s;
    s.setContext( it->m_currentContext );
    it->m_parsedStatementsQueue.enqueue( s );
}


void Soprano::Raptor::ParserStatementIteratorBackend::raptorGraphHandler( void* userData, raptor_uri* graph )
{
    Soprano::Raptor::ParserStatementIteratorBackend* it = static_cast<Soprano::Raptor::ParserStatementIteratorBackend*>( userData );
    Soprano::Node context = Soprano::Node::createResourceNode( QString::fromUtf8( ( char* )raptor_uri_as_string( graph ) ) );
    it->m_currentContext = context;
//        qDebug() << "got graph: " << context;
}
