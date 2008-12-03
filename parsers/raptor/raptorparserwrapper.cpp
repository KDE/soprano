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

#include "raptorparserwrapper.h"
#include "locator.h"
#include "error.h"

#include <raptor.h>

#include <QtCore/QDebug>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>


namespace {
    class RaptorInitHelper
    {
    public:
        RaptorInitHelper()
            : m_refCount( 0 ) {
        }

        ~RaptorInitHelper() {
        }

        void ref() {
            QMutexLocker lock( &m_mutex );
            if( m_refCount == 0 ) {
                raptor_init();
            }
            ++m_refCount;
        }

        void deref() {
            QMutexLocker lock( &m_mutex );
            Q_ASSERT( m_refCount > 0 );
            --m_refCount;
            if( m_refCount == 0 ) {
                raptor_finish();
            }
        }

    private:
        int m_refCount;
        QMutex m_mutex;
    };

    Q_GLOBAL_STATIC( RaptorInitHelper, globalRaptorRef );


    // raptor <= 1.4.15 (the onys without trig support) has a bug where is can only handle
    // application/turtle when parsing and application/x-turtle when serializing. Newer
    // versions always use the correct type
    QString mimeTypeString( Soprano::RdfSerialization s, const QString& userSerialization )
    {
#ifndef RAPTOR_HAVE_TRIG
        if ( s == Soprano::SerializationTurtle ) {
            return "application/turtle"; // x-turtle does not work....
        }
        else
#endif
        {
            return serializationMimeType( s, userSerialization );
        }
    }
}


Soprano::Raptor::ParserWrapper::ParserWrapper( raptor_parser* parser, raptor_uri* baseUri )
    : m_parser( parser ),
      m_raptorBaseUri( baseUri )
{
    grabRaptorMessages( parser );
}


Soprano::Raptor::ParserWrapper::~ParserWrapper()
{
    raptor_free_parser( m_parser );
    if ( m_raptorBaseUri ) {
        raptor_free_uri( m_raptorBaseUri );
    }
    globalRaptorRef()->deref();
}


void Soprano::Raptor::ParserWrapper::raptorMessageHandler( void* userData, raptor_locator* locator, const char* message )
{
    Soprano::Raptor::ParserWrapper* p = static_cast<Soprano::Raptor::ParserWrapper*>( userData );
    if ( locator ) {
        p->setError( Soprano::Error::ParserError( Soprano::Error::Locator( locator->line, locator->column, locator->byte ),
                                                  QString::fromUtf8( message ),
                                                  Soprano::Error::ErrorParsingFailed ) );
    }
    else {
        p->setError( Soprano::Error::Error( QString::fromUtf8( message ), Soprano::Error::ErrorUnknown ) );
    }
}


void Soprano::Raptor::ParserWrapper::setRaptorError( const Error::Error& error )
{
    setError( error );
}


void Soprano::Raptor::ParserWrapper::grabRaptorMessages( raptor_parser* parser )
{
    raptor_set_fatal_error_handler( parser, this, raptorMessageHandler );
    raptor_set_error_handler( parser, this, raptorMessageHandler );
    raptor_set_warning_handler( parser, this, raptorMessageHandler );
}


Soprano::Raptor::ParserWrapper* Soprano::Raptor::ParserWrapper::createParser( const QUrl& baseUri,
                                                                              RdfSerialization serialization,
                                                                              const QString& userSerialization )
{
    globalRaptorRef()->ref();

    // create the parser
    QString mimeType = mimeTypeString( serialization, userSerialization );
    raptor_parser* parser = 0;
    if ( serialization == Soprano::SerializationNTriples ) {
        parser = raptor_new_parser( "ntriples" ); // mimetype for ntriple is text/plain which is useless for the method below
    }
    else {
        parser = raptor_new_parser_for_content( 0,
                                                mimeType.toLatin1().data(),
                                                0,
                                                0,
                                                0 );
    }

    if ( !parser ) {
        qDebug() << "(Soprano::Raptor::Parser) no parser for serialization " << mimeTypeString( serialization, userSerialization );
        globalRaptorRef()->deref();
        return 0;
    }

    raptor_uri* raptorBaseUri = 0;
    if ( baseUri.isValid() ) {
        raptorBaseUri = raptor_new_uri( (unsigned char *) baseUri.toString().toUtf8().data() );
    }
    else {
        raptorBaseUri = raptor_new_uri( (unsigned char *) "http://soprano.sourceforge.net/dummyBaseUri" );
    }

    if ( raptor_start_parse( parser, raptorBaseUri ) ) {
        qDebug() << "Failed to start parsing.";
        raptor_free_parser( parser );
        if ( raptorBaseUri ) {
            raptor_free_uri( raptorBaseUri );
        }
        globalRaptorRef()->deref();
        return 0;
    }

    return new ParserWrapper( parser, raptorBaseUri );
}
