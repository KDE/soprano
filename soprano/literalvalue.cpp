/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007-2011 Sebastian Trueg <trueg@kde.org>
 * Copyright (C) 2009 Greg Beauchesne <greg_b@vision-play.com>
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

#include "literalvalue.h"
#include "datetime.h"
#include "xsd.h"
#include "rdf.h"
#include "node.h" // for qHash(QUrl)

#include <QtCore/QUrl>
#include <QtCore/QString>
#include <QtCore/QHash>
#include <QtCore/QDebug>
#include <QtCore/QSharedData>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>


namespace {
/**
 * Helper class for thread-safe conversion between QUrl and QVariant::Type.
 */
class XsdTypePool
{
public:
    XsdTypePool();

    QVariant::Type typeFromDataTypeUri( const QUrl& dataTypeUri ) const;
    QUrl dataTypeUriFromType( QVariant::Type type ) const;

private:
    mutable QMutex m_xsdMutex;
    mutable QMutex m_variantMutex;
    QHash<QUrl, QVariant::Type> m_xsdHash;
    QHash<int, QUrl> m_variantHash;
};

XsdTypePool::XsdTypePool()
{
    m_xsdHash.insert( Soprano::Vocabulary::XMLSchema::xsdInt(), QVariant::Int );
    m_xsdHash.insert( Soprano::Vocabulary::XMLSchema::integer(), QVariant::Int );
    m_xsdHash.insert( Soprano::Vocabulary::XMLSchema::negativeInteger(), QVariant::Int );
    m_xsdHash.insert( Soprano::Vocabulary::XMLSchema::nonNegativeInteger(), QVariant::UInt );
    m_xsdHash.insert( Soprano::Vocabulary::XMLSchema::decimal(), QVariant::Int );
    m_xsdHash.insert( Soprano::Vocabulary::XMLSchema::xsdShort(), QVariant::Int );
    m_xsdHash.insert( Soprano::Vocabulary::XMLSchema::xsdLong(), QVariant::LongLong );
    m_xsdHash.insert( Soprano::Vocabulary::XMLSchema::unsignedInt(), QVariant::UInt );
    m_xsdHash.insert( Soprano::Vocabulary::XMLSchema::unsignedShort(), QVariant::UInt );
    m_xsdHash.insert( Soprano::Vocabulary::XMLSchema::unsignedLong(), QVariant::ULongLong );
    m_xsdHash.insert( Soprano::Vocabulary::XMLSchema::boolean(), QVariant::Bool );
    m_xsdHash.insert( Soprano::Vocabulary::XMLSchema::xsdDouble(), QVariant::Double );
    m_xsdHash.insert( Soprano::Vocabulary::XMLSchema::xsdFloat(), QVariant::Double );
    m_xsdHash.insert( Soprano::Vocabulary::XMLSchema::string(), QVariant::String );
    m_xsdHash.insert( Soprano::Vocabulary::XMLSchema::date(), QVariant::Date );
    m_xsdHash.insert( Soprano::Vocabulary::XMLSchema::time(), QVariant::Time );
    m_xsdHash.insert( Soprano::Vocabulary::XMLSchema::dateTime(), QVariant::DateTime );
    m_xsdHash.insert( Soprano::Vocabulary::XMLSchema::base64Binary(), QVariant::ByteArray );
    m_xsdHash.insert( Soprano::Vocabulary::RDF::XMLLiteral(), QVariant::String );

    m_variantHash.insert( QVariant::Int, Soprano::Vocabulary::XMLSchema::xsdInt() );
    m_variantHash.insert( QVariant::LongLong, Soprano::Vocabulary::XMLSchema::xsdLong() );
    m_variantHash.insert( QVariant::UInt, Soprano::Vocabulary::XMLSchema::unsignedInt() );
    m_variantHash.insert( QVariant::ULongLong, Soprano::Vocabulary::XMLSchema::unsignedLong() );
    m_variantHash.insert( QVariant::Bool, Soprano::Vocabulary::XMLSchema::boolean() );
    m_variantHash.insert( QVariant::Double, Soprano::Vocabulary::XMLSchema::xsdDouble() );
    m_variantHash.insert( QVariant::String, Soprano::Vocabulary::XMLSchema::string() );
    m_variantHash.insert( QVariant::Date, Soprano::Vocabulary::XMLSchema::date() );
    m_variantHash.insert( QVariant::Time, Soprano::Vocabulary::XMLSchema::time() );
    m_variantHash.insert( QVariant::DateTime, Soprano::Vocabulary::XMLSchema::dateTime() );
    m_variantHash.insert( QVariant::ByteArray, Soprano::Vocabulary::XMLSchema::base64Binary() );
}

QVariant::Type XsdTypePool::typeFromDataTypeUri( const QUrl& dataTypeUri ) const
{
    QMutexLocker lock( &m_xsdMutex );
    QHash<QUrl, QVariant::Type>::const_iterator it = m_xsdHash.constFind( dataTypeUri );
    if ( it != m_xsdHash.constEnd() ) {
        return it.value();
    }
    else {
//        qDebug() << "(Soprano::LiteralValue) unknown literal type uri:" << dataTypeUri;
        return QVariant::Invalid;
    }
}

QUrl XsdTypePool::dataTypeUriFromType( QVariant::Type type ) const
{
    QMutexLocker lock( &m_variantMutex );
    return m_variantHash[type];
}

Q_GLOBAL_STATIC( XsdTypePool, s_xsdTypePool )
}


