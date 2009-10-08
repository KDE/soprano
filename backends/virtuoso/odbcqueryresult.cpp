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

#include "virtuosotools.h"

#include "node.h"
#include "literalvalue.h"
#include "xsd.h"

#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QDebug>


/******************* Virtuoso defines *******************/

#define DV_LONG_INT 189

#define DV_STRING 182
#define DV_RDF 246		/*!< RDF object that is SQL value + type id + language id + outline id + flag whether the sql value is full */

#define DV_LONG_INT 189
#define DV_SINGLE_FLOAT 190
#define DV_DOUBLE_FLOAT 191
#define DV_NUMERIC 219
#define DV_DATETIME 211
#define DV_IRI_ID 243
#define DV_TIMESTAMP 128
#define DV_TIMESTAMP_OBJ 208
#define DV_DATE  129
#define DV_TIME  210


#define DT_TYPE_DATETIME 1
#define DT_TYPE_DATE 2
#define DT_TYPE_TIME 3


#define SQL_DESC_COL_DV_TYPE		1057L
#define SQL_DESC_COL_DT_DT_TYPE		1058L
#define SQL_DESC_COL_LITERAL_ATTR	1059L
#define SQL_DESC_COL_BOX_FLAGS		1060L

/******************************************************************/



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
                    setError( Virtuoso::convertSqlError( SQL_HANDLE_STMT, d->m_hstmt ) );
                    break;
                }
            }
        }
    }

    return d->m_columns;
}


bool Soprano::ODBC::QueryResult::fetchScroll()
{
    int sts = SQLFetchScroll( d->m_hstmt, SQL_FETCH_NEXT, 1 );
    if ( sts == SQL_NO_DATA_FOUND ) {
        clearError();
        return false;
    }
    else if( sts != SQL_SUCCESS ) {
        setError( Virtuoso::convertSqlError( SQL_HANDLE_STMT, d->m_hstmt ) );
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
    if ( d->m_conn->getCharData( d->m_hstmt, colNum, &data, &length ) ) {
        SQLHDESC hdesc = 0;
        int dvtype = 0;
        int dv_dt_type = 0;
        int flag = 0;
        Soprano::Node node;

        int rc = SQLGetStmtAttr( d->m_hstmt, SQL_ATTR_IMP_ROW_DESC, &hdesc, SQL_IS_POINTER, 0 );
        if ( !SQL_SUCCEEDED(rc) ) {
            setError( Virtuoso::convertSqlError( SQL_HANDLE_STMT, d->m_hstmt ) );
            return Node();
        }
        rc = SQLGetDescField( hdesc, colNum, SQL_DESC_COL_DV_TYPE, &dvtype, SQL_IS_INTEGER, 0 );
        if ( !SQL_SUCCEEDED(rc) ) {
            setError( Virtuoso::convertSqlError( SQL_HANDLE_STMT, d->m_hstmt ) );
            return Node();
        }
        rc = SQLGetDescField( hdesc, colNum, SQL_DESC_COL_DT_DT_TYPE, &dv_dt_type, SQL_IS_INTEGER, 0 );
        if ( !SQL_SUCCEEDED(rc) ) {
            setError( Virtuoso::convertSqlError( SQL_HANDLE_STMT, d->m_hstmt ) );
            return Node();
        }
        rc = SQLGetDescField( hdesc, colNum, SQL_DESC_COL_LITERAL_ATTR, &flag, SQL_IS_INTEGER, 0 );
        if ( !SQL_SUCCEEDED(rc) ) {
            setError( Virtuoso::convertSqlError( SQL_HANDLE_STMT, d->m_hstmt ) );
            return Node();
        }
        short l_lang = (short)((flag >> 16) & 0xFFFF);
        short l_type = (short)(flag & 0xFFFF);
        rc = SQLGetDescField( hdesc, colNum, SQL_DESC_COL_BOX_FLAGS, &flag, SQL_IS_INTEGER, 0 );
        if ( !SQL_SUCCEEDED(rc) ) {
            setError( Virtuoso::convertSqlError( SQL_HANDLE_STMT, d->m_hstmt ) );
            return Node();
        }

        switch (dvtype) {
        case DV_STRING: {
            if (flag) {
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
                else {
                    node = Node( LiteralValue::createPlainLiteral( QString::fromUtf8( reinterpret_cast<const char*>( data ) ) ) );
                }
            }
            break;
        }
        case DV_RDF: {
            QUrl type = d->m_conn->getType( l_type );
            QString lang = d->m_conn->getLang( l_lang );
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
        case DV_LONG_INT: /* integer */
            node = LiteralValue::fromString( QString::fromUtf8( reinterpret_cast<const char*>( data ) ), QVariant::Int );
            break;

        case DV_SINGLE_FLOAT: /* float */
            node = LiteralValue::fromString( QString::fromUtf8( reinterpret_cast<const char*>( data ) ), Vocabulary::XMLSchema::xsdFloat() );
            break;

        case DV_DOUBLE_FLOAT: /* double */
            node = LiteralValue::fromString( QString::fromUtf8( reinterpret_cast<const char*>( data ) ), QVariant::Double );
            break;

        case DV_NUMERIC: /* decimal */
            node = LiteralValue::fromString( QString::fromUtf8( reinterpret_cast<const char*>( data ) ), Vocabulary::XMLSchema::decimal() );
            break;

        case DV_TIMESTAMP: /* datetime */
        case DV_DATE:
        case DV_TIME:
        case DV_DATETIME: {
//            qDebug() << "datetime data:" << ( const char* )data;
            QUrl type;
            switch( dv_dt_type ) {
            case DT_TYPE_DATE:
                type = Vocabulary::XMLSchema::date();
                break;
            case DT_TYPE_TIME:
                type = Vocabulary::XMLSchema::time();
                break;
            default:
                type = Vocabulary::XMLSchema::dateTime();
                break;
            }
            QString dts = QString::fromUtf8( reinterpret_cast<const char*>( data ) );
            // FIXME: make Virtuoso return correct values
            dts.replace( ' ', 'T' );
            node = LiteralValue::fromString( dts, type );
            break;
        }
        case DV_IRI_ID:
            node = LiteralValue( QString::fromUtf8( reinterpret_cast<const char*>( data ) ) );
            break;
        default:
             /***printf ("*unexpected result type %d*", dtp);***/
            break;
        }

        delete [] data;

        return node;
    }
    else {
        setError( Virtuoso::convertSqlError( SQL_HANDLE_STMT, d->m_hstmt ) );
        return Node();
    }
}
