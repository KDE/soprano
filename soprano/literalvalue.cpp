/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007-2009 Sebastian Trueg <trueg@kde.org>
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
                d->stringCache = QString::fromAscii( toByteArray().toBase64() );
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
    case QVariant::Int:
        return LiteralValue( value.toInt() );
    case QVariant::LongLong:
        return LiteralValue( value.toLongLong() );
    case QVariant::UInt:
        return LiteralValue( value.toUInt() );
    case QVariant::ULongLong:
        return LiteralValue( value.toULongLong() );
    case QVariant::Bool:
        return LiteralValue( ( value.toLower() == "true" || value.toLower() == "yes" || value.toInt() != 0 ) );
    case QVariant::Double:
        return LiteralValue( value.toDouble() );
    case QVariant::String:
        return LiteralValue( value );
    case QVariant::Date:
        return LiteralValue( DateTime::fromDateString( value ) );
    case QVariant::Time:
        return LiteralValue( DateTime::fromTimeString( value ) );
    case QVariant::DateTime:
        return LiteralValue( DateTime::fromDateTimeString( value ) );
    case QVariant::ByteArray:
        return LiteralValue( QByteArray::fromBase64( value.toAscii() ) );
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
        static_cast<const TypedData*>(v.d.constData())->dtUri = type;
        return v;
    }
}

Soprano::LiteralValue Soprano::LiteralValue::createPlainLiteral( const QString& value, const LanguageTag& lang )
{
    LiteralValue v;
    v.d = new PlainData( value, lang );
    return v;
}


QVariant::Type Soprano::LiteralValue::typeFromDataTypeUri( const QUrl& dataTypeUri )
{
    static QHash<QUrl, QVariant::Type> s_xsdTypes;
    if( s_xsdTypes.isEmpty() ) {
        s_xsdTypes.insert( Vocabulary::XMLSchema::xsdInt(), QVariant::Int );
        s_xsdTypes.insert( Vocabulary::XMLSchema::integer(), QVariant::Int );
        s_xsdTypes.insert( Vocabulary::XMLSchema::negativeInteger(), QVariant::Int );
        s_xsdTypes.insert( Vocabulary::XMLSchema::nonNegativeInteger(), QVariant::UInt );
        s_xsdTypes.insert( Vocabulary::XMLSchema::decimal(), QVariant::Int );
        s_xsdTypes.insert( Vocabulary::XMLSchema::xsdShort(), QVariant::Int );
        s_xsdTypes.insert( Vocabulary::XMLSchema::xsdLong(), QVariant::LongLong );
        s_xsdTypes.insert( Vocabulary::XMLSchema::unsignedInt(), QVariant::UInt );
        s_xsdTypes.insert( Vocabulary::XMLSchema::unsignedShort(), QVariant::UInt );
        s_xsdTypes.insert( Vocabulary::XMLSchema::unsignedLong(), QVariant::ULongLong );
        s_xsdTypes.insert( Vocabulary::XMLSchema::boolean(), QVariant::Bool );
        s_xsdTypes.insert( Vocabulary::XMLSchema::xsdDouble(), QVariant::Double );
        s_xsdTypes.insert( Vocabulary::XMLSchema::xsdFloat(), QVariant::Double );
        s_xsdTypes.insert( Vocabulary::XMLSchema::string(), QVariant::String );
        s_xsdTypes.insert( Vocabulary::XMLSchema::date(), QVariant::Date );
        s_xsdTypes.insert( Vocabulary::XMLSchema::time(), QVariant::Time );
        s_xsdTypes.insert( Vocabulary::XMLSchema::dateTime(), QVariant::DateTime );
        s_xsdTypes.insert( Vocabulary::XMLSchema::base64Binary(), QVariant::ByteArray );
        s_xsdTypes.insert( Vocabulary::RDF::XMLLiteral(), QVariant::String );
    }

    QHash<QUrl, QVariant::Type>::const_iterator it = s_xsdTypes.constFind( dataTypeUri );
    if ( it != s_xsdTypes.constEnd() ) {
        return it.value();
    }
    else {
//        qDebug() << "(Soprano::LiteralValue) unknown literal type uri:" << dataTypeUri;
        return QVariant::Invalid;
    }
}


QUrl Soprano::LiteralValue::dataTypeUriFromType( QVariant::Type type )
{
    static QHash<int, QUrl> s_variantSchemaTypeHash;
    if( s_variantSchemaTypeHash.isEmpty() ) {
        s_variantSchemaTypeHash.insert( QVariant::Int, Vocabulary::XMLSchema::xsdInt() );
        s_variantSchemaTypeHash.insert( QVariant::LongLong, Vocabulary::XMLSchema::xsdLong() );
        s_variantSchemaTypeHash.insert( QVariant::UInt, Vocabulary::XMLSchema::unsignedInt() );
        s_variantSchemaTypeHash.insert( QVariant::ULongLong, Vocabulary::XMLSchema::unsignedLong() );
        s_variantSchemaTypeHash.insert( QVariant::Bool, Vocabulary::XMLSchema::boolean() );
        s_variantSchemaTypeHash.insert( QVariant::Double, Vocabulary::XMLSchema::xsdDouble() );
        s_variantSchemaTypeHash.insert( QVariant::String, Vocabulary::XMLSchema::string() );
        s_variantSchemaTypeHash.insert( QVariant::Date, Vocabulary::XMLSchema::date() );
        s_variantSchemaTypeHash.insert( QVariant::Time, Vocabulary::XMLSchema::time() );
        s_variantSchemaTypeHash.insert( QVariant::DateTime, Vocabulary::XMLSchema::dateTime() );
        s_variantSchemaTypeHash.insert( QVariant::ByteArray, Vocabulary::XMLSchema::base64Binary() );
    }

    return s_variantSchemaTypeHash[type];
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