class Soprano::LiteralValue::LiteralValueData : public QSharedData
{
public:
    LiteralValueData()
        : stringCacheValid( false ) {
    }

    LiteralValueData( const QVariant& v )
        : value( v ),
          stringCacheValid( false ) {
    }

    virtual ~LiteralValueData() {
    }

    QVariant value;

    virtual bool isPlain() const = 0;
    virtual LanguageTag language() const { return LanguageTag(); }
    virtual QUrl dataTypeUri() const { return QUrl(); }

    mutable QString stringCache;
    mutable bool stringCacheValid;
};

class Soprano::LiteralValue::PlainData : public Soprano::LiteralValue::LiteralValueData
{
public:
    PlainData( const QString& v, const LanguageTag& l )
        : LiteralValueData( v ),
          lang( l ) {
        stringCache = v;
        stringCacheValid = true;
    }

    virtual bool isPlain() const { return true; }
    virtual LanguageTag language() const { return lang; }

    LanguageTag lang;
};

class Soprano::LiteralValue::TypedData : public Soprano::LiteralValue::LiteralValueData
{
public:
    TypedData( const QVariant & value )
        : LiteralValueData(value) {
    }

    virtual bool isPlain() const { return false; }
    virtual QUrl dataTypeUri() const { return dtUri; }

    mutable QUrl dtUri;
};

Soprano::LiteralValue::LiteralValue()
    : d( 0 )
{
}


Soprano::LiteralValue::~LiteralValue()
{
}


Soprano::LiteralValue::LiteralValue( const LiteralValue& other )
{
    d = other.d;
}


Soprano::LiteralValue::LiteralValue( const QVariant& other )
    : d( 0 )
{
    QUrl type = dataTypeUriFromType( other.type() );
    if ( !type.isEmpty() ) {
        TypedData* td = new TypedData(other);
        td->value = other;
        td->dtUri = type;
        d = td;
    }
}


Soprano::LiteralValue::LiteralValue( int i )
    : d( new TypedData( i ) )
{
}


Soprano::LiteralValue::LiteralValue( qlonglong i )
    : d( new TypedData( i ) )
{
}


Soprano::LiteralValue::LiteralValue( uint i )
    : d( new TypedData( i ) )
{
}


Soprano::LiteralValue::LiteralValue( qulonglong i )
    : d( new TypedData( i ) )
{
}


Soprano::LiteralValue::LiteralValue( bool b )
    : d( new TypedData( b ) )
{
}


Soprano::LiteralValue::LiteralValue( double d )
    : d( new TypedData( d ) )
{
}


Soprano::LiteralValue::LiteralValue( const char* string )
    : d( new TypedData( QString::fromUtf8( string ) ) )
{
}


Soprano::LiteralValue::LiteralValue( const QLatin1String& string )
    : d( new TypedData( string ) )
{
}


Soprano::LiteralValue::LiteralValue( const QString& string )
    : d( new TypedData( string ) )
{
}


Soprano::LiteralValue::LiteralValue( const QDate& date )
    : d( new TypedData( date ) )
{
}


Soprano::LiteralValue::LiteralValue( const QTime& time )
    : d( new TypedData( time ) )
{
}


Soprano::LiteralValue::LiteralValue( const QDateTime& datetime )
    : d( new TypedData( datetime.toUTC() ) )
{
}


Soprano::LiteralValue::LiteralValue( const QByteArray& data )
    : d( new TypedData( data ) )
{
}


bool Soprano::LiteralValue::isValid() const
{
    return d ? d->value.isValid() : false;
}


