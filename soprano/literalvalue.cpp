/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007 Sebastian Trueg <trueg@kde.org>
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

// Uncomment this to disable the union typing
//#define DISABLE_SOPRANO_LITERALVALUE_UNION


class Soprano::LiteralValue::Private : public QSharedData
{
public:
    Private()
        : stringCacheValid( false ),
         plainLiteral( true ) {
#ifndef DISABLE_SOPRANO_LITERALVALUE_UNION
        new (&uriLangUnion) Soprano::LanguageTag();
#endif /* DISABLE_SOPRANO_LITERALVALUE_UNION */
    }

    Private( const QVariant& v )
        : value( v ),
          stringCacheValid( false ),
          plainLiteral( false ) {
#ifndef DISABLE_SOPRANO_LITERALVALUE_UNION
        new (&uriLangUnion) QUrl();
#endif /* DISABLE_SOPRANO_LITERALVALUE_UNION */
    }

    ~Private()
    {
#ifndef DISABLE_SOPRANO_LITERALVALUE_UNION
        destroyData();
#endif /* DISABLE_SOPRANO_LITERALVALUE_UNION */
    }

    QVariant value;

    void setLanguage( const LanguageTag& lang );
    void setDataTypeUri( const QUrl& uri ) const;

    LanguageTag language() const;
    QUrl dataTypeUri() const;

    mutable QString stringCache;

    mutable bool stringCacheValid;
    mutable bool plainLiteral;
private:
    // Poor man's union of QUrl and Soprano::LanguageTag that works for non-POD types
#ifndef DISABLE_SOPRANO_LITERALVALUE_UNION
    mutable void * uriLangUnion;
    void destroyData() const;
#else
    mutable QUrl dataTypeUriValue;
    mutable Soprano::LanguageTag languageTag;
#endif /* DISABLE_SOPRANO_LITERALVALUE_UNION */

};

#ifndef DISABLE_SOPRANO_LITERALVALUE_UNION
// Static assertions; causes a compiler error if violated
static char assert_size[
    sizeof(QUrl) == sizeof(void *)
    && sizeof(Soprano::LanguageTag) == sizeof(void *) ? 1 : -1
];

void Soprano::LiteralValue::Private::destroyData() const
{
    if (plainLiteral) {
        reinterpret_cast<Soprano::LanguageTag *>(&uriLangUnion)->Soprano::LanguageTag::~LanguageTag();
    } else {
        reinterpret_cast<QUrl *>(&uriLangUnion)->~QUrl();
    }
}
#endif /* DISABLE_SOPRANO_LITERALVALUE_UNION */

void Soprano::LiteralValue::Private::setLanguage( const Soprano::LanguageTag& lang )
{
#ifndef DISABLE_SOPRANO_LITERALVALUE_UNION
    destroyData();
    new (&uriLangUnion) Soprano::LanguageTag(lang);
#else
    languageTag = lang;
    dataTypeUriValue = QUrl();
#endif /* DISABLE_SOPRANO_LITERALVALUE_UNION */
    plainLiteral = true;
}

void Soprano::LiteralValue::Private::setDataTypeUri( const QUrl& uri ) const
{
#ifndef DISABLE_SOPRANO_LITERALVALUE_UNION
    destroyData();
    new (&uriLangUnion) QUrl(uri);
#else
    languageTag = Soprano::LanguageTag();
    dataTypeUriValue = uri;
#endif /* DISABLE_SOPRANO_LITERALVALUE_UNION */
    plainLiteral = false;
}

Soprano::LanguageTag Soprano::LiteralValue::Private::language() const
{
#ifndef DISABLE_SOPRANO_LITERALVALUE_UNION
    return plainLiteral
        ? *reinterpret_cast<Soprano::LanguageTag *>(&uriLangUnion)
        : Soprano::LanguageTag();
#else
    return languageTag;
#endif /* DISABLE_SOPRANO_LITERALVALUE_UNION */
}

QUrl Soprano::LiteralValue::Private::dataTypeUri() const
{
#ifndef DISABLE_SOPRANO_LITERALVALUE_UNION
    return plainLiteral
        ? QUrl()
        : *reinterpret_cast<QUrl *>(&uriLangUnion);
#else
    return dataTypeUriValue;
#endif /* DISABLE_SOPRANO_LITERALVALUE_UNION */
}

