/* This file is part of Soprano
 *
 * Copyright (C) 2007-2008 Sebastian Trueg <trueg@kde.org>
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

#include "parser2.h"
#include "statementiterator.h"
#include "iteratorbackend.h"

#include <QtCore/QBuffer>
#include <QtCore/QFile>
#include <QtCore/QUrl>
#include <QtCore/QStringList>
#include <QtCore/QDebug>


namespace {
    /**
     * Wrapper iterator which forwards all calls and deletes the io device
     * when done.
     * This is necessary since parsers may continue parsing in the iterator
     * implementation after it has been returned from Parser2::parse.
     */
    class StatementIteratorWrapper : public Soprano::IteratorBackend<Soprano::Statement>
    {
    public:
        StatementIteratorWrapper( const Soprano::StatementIterator &it, QIODevice* dev, QByteArray* data = 0 )
            : m_it( it ),
              m_device( dev ),
              m_data( data ) {
        }

        Soprano::Statement current() const {
            return m_it.current();
        }

        bool next() {
            return m_it.next();
        }

        void close() {
            m_it.close();
            delete m_device;
            m_device = 0;
            delete m_data;
            m_data = 0;
        }

    private:
        Soprano::StatementIterator m_it;
        QIODevice* m_device;
        QByteArray* m_data;
    };
}


class Soprano::Parser2::Private
{
};


Soprano::Parser2::Parser2( const QString& name )
    : Plugin( name ),
      d( new Private() )
{
}


Soprano::Parser2::~Parser2()
{
    delete d;
}


Soprano::StatementIterator Soprano::Parser2::parseFile( const QString& filename, const QUrl& baseUri, RdfSerialization serialization, const QString& userSerialization ) const
{
    QFile* f = new QFile( filename );
    if ( f->open( QIODevice::ReadOnly | QIODevice::Text ) ) {
        clearError();
        return new StatementIteratorWrapper( parse( f, baseUri, serialization, userSerialization ), f );
    }
    else {
        delete f;
        setError( QString("Unable to open file %1").arg( filename ), Error::ErrorInvalidArgument );
        return 0;
    }
}


Soprano::StatementIterator Soprano::Parser2::parseString( const QByteArray& data, const QUrl& baseUri, RdfSerialization serialization, const QString& userSerialization ) const
{
    clearError();
    QByteArray* bufData = new QByteArray( data );
    QBuffer* buf = new QBuffer( bufData );
    buf->open( QIODevice::ReadOnly );
    return new StatementIteratorWrapper( parse( buf, baseUri, serialization, userSerialization ), buf, bufData );
}


bool Soprano::Parser2::supportsSerialization( RdfSerialization s, const QString& userSerialization ) const
{
    if ( s == SerializationUser || s == SerializationUnknown ) {
        return supportedUserSerializations().contains( userSerialization );
    }
    else {
        return supportedSerializations() & s;
    }
}


QStringList Soprano::Parser2::supportedUserSerializations() const
{
    return QStringList();
}