bool Soprano::LiteralValue::isPlain() const
{
    return d ? d->isPlain() : false;
}

Soprano::LiteralValue& Soprano::LiteralValue::operator=( const LiteralValue& v )
{
    d = v.d;
    return *this;
}


Soprano::LiteralValue& Soprano::LiteralValue::operator=( int i )
{
    QUrl dtUri( dataTypeUri() );
    TypedData* td = new TypedData(i);
    if ( typeFromDataTypeUri( dtUri ) == QVariant::Int ) {
        td->dtUri = dtUri;
    }
    d = td;
    return *this;
}


Soprano::LiteralValue& Soprano::LiteralValue::operator=( qlonglong i )
{
    d = new TypedData(i);
    return *this;
}


Soprano::LiteralValue& Soprano::LiteralValue::operator=( uint i )
{
    QUrl dtUri( dataTypeUri() );
    TypedData* td = new TypedData(i);
    if ( typeFromDataTypeUri( dtUri ) == QVariant::UInt ) {
        td->dtUri = dtUri;
    }
    d = td;
    return *this;
}


Soprano::LiteralValue& Soprano::LiteralValue::operator=( qulonglong i )
{
    d = new TypedData(i);
    return *this;
}


Soprano::LiteralValue& Soprano::LiteralValue::operator=( bool b )
{
    d = new TypedData(b);
    return *this;
}


Soprano::LiteralValue& Soprano::LiteralValue::operator=( double v )
{
    d = new TypedData(v);
    return *this;
}


Soprano::LiteralValue& Soprano::LiteralValue::operator=( const QString& string )
{
    TypedData * td = new TypedData(string);
    td->stringCacheValid = true;
    td->stringCache = string;
    d = td;
    return *this;
}


Soprano::LiteralValue& Soprano::LiteralValue::operator=( const QLatin1String& string )
{
    return operator=( QString::fromLatin1( string.latin1() ) );
}


Soprano::LiteralValue& Soprano::LiteralValue::operator=( const QDate& date )
{
    d = new TypedData(date);
    return *this;
}


Soprano::LiteralValue& Soprano::LiteralValue::operator=( const QTime& time )
{
    d = new TypedData(time);
    return *this;
}


Soprano::LiteralValue& Soprano::LiteralValue::operator=( const QDateTime& datetime )
{
    d = new TypedData(datetime);
    return *this;
}


Soprano::LiteralValue& Soprano::LiteralValue::operator=( const QByteArray& data )
{
    d = new TypedData(data);
    return *this;
}


bool Soprano::LiteralValue::isInt() const
{
    return( type() == QVariant::Int );
}


bool Soprano::LiteralValue::isInt64() const
{
    return( type() == QVariant::LongLong );
}


bool Soprano::LiteralValue::isUnsignedInt() const
{
    return( type() == QVariant::UInt );
}


bool Soprano::LiteralValue::isUnsignedInt64() const
{
    return( type() == QVariant::ULongLong );
}


bool Soprano::LiteralValue::isBool() const
{
    return( type() == QVariant::Bool );
}


bool Soprano::LiteralValue::isDouble() const
{
    return( type() == QVariant::Double );
}


bool Soprano::LiteralValue::isString() const
{
    return( type() == QVariant::String );
}


bool Soprano::LiteralValue::isDate() const
{
    return( type() == QVariant::Date );
}


bool Soprano::LiteralValue::isTime() const
{
    return( type() == QVariant::Time );
}


bool Soprano::LiteralValue::isDateTime() const
{
    return( type() == QVariant::DateTime );
}


bool Soprano::LiteralValue::isByteArray() const
{
    return( type() == QVariant::ByteArray );
}


int Soprano::LiteralValue::toInt() const
{
    return d ? d->value.toInt() : 0;
}


qlonglong Soprano::LiteralValue::toInt64() const
{
    return d ? d->value.toLongLong() : 0L;
}


uint Soprano::LiteralValue::toUnsignedInt() const
{
    return d ? d->value.toUInt() : 0U;
}


qulonglong Soprano::LiteralValue::toUnsignedInt64() const
{
    return d ? d->value.toULongLong() : 0UL;
}


bool Soprano::LiteralValue::toBool() const
{
    return d ? d->value.toBool() : false;
}


double Soprano::LiteralValue::toDouble() const
{
    return d ? d->value.toDouble() : 0.0;
}


