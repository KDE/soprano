/*
 * This file is part of Soprano Project.
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

#include "datastream.h"

#include "node.h"
#include "statement.h"
#include "bindingset.h"
#include "backend.h"
#include "literalvalue.h"
#include "locator.h"
#include "languagetag.h"

#include <QtCore/QIODevice>
#include <QtCore/QDateTime>


Soprano::DataStream::DataStream()
{

}


Soprano::DataStream::~DataStream()
{
}


bool Soprano::DataStream::writeByteArray( const QByteArray& a )
{
    quint32 len = a.length();

    if ( !writeUnsignedInt32( len ) ) {
        return false;
    }

    if( !write( a.data(), a.size() ) ) {
        return false;
    }

    return true;
}


bool Soprano::DataStream::writeString( const QString& s )
{
    // FIXME: Optimize this by directly encoding QChars just like operator<< does
    return writeByteArray( s.toUtf8() );
}


bool Soprano::DataStream::writeUrl( const QUrl& url )
{
    return writeByteArray( url.toEncoded() );
}


bool Soprano::DataStream::writeUnsignedInt8( quint8 v )
{
    if( !write( (char*)&v, 1 ) ) {
        setError( "Failed to write unsigned int8." );
        return false;
    }
    return true;
}


bool Soprano::DataStream::writeUnsignedInt16( quint16 v )
{
    if( !write( (char*)&v, sizeof( quint16 ) ) ) {
        setError( "Failed to write unsigned int32." );
        return false;
    }
    return true;
}


bool Soprano::DataStream::writeUnsignedInt32( quint32 v )
{
    if( !write( (char*)&v, sizeof( quint32 ) ) ) {
        setError( "Failed to write unsigned int32." );
        return false;
    }
    return true;
}


bool Soprano::DataStream::writeInt32( qint32 v )
{
    if( !write( (char*)&v, sizeof( qint32 ) ) ) {
        setError( "Failed to write int32." );
        return false;
    }
    return true;
}


bool Soprano::DataStream::writeErrorCode( Error::ErrorCode code )
{
    return writeUnsignedInt16( ( quint16 )code );
}


bool Soprano::DataStream::writeBool( bool v )
{
    return writeUnsignedInt8( ( quint8 )v );
}


bool Soprano::DataStream::writeLocator( const Error::Locator& loc )
{
    return( writeInt32( ( qint32 )loc.line() ) &&
            writeInt32( ( qint32 )loc.column() ) &&
            writeInt32( ( qint32 )loc.byte() ) &&
            writeString( loc.fileName() ) );
}


bool Soprano::DataStream::writeError( const Error::Error& error )
{
    if ( !writeBool( error.isParserError() ) ||
         !writeInt32( ( qint32 )error.code() ) ||
         !writeString( error.message() ) ) {
        return false;
    }

    if ( error.isParserError() ) {
        Soprano::Error::ParserError e( error );
        if ( !writeLocator( e.locator() ) ) {
            return false;
        }
    }
    return true;
}


// bool Soprano::DataStream::writeBackendSetting( const BackendSetting& setting )
// {
//     return ( writeUnsignedInt16( ( quint16 )setting.option() ) &&
//              writeString( setting.userOptionName() ) &&
//              writeVariant( setting.value() ) );
// }


bool Soprano::DataStream::writeLiteralValue( const LiteralValue& value )
{
    if ( !writeBool( value.isPlain() ) ) {
        return false;
    }
    if ( value.isPlain() ) {
        return( writeString( value.toString() ) &&
                writeString( value.language().toString() ) );
    }
    else {
        QVariant var = value.variant();
        bool status = writeInt32( var.type() );
        switch( var.type() ) {
            case QVariant::String:
                status &= writeString( var.toString() );
                break;
            case QVariant::Url:
                status &= writeUrl( var.toUrl() );
                break;
            case QVariant::Int:
                status &= writeInt32( var.toInt() );
                break;
            case QVariant::Bool:
                status &= writeBool( var.toBool() );
                break;
            case QVariant::ByteArray:
                status &= writeByteArray( var.toByteArray() );
                break;
            case QVariant::DateTime :
                status &= writeUnsignedInt32( var.toDateTime().toTime_t() );
                break;
            default:
                status &= writeString( value.toString() );
                status &= writeUrl( value.dataTypeUri() );
        }
        return status;
    }
}


bool Soprano::DataStream::writeNode( const Node& node )
{
    if ( !writeUnsignedInt8( ( quint8 )node.type() ) ) {
        return false;
    }

    if ( node.type() == Soprano::Node::LiteralNode ) {
        if ( !writeLiteralValue( node.literal() ) ) {
            return false;
        }
    }
    else if ( node.type() == Soprano::Node::ResourceNode ) {
        if ( !writeUrl( node.uri() ) ) {
            return false;
        }
    }
    else if ( node.type() == Soprano::Node::BlankNode ) {
        if ( !writeString( node.identifier() ) ) {
            return false;
        }
    }

    return true;
}


bool Soprano::DataStream::writeStatement( const Statement& s )
{
    if ( !writeNode( s.subject() ) ||
         !writeNode( s.predicate() ) ||
         !writeNode( s.object() ) ||
         !writeNode( s.context() ) ) {
        return false;
    }
    return true;
}


bool Soprano::DataStream::writeBindingSet( const BindingSet& set )
{
    QStringList bn = set.bindingNames();
    if ( !writeUnsignedInt32( ( quint32 )bn.count() ) ) {
        return false;
    }
    for ( int i = 0; i < set.count(); ++i ) {
        if ( !writeString( bn[i] ) ||
             !writeNode( set[i] ) ) {
            return false;
        }
    }
    return true;
}


bool Soprano::DataStream::readByteArray( QByteArray& a )
{
    quint32 len;
    if ( !readUnsignedInt32( len ) ) {
        return false;
    }

    a.resize( len );

    return read( a.data(), len );
}


bool Soprano::DataStream::readString( QString& s )
{
    QByteArray a;
    if ( readByteArray( a ) ) {
        s = QString::fromUtf8( a );
        return true;
    }
    else {
        return false;
    }
}


bool Soprano::DataStream::readUrl( QUrl& url )
{
    QByteArray a;
    if ( readByteArray( a ) ) {
        url = QUrl::fromEncoded( a, QUrl::StrictMode );
        return true;
    }
    else {
        return false;
    }
}


bool Soprano::DataStream::readUnsignedInt8( quint8& i )
{
    if ( !read( ( char* )&i, 1 ) ) {
        i = 0;
        setError( "Failed to read unsigned int8" );
        return false;
    }
    else {
        clearError();
        return true;
    }
}


bool Soprano::DataStream::readUnsignedInt16( quint16& i )
{
    if ( !read( (char*)&i, 2 ) ) {
        i = 0;
        setError( "Failed to read unsigned int16" );
        return false;
    }
    else {
        clearError();
        return true;
    }
}


bool Soprano::DataStream::readUnsignedInt32( quint32& i )
{
    if ( !read( (char*)&i, 4 ) ) {
        i = 0;
        setError( "Failed to read unsigned int32" );
        return false;
    }
    else {
        clearError();
        return true;
    }
}


bool Soprano::DataStream::readInt32( qint32& i )
{
    if ( !read( (char*)&i, 4 ) ) {
        i = 0;
        setError( "Failed to read int32" );
        return false;
    }
    else {
        clearError();
        return true;
    }
}


bool Soprano::DataStream::readErrorCode( Error::ErrorCode& code )
{
    quint16 v;
    if ( readUnsignedInt16( v ) ) {
        code = Error::convertErrorCode( v );
        return true;
    }
    else {
        return false;
    }
}


bool Soprano::DataStream::readBool( bool& b )
{
    quint8 v;
    if ( readUnsignedInt8( v ) ) {
        b = v;
        return true;
    }
    else {
        return false;
    }
}


bool Soprano::DataStream::readLocator( Error::Locator& loc )
{
    qint32 line, column, byte;
    QString fileName;
    if ( readInt32( line ) &&
         readInt32( column ) &&
         readInt32( byte ) &&
         readString( fileName ) ) {
        loc = Soprano::Error::Locator( line, column, byte, fileName );
        return true;
    }
    else {
        return false;
    }
}


bool Soprano::DataStream::readError( Error::Error& error )
{
    bool isParserError = false;
    QString message;
    qint32 code;
    if ( !readBool( isParserError ) ||
         !readInt32( code ) ||
         !readString( message ) ) {
        return false;
    }

    if ( isParserError ) {
        Soprano::Error::Locator loc;
        if ( !readLocator( loc ) ) {
            return false;
        }
        error = Soprano::Error::ParserError( loc, message, code );
    }
    else {
        error = Soprano::Error::Error( message, code );
    }
    return true;
}


// bool Soprano::DataStream::readBackendSetting( BackendSetting& setting )
// {
//     quint16 option;
//     QString userOption;
//     QVariant value;
//     if ( readUnsignedInt16( option ) &&
//          readString( userOption ) &&
//          readVariant( value ) ) {
//         if ( option == Soprano::BackendOptionUser ) {
//             setting = Soprano::BackendSetting( userOption, value );
//         }
//         else {
//             setting = Soprano::BackendSetting( ( Soprano::BackendOption )option, value );
//         }
//         return true;
//     }
//     else {
//         return false;
//     }
// }


bool Soprano::DataStream::readLiteralValue( LiteralValue& val )
{
    bool plain;
    if ( readBool( plain ) ) {
        QString v;
        if ( plain ) {
            QString lang;
            if ( readString( v ) &&
                 readString( lang ) ) {
                val = LiteralValue::createPlainLiteral( v, lang );
                return true;
            }
        }
        else {
            int t;
            bool status = readInt32( t );
            if( !status )
                return false;

            QVariant::Type type = static_cast<QVariant::Type>( t );
            switch( type ) {
                case QVariant::String: {
                    QString str;
                    status &= readString( str );
                    val = LiteralValue( str );
                    break;
                }
                case QVariant::Url: {
                    QUrl url;
                    status &= readUrl( url );
                    val = LiteralValue( url );
                    break;
                }
                case QVariant::Int: {
                    int integer;
                    status &= readInt32( integer );
                    val = LiteralValue( integer );
                    break;
                }
                case QVariant::Bool: {
                    bool boolean;
                    status &= readBool( boolean );
                    val = LiteralValue( boolean );
                    break;
                }
                case QVariant::ByteArray: {
                    QByteArray array;
                    status &= readByteArray( array );
                    val = LiteralValue( array );
                    break;
                }
                case QVariant::DateTime : {
                    quint32 timet;
                    status &= readUnsignedInt32( timet );
                    val = LiteralValue( QDateTime::fromTime_t(timet) );
                    break;
                }
                default: {
                    QString str;
                    QUrl dt;
                    status &= readString( str );
                    status &= readUrl( dt );
                    val = LiteralValue::fromString( str, dt );
                    break;
                }
            }

            return status;
        }
    }

    return false;
}


bool Soprano::DataStream::readNode( Node& node )
{
    quint8 type;
    if ( !readUnsignedInt8( type ) ) {
        return false;
    }
    if ( type == Soprano::Node::LiteralNode ) {
        Soprano::LiteralValue v;
        if ( !readLiteralValue( v ) ) {
            return false;
        }
        node = v;
    }
    else if ( type == Soprano::Node::ResourceNode ) {
        QUrl url;
        if ( !readUrl( url ) ) {
            return false;
        }
        node = Soprano::Node( url );
    }
    else if ( type == Soprano::Node::BlankNode ) {
        QString id;
        if ( !readString( id ) ) {
            return false;
        }
        node = Soprano::Node( id );
    }
    else {
        node = Soprano::Node();
    }

    return true;
}


bool Soprano::DataStream::readStatement( Statement& s )
{
    Soprano::Node subject, predicate, object, context;
    if ( readNode( subject ) &&
         readNode( predicate ) &&
         readNode( object ) &&
         readNode( context ) ) {
        s = Statement( subject, predicate, object, context );
        return true;
    }
    else {
        return false;
    }
}


bool Soprano::DataStream::readBindingSet( BindingSet& set )
{
    set = BindingSet();
    quint32 nb;
    if ( !readUnsignedInt32( nb ) ) {
        return false;
    }
    for ( quint32 i = 0; i < nb; ++i ) {
        QString name;
        Node node;
        if ( !readString( name ) ||
             !readNode( node ) ) {
            return false;
        }
        set.insert( name, node );
    }
    return true;
}