Soprano::LiteralValue::LiteralValue()
    : d( new Private )
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
    : d( new Private() )
{
    QUrl type = dataTypeUriFromType( other.type() );
    if ( !type.isEmpty() ) {
        d->value = other;
        d->setDataTypeUri(type);
    }
}


Soprano::LiteralValue::LiteralValue( int i )
    : d( new Private( i ) )
{
}


Soprano::LiteralValue::LiteralValue( qlonglong i )
    : d( new Private( i ) )
{
}


Soprano::LiteralValue::LiteralValue( uint i )
    : d( new Private( i ) )
{
}


Soprano::LiteralValue::LiteralValue( qulonglong i )
    : d( new Private( i ) )
{
}


Soprano::LiteralValue::LiteralValue( bool b )
    : d( new Private( b ) )
{
}


Soprano::LiteralValue::LiteralValue( double d )
    : d( new Private( d ) )
{
}


Soprano::LiteralValue::LiteralValue( const char* string )
    : d( new Private( QString::fromUtf8( string ) ) )
{
}


Soprano::LiteralValue::LiteralValue( const QLatin1String& string )
    : d( new Private( string ) )
{
}


Soprano::LiteralValue::LiteralValue( const QString& string )
    : d( new Private( string ) )
{
}


Soprano::LiteralValue::LiteralValue( const QDate& date )
    : d( new Private( date ) )
{
}


Soprano::LiteralValue::LiteralValue( const QTime& time )
    : d( new Private( time ) )
{
}


Soprano::LiteralValue::LiteralValue( const QDateTime& datetime )
    : d( new Private( datetime.toUTC() ) )
{
}


Soprano::LiteralValue::LiteralValue( const QByteArray& data )
    : d( new Private( data ) )
{
}


bool Soprano::LiteralValue::isValid() const
{
    return d->value.isValid();
}

bool Soprano::LiteralValue::isPlain() const
{
    return d->plainLiteral;
}

Soprano::LiteralValue& Soprano::LiteralValue::operator=( const LiteralValue& v )
{
    d = v.d;
    return *this;
}


Soprano::LiteralValue& Soprano::LiteralValue::operator=( int i )
{
    d->stringCacheValid = false;
    d->value.setValue( i );
    if ( typeFromDataTypeUri( d->dataTypeUri() ) != QVariant::Int ) {
        d->setDataTypeUri(QUrl());
    }
    return *this;
}


Soprano::LiteralValue& Soprano::LiteralValue::operator=( qlonglong i )
{
    d->stringCacheValid = false;
    d->value.setValue( i );
    d->setDataTypeUri(QUrl());
    return *this;
}


Soprano::LiteralValue& Soprano::LiteralValue::operator=( uint i )
{
    d->stringCacheValid = false;
    d->value.setValue( i );
    if ( typeFromDataTypeUri( d->dataTypeUri() ) != QVariant::UInt ) {
        d->setDataTypeUri(QUrl());
    }
    return *this;
}


Soprano::LiteralValue& Soprano::LiteralValue::operator=( qulonglong i )
{
    d->stringCacheValid = false;
    d->value.setValue( i );
    d->setDataTypeUri(QUrl());
    return *this;
}


Soprano::LiteralValue& Soprano::LiteralValue::operator=( bool b )
{
    d->stringCacheValid = false;
    d->value.setValue( b );
    d->setDataTypeUri(QUrl());
    return *this;
}


Soprano::LiteralValue& Soprano::LiteralValue::operator=( double v )
{
    d->stringCacheValid = false;
    d->value.setValue( v );
    d->setDataTypeUri(QUrl());
    return *this;
}


Soprano::LiteralValue& Soprano::LiteralValue::operator=( const QString& string )
{
    d->stringCacheValid = true;
    d->stringCache = string;
    d->value.setValue( string );
    d->setDataTypeUri(QUrl());
    return *this;
}


Soprano::LiteralValue& Soprano::LiteralValue::operator=( const QLatin1String& string )
{
    return operator=( QString::fromLatin1( string.latin1() ) );
}


Soprano::LiteralValue& Soprano::LiteralValue::operator=( const QDate& date )
{
    d->stringCacheValid = false;
    d->value.setValue( date );
    d->setDataTypeUri(QUrl());
    return *this;
}