// here we use DataTime instead of QT's built-in time conversion
// since the latter does not properly handle fractions of seconds
// in the ISO8601 specification.
QString Soprano::LiteralValue::toString() const
{
    if ( d ) {
        if ( !d->stringCacheValid ) {
            if( isInt() )
                d->stringCache = QString::number( toInt() );
            else if( isInt64() )
                d->stringCache = QString::number( toInt64() );
            else if( isUnsignedInt() )
                d->stringCache = QString::number( toUnsignedInt() );
            else if( isUnsignedInt64() )
                d->stringCache = QString::number( toUnsignedInt64() );
            else if( isBool() )
                d->stringCache = ( toBool() ? QString("true") : QString("false" ) );
            else if( isDouble() ) // FIXME: decide on a proper double encoding or check if there is one in xml schema
                d->stringCache = QString::number( toDouble(), 'e', 10 );
            else if( isDate() )
                d->stringCache = DateTime::toString( toDate() );
            else if( isTime() )
                d->stringCache = DateTime::toString( toTime() );
            else if( isDateTime() )
                d->stringCache = DateTime::toString( toDateTime() );
            else if ( isByteArray() )
                d->stringCache = QString::fromLatin1( toByteArray().toBase64() );
            else
                d->stringCache = d->value.toString();

            d->stringCacheValid = true;
        }

        return d->stringCache;
    }
    else {
        return QString();
    }
}


QDate Soprano::LiteralValue::toDate() const
{
    if ( isDate() ) {
        // QVariant::toDate does use the simple ISO format
        return d->value.toDate();
    }
    else {
        return DateTime::fromDateString( toString() );
    }
}


QTime Soprano::LiteralValue::toTime() const
{
    if ( isTime() ) {
        // QVariant::toTime does use the simple ISO format
        return d->value.toTime();
    }
    else {
        return DateTime::fromTimeString( toString() );
    }
}


QDateTime Soprano::LiteralValue::toDateTime() const
{
    if ( isDateTime() ) {
        // QVariant::toDateTime does use the simple ISO format
        return d->value.toDateTime();
    }
    else {
        return DateTime::fromDateTimeString( toString() );
    }
}


QByteArray Soprano::LiteralValue::toByteArray() const
{
    return d ? d->value.toByteArray() : QByteArray();
}


QVariant::Type Soprano::LiteralValue::type() const
{
    return d ? d->value.type() : QVariant::Invalid;
}


QVariant Soprano::LiteralValue::variant() const
{
    return d ? d->value : QVariant();
}


QUrl Soprano::LiteralValue::dataTypeUri() const
{
    if ( d ) {
        if ( !d->isPlain() && d->dataTypeUri().isEmpty() ) {
            static_cast<const TypedData*>(d.constData())->dtUri = LiteralValue::dataTypeUriFromType( type() );
        }
        return d->dataTypeUri();
    }
    else {
        return QUrl();
    }
}

Soprano::LanguageTag Soprano::LiteralValue::language() const
{
    return d ? d->language() : LanguageTag();
}


bool Soprano::LiteralValue::operator==( const LiteralValue& other ) const
{
    if ( variant() != other.variant() ||
         isPlain() != other.isPlain() )
        return false;

    if ( isPlain() ) {
        return language() == other.language();
    }
    else {
        return dataTypeUri() == other.dataTypeUri();
    }
}


bool Soprano::LiteralValue::operator!=( const LiteralValue& other ) const
{
    if ( variant() != other.variant() ||
         isPlain() != other.isPlain() )
        return true;

    if ( isPlain() ) {
        return language() != other.language();
    }
    else {
        return dataTypeUri() != other.dataTypeUri();
    }
}


