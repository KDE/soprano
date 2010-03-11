/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2008-2010 Sebastian Trueg <trueg@kde.org>
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

#ifndef _SOPRANO_DATA_STREAM_H_
#define _SOPRANO_DATA_STREAM_H_

#include "error.h"

class QUrl;
class QByteArray;

namespace Soprano {

    class LiteralValue;
    class BindingSet;
    class BackendSetting;
    class Node;
    class Statement;
    class Socket;
    namespace Error {
        class Locator;
    }

    /**
     * Provides streaming methods for all %Soprano
     * types. Compared to QDataStream's operators
     * it provides error handling and automatically
     * fills the buffer if more data needs to be
     * requested (QIODevice::waitForReadyRead)
     *
     * \author Sebastian Trueg <trueg@kde.org>
     */
    class DataStream : public Error::ErrorCache
    {
    public:
        DataStream( Socket* dev );
        ~DataStream();

        bool writeByteArray( const QByteArray& );
        bool writeString( const QString& );
        bool writeUrl( const QUrl& );
//        bool writeVariant( const QVariant& );
        bool writeUnsignedInt8( quint8 );
        bool writeUnsignedInt16( quint16 );
        bool writeUnsignedInt32( quint32 );
        bool writeInt32( qint32 );
        bool writeBool( bool );

        bool writeErrorCode( Error::ErrorCode code );
        bool writeLocator( const Error::Locator& );
        bool writeError( const Error::Error& );
//        bool writeBackendSetting( const BackendSetting& );
        bool writeLiteralValue( const LiteralValue& );
        bool writeNode( const Node& );
        bool writeStatement( const Statement& );
        bool writeBindingSet( const BindingSet& );

        bool readByteArray( QByteArray& );
        bool readString( QString& );
        bool readUrl( QUrl& );
//        bool readVariant( QVariant& );
        bool readUnsignedInt8( quint8& );
        bool readUnsignedInt16( quint16& );
        bool readUnsignedInt32( quint32& );
        bool readInt32( qint32& );
        bool readBool( bool& );

        bool readErrorCode( Error::ErrorCode& code );
        bool readLocator( Error::Locator& );
        bool readError( Error::Error& );
//        bool readBackendSetting( BackendSetting& );
        bool readLiteralValue( LiteralValue& );
        bool readNode( Node& );
        bool readStatement( Statement& );
        bool readBindingSet( BindingSet& );

    private:
        /**
         * Read from the device including waiting for data
         * to be ready.
         *
         * \sa QIODevice::read, QIODevice::waitForReadyRead
         */
        bool read( char* data, qint64 size );

        Socket* m_device;
    };
}

#endif