Soprano::LiteralValue& Soprano::LiteralValue::operator=( const QTime& time )
{
    d->stringCacheValid = false;
    d->value.setValue( time );
    d->setDataTypeUri(QUrl());
    return *this;
}


Soprano::LiteralValue& Soprano::LiteralValue::operator=( const QDateTime& datetime )
{
    d->stringCacheValid = false;
    d->value.setValue( datetime.toUTC() );
    d->setDataTypeUri(QUrl());
    return *this;
}


Soprano::LiteralValue& Soprano::LiteralValue::operator=( const QByteArray& data )
{
    d->stringCacheValid = false;
    d->value.setValue( data );
    d->setDataTypeUri(QUrl());
    return *this;
}


bool Soprano::LiteralValue::isInt() const
{
    return( d->value.type() == QVariant::Int );
}


bool Soprano::LiteralValue::isInt64() const
{
    return( d->value.type() == QVariant::LongLong );
}


bool Soprano::LiteralValue::isUnsignedInt() const
{
    return( d->value.type() == QVariant::UInt );
}


bool Soprano::LiteralValue::isUnsignedInt64() const
{
    return( d->value.type() == QVariant::ULongLong );
}


bool Soprano::LiteralValue::isBool() const
{
    return( d->value.type() == QVariant::Bool );
}


bool Soprano::LiteralValue::isDouble() const
{
    return( d->value.type() == QVariant::Double );
}


bool Soprano::LiteralValue::isString() const
{
    return( d->value.type() == QVariant::String );
}


bool Soprano::LiteralValue::isDate() const
{
    return( d->value.type() == QVariant::Date );
}


bool Soprano::LiteralValue::isTime() const
{
    return( d->value.type() == QVariant::Time );
}


bool Soprano::LiteralValue::isDateTime() const
{
    return( d->value.type() == QVariant::DateTime );
}


bool Soprano::LiteralValue::isByteArray() const
{
    return( d->value.type() == QVariant::ByteArray );
}


int Soprano::LiteralValue::toInt() const
{
    return d->value.toInt();
}


qlonglong Soprano::LiteralValue::toInt64() const
{
    return d->value.toLongLong();
}


uint Soprano::LiteralValue::toUnsignedInt() const
{
    return d->value.toUInt();
}


qulonglong Soprano::LiteralValue::toUnsignedInt64() const
{
    return d->value.toULongLong();
}


bool Soprano::LiteralValue::toBool() const
{
    return d->value.toBool();
}


double Soprano::LiteralValue::toDouble() const
{
    return d->value.toDouble();
}


// here we use DataTime instead of QT's built-in time conversion
// since the latter does not properly handle fractions of seconds
// in the ISO8601 specification.
QString Soprano::LiteralValue::toString() const
{
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
    return d->value.toByteArray();
}


QVariant::Type Soprano::LiteralValue::type() const
{
    return d->value.type();
}


QVariant Soprano::LiteralValue::variant() const
{
    return d->value;
}


QUrl Soprano::LiteralValue::dataTypeUri() const
{
    if ( !d->plainLiteral && d->dataTypeUri().isEmpty() ) {
        d.constData()->setDataTypeUri(LiteralValue::dataTypeUriFromType( type() ));
    }
    return d->dataTypeUri();
}

Soprano::LanguageTag Soprano::LiteralValue::language() const
{
    return d->language();
}


bool Soprano::LiteralValue::operator==( const LiteralValue& other ) const
{
    if ( d->value != other.d->value ||
         d->plainLiteral != other.d->plainLiteral ) return false;

    if ( d->plainLiteral ) {
        return language() == other.language();
    } else {
        return dataTypeUri() == other.dataTypeUri();
    }
}


bool Soprano::LiteralValue::operator!=( const LiteralValue& other ) const
{
    if ( d->value != other.d->value ||
         d->plainLiteral != other.d->plainLiteral ) return true;

    if ( d->plainLiteral ) {
        return language() != other.language();
    } else {
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
        v.d->setDataTypeUri(type);
        return v;
    }
}

Soprano::LiteralValue Soprano::LiteralValue::createPlainLiteral( const QString& value, const LanguageTag& lang )
{
    if ( value.isEmpty() && lang.isEmpty() ) {
        return LiteralValue();
    }
    else {
        LiteralValue v;
        v.d->value = v.d->stringCache = value;
        v.d->stringCacheValid = true;
        v.d->setLanguage( lang );
        return v;
    }
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