Soprano::LiteralValue Soprano::LiteralValue::fromString( const QString& value, QVariant::Type type )
{
    switch( type ) {
    case QVariant::Int: {
        bool ok = false;
        int val = value.toInt(&ok);
        if(ok)
            return LiteralValue( val );
        else
            return LiteralValue();
    }
    case QVariant::LongLong: {
        bool ok = false;
        qlonglong val = value.toLongLong(&ok);
        if(ok)
            return LiteralValue( val );
        else
            return LiteralValue();
    }
    case QVariant::UInt: {
        bool ok = false;
        uint val = value.toUInt(&ok);
        if(ok)
            return LiteralValue( val );
        else
            return LiteralValue();
    }
    case QVariant::ULongLong: {
        bool ok = false;
        qulonglong val = value.toULongLong(&ok);
        if(ok)
            return LiteralValue( val );
        else
            return LiteralValue();
    }
    case QVariant::Double: {
        bool ok = false;
        double val = value.toDouble(&ok);
        if(ok)
            return LiteralValue( val );
        else
            return LiteralValue();
    }
    case QVariant::Date: {
        QDate date = DateTime::fromDateString( value );
        if( date.isValid() )
            return LiteralValue( date );
        else
            return LiteralValue();
    }
    case QVariant::Time: {
        QTime date = DateTime::fromTimeString( value );
        if( date.isValid() )
            return LiteralValue( date );
        else
            return LiteralValue();
    }
    case QVariant::DateTime: {
        QDateTime date = DateTime::fromDateTimeString( value );
        if( date.isValid() )
            return LiteralValue( date );
        else
            return LiteralValue();
    }
    case QVariant::Bool: {
        bool ok = false;
        int v = value.toInt(&ok);
        if(ok)
            return LiteralValue(v != 0);
        else if( value.toLower() == "true" || value.toLower() == "yes" )
            return LiteralValue( true );
        else if( value.toLower() == "false" || value.toLower() == "no" )
            return LiteralValue( false );
        else
            return LiteralValue();
    }
    case QVariant::ByteArray:
        return LiteralValue( QByteArray::fromBase64( value.toLatin1() ) );
    case QVariant::String:
        return LiteralValue( value );
    default:
//        qDebug() << "(Soprano::LiteralValue) unknown type: " << type << "storing as string value." << endl;
        return LiteralValue( value );
    }
}


Soprano::LiteralValue Soprano::LiteralValue::fromString( const QString& value, const QUrl& type )
{
    if ( type.isEmpty() && value.isEmpty() ) {
        return LiteralValue();
    }
    else {
        LiteralValue v = LiteralValue::fromString( value, typeFromDataTypeUri( type ) );
        Q_ASSERT(!v.isPlain());
        if( v.isValid() ) {
            static_cast<const TypedData*>(v.d.constData())->dtUri = type;
        }
        return v;
    }
}


Soprano::LiteralValue Soprano::LiteralValue::fromVariant( const QVariant& value, const QUrl& dataType )
{
    //
    // Special case: time_t -> datetime
    //
    if( dataType == Vocabulary::XMLSchema::dateTime() &&
        value.canConvert(QVariant::Int) ) {
        bool ok = false;
        int time = value.toInt(&ok);
        if(ok) {
            return Soprano::LiteralValue( QDateTime::fromTime_t(time) );
        }
    }

    const QVariant::Type literalType = typeFromDataTypeUri( dataType );
    if( value.canConvert(literalType) ) {
        LiteralValue v;
        bool ok = false;

        //
        // We map 5 different XSD types to Int.
        // Thus, we need to handle them separately since the constructor will fall
        // back to xsd:int
        //
        if( literalType == QVariant::Int ) {
            v = value.toInt(&ok);
        }

        //
        // Handle xsd:unsignedInt and xsd:nonNegativeInteger
        //
        else if( literalType == QVariant::UInt ) {
            v = value.toUInt(&ok);
        }

        //
        // Handle xsd:float and xsd:double
        //
        else if( literalType == QVariant::Double ) {
            v = value.toDouble(&ok);
        }

        //
        // The perfect match
        //
        else if( value.type() == literalType ) {
            v = value;
            ok = true;
        }

        if( v.isValid() && ok ) {
            // fixup the datatype
            static_cast<const TypedData*>(v.d.constData())->dtUri = dataType;
            return v;
        }
    }

    // fallback
    return fromString(value.toString(), dataType);
}


Soprano::LiteralValue Soprano::LiteralValue::createPlainLiteral( const QString& value, const LanguageTag& lang )
{
    LiteralValue v;
    v.d = new PlainData( value, lang );
    return v;
}


QVariant::Type Soprano::LiteralValue::typeFromDataTypeUri( const QUrl& dataTypeUri )
{
    return s_xsdTypePool()->typeFromDataTypeUri(dataTypeUri);
}


QUrl Soprano::LiteralValue::dataTypeUriFromType( QVariant::Type type )
{
    return s_xsdTypePool()->dataTypeUriFromType(type);
}


QDebug operator<<( QDebug dbg, const Soprano::LiteralValue& v )
{
    dbg << v.toString();
    return dbg;
}

uint Soprano::qHash( const LiteralValue& lit )
{
    if (lit.isPlain()) {
        return ~(qHash( lit.toString() ) ^ qHash( lit.language() ));
    } else {
        return qHash( lit.toString() ) ^ qHash( lit.dataTypeUri() );
    }
}
