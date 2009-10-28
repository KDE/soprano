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
            for ( int col = 1; col <= numCols; ++col ) {
                SQLTCHAR colName[51];
                colName[50] = 0;
                SQLSMALLINT colType;
                SQLULEN colPrecision;
                SQLSMALLINT colScale;
                SQLSMALLINT colNullable;
                if ( SQLDescribeCol( d->m_hstmt,
                                     col,
                                     (SQLTCHAR *) colName,
                                     50,
                                     0,
                                     &colType,
                                     &colPrecision,
                                     &colScale,
                                     &colNullable) == SQL_SUCCESS ) {
                    d->m_columns.append( QString::fromLatin1( ( const char* )colName ) );
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

        //
        // Before we can retrieve the column meta data using SQLGetDescField,
        // we first needs to retrieve the correct descriptor handle attached to the statement handle
        //
        if ( !SQL_SUCCEEDED( SQLGetStmtAttr( d->m_hstmt, SQL_ATTR_IMP_ROW_DESC, &hdesc, SQL_IS_POINTER, 0 ) ) ) {
            setError( Virtuoso::convertSqlError( SQL_HANDLE_STMT, d->m_hstmt, QLatin1String( "SQLGetStmtAttr failed" ) ) );
            delete [] data;
            return Node();
        }

        //
        // Retrieve the datatype of a field
        // Will yield one of the VIRTUOSO_DV_* defined in virtuosoodbcext.h
        //
        else if ( !SQL_SUCCEEDED( SQLGetDescField( hdesc, colNum, SQL_DESC_COL_DV_TYPE, &dvtype, SQL_IS_INTEGER, 0 ) ) ) {
            setError( Virtuoso::convertSqlError( SQL_HANDLE_STMT, d->m_hstmt, QLatin1String( "SQLGetDescField SQL_DESC_COL_DV_TYPE failed" ) ) );
            delete [] data;
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
                delete [] data;
                return Node();
            }

            if ( boxFlags == 1 ) {
                if ( data && strncmp( (char*)data, "_:", 2 ) == 0 ) {
                    node = Node( QString::fromLatin1( reinterpret_cast<const char*>( data )+2 ) );
                }
                else {
                    node = Node( QUrl::fromEncoded( reinterpret_cast<const char*>( data ), QUrl::StrictMode ) );
                }
            }
            else {
                if ( data && strncmp( (char*)data, "nodeID://", 9 ) == 0 ) {
                    node = Node( QString::fromLatin1( reinterpret_cast<const char*>( data )+9 ) );
                }
                else if ( boxFlags == 2 ) {
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
            SQLCHAR langBuf[100];
            SQLCHAR typeBuf[100];
            SQLINTEGER langBufLen = 0;
            SQLINTEGER typeBufLen = 0;
            if ( !SQL_SUCCEEDED( SQLGetDescField( hdesc, colNum, SQL_DESC_COL_LITERAL_LANG, langBuf, sizeof( langBuf ), &langBufLen ) ) ||
                 !SQL_SUCCEEDED( SQLGetDescField( hdesc, colNum, SQL_DESC_COL_LITERAL_TYPE, typeBuf, sizeof( typeBuf ), &typeBufLen ) ) ) {
                setError( Virtuoso::convertSqlError( SQL_HANDLE_STMT, d->m_hstmt, QLatin1String( "SQLGetDescField SQL_DESC_COL_LITERAL_* failed" ) ) );
                delete [] data;
                return Node();
            }

            QUrl type = QUrl::fromEncoded( QByteArray::fromRawData( reinterpret_cast<const char*>( typeBuf ), typeBufLen ), QUrl::StrictMode );
            QString lang = QString::fromLatin1( reinterpret_cast<const char*>( langBuf ), langBufLen );
            const char* str = reinterpret_cast<const char*>( data );

            if ( type == Virtuoso::fakeBooleanType() ) {
                node = Node( LiteralValue( !qstrcmp( "true", str ) ) );
            }
            else {
                if ( type == Virtuoso::fakeBase64BinaryType() )
                    type = Soprano::Vocabulary::XMLSchema::base64Binary();

                if ( type.isEmpty() )
                    node = Node( LiteralValue::createPlainLiteral( QString::fromUtf8( str ), lang ) );
                else
                    node = Node( LiteralValue::fromString( QString::fromUtf8( str ), type ) );
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
                delete [] data;
                return Node();
            }
            QUrl type;
            switch( dv_dt_type ) {
            case VIRTUOSO_DT_TYPE_DATE:
                type = Vocabulary::XMLSchema::date();
                break;
            case VIRTUOSO_DT_TYPE_TIME:
                type = Vocabulary::XMLSchema::time();
                break;
            default:
                type = Vocabulary::XMLSchema::dateTime();
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
            // This type is only returned in output:valmode "LONG"
            // It needs to be translated into a literal string using the
            // ID_TO_IRI() function as the value is database specific.
            //
            setError( QLatin1String( "IRI_ID is not supported yet." ) );
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

        delete [] data;

        return node;
    }
    else {
        return Node();
    }
}


bool Soprano::ODBC::QueryResult::getCharData( int colNum, SQLCHAR** buffer, SQLLEN* length )
{
    SQLCHAR dummyBuffer[1]; // dummy buffer only used to determine length

    int r = SQLGetData( d->m_hstmt, colNum, SQL_C_CHAR, dummyBuffer, 0, length );

    if ( SQL_SUCCEEDED( r ) ) {
        //
        // Treat a 0 length and null data as an empty node
        //
        if ( *length == SQL_NULL_DATA || *length == 0 ) {
            *buffer = 0;
            *length = 0;
            clearError();
            return true;
        }

        //
        // again with real length buffer
        //
        else {
            *buffer = new SQLCHAR[*length+4]; // FIXME: why +4 (I got this from the redland plugin)
            r = SQLGetData ( d->m_hstmt, colNum, SQL_C_CHAR, *buffer, *length+4, length );
            if ( SQL_SUCCEEDED( r ) ) {
                clearError();
                return true;
            }
            else {
                delete [] *buffer;
                *buffer = 0;
                *length = 0;
                setError( Virtuoso::convertSqlError( SQL_HANDLE_STMT, d->m_hstmt, QLatin1String( "SQLGetData failed" ) ) );
                return false;
            }
        }
    }
    else {
        setError( Virtuoso::convertSqlError( SQL_HANDLE_STMT, d->m_hstmt, QLatin1String( "SQLGetData for data lenght failed" ) ) );
        return false;
    }
}
