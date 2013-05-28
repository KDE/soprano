/*
 * This file is part of Soprano Project
 *
 * Copyright (C) 2009 Sebastian Trueg <trueg@kde.org>
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

#include "odbcqueryresult.h"
#include "odbcqueryresult_p.h"
#include "odbcconnection_p.h"
#include "virtuosoodbcext.h"
#include "virtuosotools.h"

#include "node.h"
#include "literalvalue.h"
#include "xsd.h"

#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QScopedPointer>
#include <QtCore/QDebug>


Soprano::ODBC::QueryResult::QueryResult()
    : d( new QueryResultPrivate() )
{
}


Soprano::ODBC::QueryResult::~QueryResult()
{
    d->m_conn->m_openResults.removeAll( this );
    SQLCloseCursor( d->m_hstmt );
    SQLFreeHandle( SQL_HANDLE_STMT, d->m_hstmt );
    delete d;
}


QStringList Soprano::ODBC::QueryResult::resultColumns()
{
    if ( d->m_columns.isEmpty() ) {
        SQLSMALLINT numCols = -1;
        if ( SQLNumResultCols( d->m_hstmt, &numCols ) != SQL_SUCCESS ) {
            setError( Virtuoso::convertSqlError( SQL_HANDLE_STMT, d->m_hstmt ) );
        }
        else {
            clearError();
            d->m_columns.reserve( numCols );
            d->m_columTypes.reserve( numCols );
            for ( int col = 1; col <= numCols; ++col ) {
                SQLTCHAR colName[51];
                colName[50] = 0;
                SQLSMALLINT colType;
                if ( SQLDescribeCol( d->m_hstmt,
                                     col,
                                     (SQLTCHAR *) colName,
                                     50,
                                     0,
                                     &colType,
                                     0,
                                     0,
                                     0) == SQL_SUCCESS ) {
                    d->m_columns.append( QString::fromLatin1( ( const char* )colName ) );
                    d->m_columTypes.append( colType );
                }
                else {
                    setError( Virtuoso::convertSqlError( SQL_HANDLE_STMT, d->m_hstmt, QLatin1String( "SQLDescribeCol failed" ) ) );
                    break;
                }
            }
        }
    }

    return d->m_columns;
}


bool Soprano::ODBC::QueryResult::fetchRow()
{
    int sts = SQLFetch( d->m_hstmt );
    if ( sts == SQL_NO_DATA_FOUND ) {
        clearError();
        return false;
    }
    else if( sts != SQL_SUCCESS ) {
        setError( Virtuoso::convertSqlError( SQL_HANDLE_STMT, d->m_hstmt, QLatin1String( "SQLFetch failed" ) ) );
        return false;
    }
    else {
        return true;
    }
}


Soprano::Node Soprano::ODBC::QueryResult::getData( int colNum )
{
    SQLCHAR* data = 0;
    SQLLEN length = 0;
    if ( getCharData( colNum, &data, &length ) ) {
        SQLHDESC hdesc = 0;
        int dvtype = 0;

        // easy mem cleanup: never care about data again below
        QScopedPointer<SQLCHAR, QScopedPointerArrayDeleter<SQLCHAR> > dap( data );

        //
        // Before we can retrieve the column meta data using SQLGetDescField,
        // we first needs to retrieve the correct descriptor handle attached to the statement handle
        //
        if ( !SQL_SUCCEEDED( SQLGetStmtAttr( d->m_hstmt, SQL_ATTR_IMP_ROW_DESC, &hdesc, SQL_IS_POINTER, 0 ) ) ) {
            setError( Virtuoso::convertSqlError( SQL_HANDLE_STMT, d->m_hstmt, QLatin1String( "SQLGetStmtAttr failed" ) ) );
            return Node();
        }

        //
        // Retrieve the datatype of a field
        // Will yield one of the VIRTUOSO_DV_* defined in virtuosoodbcext.h
        //
        else if ( !SQL_SUCCEEDED( SQLGetDescField( hdesc, colNum, SQL_DESC_COL_DV_TYPE, &dvtype, SQL_IS_INTEGER, 0 ) ) ) {
            setError( Virtuoso::convertSqlError( SQL_HANDLE_STMT, d->m_hstmt, QLatin1String( "SQLGetDescField SQL_DESC_COL_DV_TYPE failed" ) ) );
            return Node();
        }

        // The node we will construct below
        Soprano::Node node;

        switch (dvtype) {
        case VIRTUOSO_DV_STRING: {
            //
            // Retrieve the flags associated with the field:
            // 0  - field contains a normal string
            // 1  - field contains an IRI string
            // 2  - field contains a UTF-8 string
            //
            int boxFlags = 0;
            if ( !SQL_SUCCEEDED( SQLGetDescField( hdesc, colNum, SQL_DESC_COL_BOX_FLAGS, &boxFlags, SQL_IS_INTEGER, 0 ) ) ) {
                setError( Virtuoso::convertSqlError( SQL_HANDLE_STMT, d->m_hstmt, QLatin1String( "SQLGetDescField failed" ) ) );
                return Node();
            }

            if ( boxFlags & VIRTUOSO_BF_IRI ) {
                if ( data && strncmp( (char*)data, "_:", 2 ) == 0 ) {
                    node = Node( QString::fromUtf8( reinterpret_cast<const char*>( data )+2 ) );
                }
                else {
                    node = Node( QUrl::fromEncoded( reinterpret_cast<const char*>( data ), QUrl::StrictMode ) );
                }
            }
            else {
                if ( data && strncmp( (char*)data, "nodeID://", 9 ) == 0 ) {
                    node = Node( QString::fromLatin1( reinterpret_cast<const char*>( data )+9 ) );
                }
                else if ( boxFlags & VIRTUOSO_BF_UTF8 ) {
                    node = Node( LiteralValue::createPlainLiteral( QString::fromUtf8( reinterpret_cast<const char*>( data ) ) ) );
                }
                else {
                    node = Node( LiteralValue::createPlainLiteral( QString::fromLatin1( reinterpret_cast<const char*>( data ) ) ) );
                }
            }
            break;
        }

        case VIRTUOSO_DV_RDF: {
            //
            // Retrieve lang and type strings which are cached in the server for faster lookups
            //
            SQLCHAR typeBuf[100];
            SQLINTEGER typeBufLen = 0;

            bool fetchTypeSucceded = SQL_SUCCEEDED( SQLGetDescField( hdesc, colNum,
                                                                     SQL_DESC_COL_LITERAL_TYPE,
                                                                     typeBuf, sizeof( typeBuf ), &typeBufLen ) );

            const char* str = reinterpret_cast<const char*>( data );

            if( fetchTypeSucceded ) {
                const char* typeStr = reinterpret_cast<const char*>( typeBuf );

                if ( !qstrncmp( typeStr, Virtuoso::fakeBooleanTypeString(), typeBufLen ) ) {
                    node = Node( LiteralValue( !qstrcmp( "true", str ) ) );
                }
                else {
                    QUrl type;
                    // FIXME: Disable these checks based on the backend settings!
                    if ( !qstrncmp( typeStr, Virtuoso::fakeBase64BinaryTypeString(), typeBufLen ) )
                        type = Soprano::Vocabulary::XMLSchema::base64Binary();
                    else
                        type = QUrl::fromEncoded( QByteArray::fromRawData( typeStr, typeBufLen ), QUrl::StrictMode );
                    node = Node( LiteralValue::fromString( QString::fromUtf8( str ), type ) );
                }
            }
            else {
                SQLCHAR langBuf[100];
                SQLINTEGER langBufLen = 0;

                bool fetchLangSucceded = SQL_SUCCEEDED( SQLGetDescField( hdesc, colNum,
                                                                         SQL_DESC_COL_LITERAL_LANG,
                                                                         langBuf, sizeof( langBuf ), &langBufLen ) );

                if( fetchLangSucceded ) {
                    QString lang = QString::fromLatin1( reinterpret_cast<const char*>( langBuf ), langBufLen );
                    node = Node( LiteralValue::createPlainLiteral( QString::fromUtf8( str ), lang ) );
                }
                else {
                    setError( Virtuoso::convertSqlError( SQL_HANDLE_STMT, d->m_hstmt,
                                                         QLatin1String( "SQLGetDescField SQL_DESC_COL_LITERAL_* failed" ) ) );
                    return Node();
                }
            }
            break;
        }

        case VIRTUOSO_DV_LONG_INT:
            node = LiteralValue::fromString( QString::fromUtf8( reinterpret_cast<const char*>( data ) ), QVariant::Int );
            break;

        case VIRTUOSO_DV_SINGLE_FLOAT:
            node = LiteralValue::fromString( QString::fromUtf8( reinterpret_cast<const char*>( data ) ), Vocabulary::XMLSchema::xsdFloat() );
            break;

        case VIRTUOSO_DV_DOUBLE_FLOAT:
            node = LiteralValue::fromString( QString::fromUtf8( reinterpret_cast<const char*>( data ) ), QVariant::Double );
            break;

        case VIRTUOSO_DV_NUMERIC:
            node = LiteralValue::fromString( QString::fromUtf8( reinterpret_cast<const char*>( data ) ), Vocabulary::XMLSchema::decimal() );
            break;

        case VIRTUOSO_DV_TIMESTAMP:
        case VIRTUOSO_DV_DATE:
        case VIRTUOSO_DV_TIME:
        case VIRTUOSO_DV_DATETIME: {
            //
            // Retrieve the date subtype
            // Will yield one of the VIRTUOSO_DT_TYPE_* defined in virtuosoodbcext.h
            //
            int dv_dt_type = 0;
            if ( !SQL_SUCCEEDED( SQLGetDescField( hdesc, colNum, SQL_DESC_COL_DT_DT_TYPE, &dv_dt_type, SQL_IS_INTEGER, 0 ) ) ) {
                setError( Virtuoso::convertSqlError( SQL_HANDLE_STMT, d->m_hstmt, QLatin1String( "SQLGetDescField SQL_DESC_COL_DT_DT_TYPE failed" ) ) );
                return Node();
            }
            QVariant::Type type;
            switch( dv_dt_type ) {
            case VIRTUOSO_DT_TYPE_DATE:
                type = QVariant::Date;
                break;
            case VIRTUOSO_DT_TYPE_TIME:
                type = QVariant::Time;
                break;
            default:
                type = QVariant::DateTime;
                break;
            }
            QString dts = QString::fromUtf8( reinterpret_cast<const char*>( data ) );
            // Virtuoso returns datetime values with a space instead of a T: "2009-04-07 13:33:19.790"
            dts.replace( ' ', 'T' );
            node = LiteralValue::fromString( dts, type );
            break;
        }

        case VIRTUOSO_DV_IRI_ID:
            //
            // node is an IRI ID
            //
            // It needs to be translated into a URIusing the
            // ID_TO_IRI() function as the value is database specific.
            //
            // For now, we simply pass it on as a string literal
            //
            node = LiteralValue(QString::fromLatin1(reinterpret_cast<const char*>(data), length));
            break;

        case 204:
            // VIRTUOSO_DV_DB_NULL
            // a null node -> empty
            break;

        default:
            qDebug("*unexpected result type %d*", dvtype);
            setError( QString( "Internal Error: Unknown result type %1" ).arg( dvtype ) );
            break;
        }

        return node;
    }
    else {
        return Node();
    }
}


bool Soprano::ODBC::QueryResult::isBlob( int colNum )
{
    return ( d->m_columTypes[colNum-1] == SQL_LONGVARCHAR ||
             d->m_columTypes[colNum-1] == SQL_LONGVARBINARY ||
             d->m_columTypes[colNum-1] == SQL_WLONGVARCHAR );
}


bool Soprano::ODBC::QueryResult::getCharData( int colNum, SQLCHAR** buffer, SQLLEN* length )
{
    // We pre alocate a buffer which can hold most of the values that we get.
    // If it cannot, only then do we allocate the proper size
    // This way we avoid the extra SQLGetData call
    const static int bufSize = 100;
    *buffer = new SQLCHAR[ bufSize ];

    int r = SQLGetData( d->m_hstmt, colNum, SQL_C_CHAR, *buffer, bufSize, length );

    if ( SQL_SUCCEEDED( r ) ) {
        //
        // Treat a 0 length and null data as an empty node
        //
        if ( *length == SQL_NULL_DATA || *length == 0 ) {
            delete [] *buffer;
            *buffer = 0;
            *length = 0;
            clearError();
            return true;
        }

        if( *length > bufSize ) {
            SQLCHAR* oldBuffer = *buffer;

            *buffer = new SQLCHAR[ *length + 4 ]; // FIXME: Why the +4 (I got this from the redland plugin)
            memcpy( *buffer, oldBuffer, bufSize );
            delete [] oldBuffer;

            // The -1 is cause SQLGetData returns a null terminated string
            SQLCHAR* newBuffer = (*buffer) + bufSize - 1;
            int len = *length - ( bufSize - 1 ) + 1; // The +1 is for the null char

            int r = SQLGetData( d->m_hstmt, colNum, SQL_C_CHAR, newBuffer, len, length );
            if( SQL_SUCCEEDED( r ) ) {
                clearError();
                return true;
            }
            else {
                delete [] *buffer;
                *buffer = 0;
                *length = 0;
                setError( Virtuoso::convertSqlError( SQL_HANDLE_STMT, d->m_hstmt,
                                                     QLatin1String( "SQLGetData failed" ) ) );
                return false;
            }
        }
        else {
            clearError();
            return true;
        }
    }
    else {
        delete [] *buffer;
        *buffer = 0;
        *length = 0;
        setError( Virtuoso::convertSqlError( SQL_HANDLE_STMT, d->m_hstmt,
                                             QLatin1String( "SQLGetData failed" ) ) );
        return false;
    }
}
