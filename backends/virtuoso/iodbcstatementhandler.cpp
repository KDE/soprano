/*
 * This file is part of Soprano Project
 *
 * Copyright (C) 2008 Sebastian Trueg <trueg@kde.org>
 *
 * Certain bits taken from iodbctest.c
 * Copyright (C) 1996-2006 by OpenLink Software <iodbc@openlinksw.com>
 *
 * Certain bits taken from rdf_storage_virtuoso.c
 * Copyright (C) 2000-2008, Openlink Software,  http://www.openlinksw.com/
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

#include "iodbcstatementhandler.h"
#include "iodbctools.h"
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


namespace {
    template<typename T> class KeyCache {
    public:
        bool contains( short key ) const {
            QMutexLocker lock( &m_mutex );
            return m_hash.contains( key );
        }
        void insert( short key, const T& value ) {
            QMutexLocker lock( &m_mutex );
            qDebug() << "inserting into KeyCache:" << key << value;
            m_hash.insert( key, value );
        }
        T value( short key ) const {
            QMutexLocker lock( &m_mutex );
            return m_hash[key];
        }

    private:
        QHash<short, T> m_hash;
        mutable QMutex m_mutex;
    };
}


Q_GLOBAL_STATIC( KeyCache<QString>, s_langCache )
Q_GLOBAL_STATIC( KeyCache<QUrl>, s_typeCache )


class Soprano::IODBCStatementHandler::Private
{
public:
    HDBC m_hdbc;
    HSTMT m_hstmt;

    bool getCharData( HSTMT hstmt, int colNum, SQLCHAR** buffer, SQLLEN* length );

    QString getLang( short key );
    QUrl getType( short key );
};


bool Soprano::IODBCStatementHandler::Private::getCharData( HSTMT hstmt, int colNum, SQLCHAR** buffer, SQLLEN* length )
{
    SQLCHAR dummyBuffer[1]; // dummy buffer only used to determine length

    int r = SQLGetData( hstmt, colNum, SQL_C_CHAR, dummyBuffer, 0, length );

    if ( SQL_SUCCEEDED( r ) ) {
        //
        // Treat a 0 length and null data as an empty node
        //
        if ( *length == SQL_NULL_DATA || *length == 0 ) {
            *buffer = 0;
            *length = 0;
            return true;
        }

        //
        // again with real length buffer
        //
        else {
            *buffer = new SQLCHAR[*length+4]; // FIXME: why +4 (I got this from the redland plugin)
            r = SQLGetData ( hstmt, colNum, SQL_C_CHAR, *buffer, *length+4, length );
            if ( SQL_SUCCEEDED( r ) ) {
                return true;
            }
            else {
                delete [] *buffer;
                *buffer = 0;
                *length = 0;
                return false;
            }
        }
    }
    else {
        return false;
    }
}


QString Soprano::IODBCStatementHandler::Private::getLang( short key )
{
    // FIXME: thread safety + error handling
    if ( s_langCache()->contains( key ) ) {
        return s_langCache()->value( key );
    }

    QString lang;

    HSTMT hstmt;
    int rc = SQLAllocHandle( SQL_HANDLE_STMT, m_hdbc, &hstmt );
    if ( !SQL_SUCCEEDED(rc) ) {
        qDebug() << "Failed to allocate HSTMT";
        return QString();
    }

    long ind = 0;
    rc = SQLBindParameter( hstmt, 1, SQL_PARAM_INPUT, SQL_C_SSHORT, SQL_SMALLINT, 0, 0, &key, 0, &ind );
    if ( SQL_SUCCEEDED(rc) ) {
        rc = SQLExecDirect( hstmt, TEXT("select RL_ID from DB.DBA.RDF_LANGUAGE where RL_TWOBYTE=?" ), SQL_NTS );
        if ( SQL_SUCCEEDED(rc) ) {
            rc = SQLFetch( hstmt );
            if ( SQL_SUCCEEDED(rc) ) {
                SQLCHAR* data = 0;
                SQLLEN length = 0;
                if ( getCharData( hstmt, 1, &data, &length ) ) {
                    lang = QString::fromLatin1( reinterpret_cast<const char*>( data ), length );
                    s_langCache()->insert( key, lang );
                }
            }
        }
    }

    SQLCloseCursor( hstmt );
    SQLFreeHandle( SQL_HANDLE_STMT, hstmt );

    return lang;
}


QUrl Soprano::IODBCStatementHandler::Private::getType( short key )
{
    // FIXME: thread safety + error handling
    if ( s_typeCache()->contains( key ) ) {
        return s_typeCache()->value( key );
    }

    QUrl type;

    HSTMT hstmt;

    int rc = SQLAllocHandle( SQL_HANDLE_STMT, m_hdbc, &hstmt );
    if (!SQL_SUCCEEDED(rc)) {
        qDebug() << "Failed to allocate HSTMT";
        return QUrl();
    }

    long ind;
    rc = SQLBindParameter( hstmt, 1, SQL_PARAM_INPUT, SQL_C_SSHORT, SQL_SMALLINT, 0, 0, &key, 0, &ind);
    if ( SQL_SUCCEEDED(rc) ) {
        rc = SQLExecDirect( hstmt, TEXT("select RDT_QNAME from DB.DBA.RDF_DATATYPE where RDT_TWOBYTE=?"), SQL_NTS );
        if ( SQL_SUCCEEDED(rc) ) {
            rc = SQLFetch( hstmt );
            if ( SQL_SUCCEEDED(rc) ) {
                SQLCHAR* data = 0;
                SQLLEN length = 0;
                if ( getCharData( hstmt, 1, &data, &length ) ) {
                    type = QUrl::fromEncoded( reinterpret_cast<const char*>( data ), QUrl::StrictMode );
                    s_typeCache()->insert( key, type );
                }
            }
        }
    }

    SQLCloseCursor( hstmt );
    SQLFreeHandle( SQL_HANDLE_STMT, hstmt );

    return type;
}


Soprano::IODBCStatementHandler::IODBCStatementHandler( HDBC hdbc, HSTMT hstmt )
    : d( new Private() )
{
    d->m_hdbc = hdbc;
    d->m_hstmt = hstmt;
}


Soprano::IODBCStatementHandler::~IODBCStatementHandler()
{
    if ( d->m_hstmt ) {
        SQLCloseCursor( d->m_hstmt );
        SQLFreeHandle( SQL_HANDLE_STMT, d->m_hstmt );
        d->m_hstmt = SQL_NULL_HANDLE;
    }
}


QStringList Soprano::IODBCStatementHandler::resultColumns()
{
    QStringList cols;
    SQLSMALLINT numCols = -1;
    if ( SQLNumResultCols( d->m_hstmt, &numCols ) != SQL_SUCCESS ) {
        setError( IODBC::convertSqlError( SQL_HANDLE_STMT, d->m_hstmt ) );
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
                cols.append( QString::fromWCharArray( colName ) );
            }
            else {
                setError( IODBC::convertSqlError( SQL_HANDLE_STMT, d->m_hstmt ) );
                break;
            }
        }
    }

    return cols;
}


bool Soprano::IODBCStatementHandler::fetchScroll()
{
    int sts = SQLFetchScroll( d->m_hstmt, SQL_FETCH_NEXT, 1 );
    if ( sts == SQL_NO_DATA_FOUND ) {
        clearError();
        return false;
    }
    else if( sts != SQL_SUCCESS ) {
        setError( IODBC::convertSqlError( SQL_HANDLE_STMT, d->m_hstmt ) );
        return false;
    }
    else {
        return true;
    }
}


Soprano::Node Soprano::IODBCStatementHandler::getData( int colNum )
{
    SQLCHAR* data = 0;
    SQLLEN length = 0;
    if ( d->getCharData( d->m_hstmt, colNum, &data, &length ) ) {
        SQLHDESC hdesc = 0;
        int dvtype = 0;
        int dv_dt_type = 0;
        int flag = 0;
        Soprano::Node node;

        int rc = SQLGetStmtAttr( d->m_hstmt, SQL_ATTR_IMP_ROW_DESC, &hdesc, SQL_IS_POINTER, 0 );
        if ( !SQL_SUCCEEDED(rc) ) {
            setError( IODBC::convertSqlError( SQL_HANDLE_STMT, d->m_hstmt ) );
            return Node();
        }
        rc = SQLGetDescField( hdesc, colNum, SQL_DESC_COL_DV_TYPE, &dvtype, SQL_IS_INTEGER, 0 );
        if ( !SQL_SUCCEEDED(rc) ) {
            setError( IODBC::convertSqlError( SQL_HANDLE_STMT, d->m_hstmt ) );
            return Node();
        }
        rc = SQLGetDescField( hdesc, colNum, SQL_DESC_COL_DT_DT_TYPE, &dv_dt_type, SQL_IS_INTEGER, 0 );
        if ( !SQL_SUCCEEDED(rc) ) {
            setError( IODBC::convertSqlError( SQL_HANDLE_STMT, d->m_hstmt ) );
            return Node();
        }
        rc = SQLGetDescField( hdesc, colNum, SQL_DESC_COL_LITERAL_ATTR, &flag, SQL_IS_INTEGER, 0 );
        if ( !SQL_SUCCEEDED(rc) ) {
            setError( IODBC::convertSqlError( SQL_HANDLE_STMT, d->m_hstmt ) );
            return Node();
        }
        short l_lang = (short)((flag >> 16) & 0xFFFF);
        short l_type = (short)(flag & 0xFFFF);
        rc = SQLGetDescField( hdesc, colNum, SQL_DESC_COL_BOX_FLAGS, &flag, SQL_IS_INTEGER, 0 );
        if ( !SQL_SUCCEEDED(rc) ) {
            setError( IODBC::convertSqlError( SQL_HANDLE_STMT, d->m_hstmt ) );
            return Node();
        }

//         qDebug() << "flag:" << flag;
//         qDebug() << "data:" << ( const char* )data;

        switch (dvtype) {
        case DV_STRING: {
            if (flag) {
                if ( strncmp( (char*)data, "_:", 2 ) == 0 ) {
                    node = Node( QString::fromLatin1( reinterpret_cast<const char*>( data )+2 ) );
                }
                else {
                    node = Node( QUrl::fromEncoded( reinterpret_cast<const char*>( data ), QUrl::StrictMode ) );
                }
            }
            else {
                if ( strncmp( (char*)data, "nodeID://", 9 ) == 0 ) {
                    node = Node( QString::fromLatin1( reinterpret_cast<const char*>( data )+9 ) );
                }
                else {
                    node = Node( LiteralValue( QString::fromUtf8( reinterpret_cast<const char*>( data ) ) ) );
                }
            }
            break;
        }
        case DV_RDF: {
            QUrl type = d->getType( l_type );
            QString lang = d->getLang( l_lang );
            node = Node( LiteralValue::fromString( QString::fromUtf8( reinterpret_cast<const char*>( data ) ), type ), lang );
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
            node = LiteralValue::fromString( QString::fromUtf8( reinterpret_cast<const char*>( data ) ), type );
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
        setError( IODBC::convertSqlError( SQL_HANDLE_STMT, d->m_hstmt ) );
        return Node();
    }
}
