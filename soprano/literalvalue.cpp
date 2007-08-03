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
#include "xmls.h"

#include <QtCore/QUrl>
#include <QtCore/QString>
#include <QtCore/QHash>
#include <QtCore/QDebug>
#include <QtCore/QSharedData>


static uint qHash( const QUrl& url )
{
    return qHash( url.toString() );
}


class Soprano::LiteralValue::Private : public QSharedData
{
public:
    QUrl dataTypeUri;
};


Soprano::LiteralValue::LiteralValue()
    : QVariant(),
      d( new Private )
{
}


Soprano::LiteralValue::~LiteralValue()
{
}


Soprano::LiteralValue::LiteralValue( const LiteralValue& other )
    : QVariant( other )
{
    d = other.d;
}


Soprano::LiteralValue::LiteralValue( const QVariant& other )
    : QVariant(),
      d( new Private() )
{
    QUrl type = dataTypeUriFromType( other.type() );
    if ( !type.isEmpty() ) {
        QVariant::operator=( other );
        d->dataTypeUri = type;
    }
}


Soprano::LiteralValue::LiteralValue( int i )
    : QVariant( i ),
      d( new Private )
{
}


Soprano::LiteralValue::LiteralValue( qlonglong i )
    : QVariant( i ),
      d( new Private )
{
}


Soprano::LiteralValue::LiteralValue( uint i )
    : QVariant( i ),
      d( new Private )
{
}


Soprano::LiteralValue::LiteralValue( qulonglong i )
    : QVariant( i ),
      d( new Private )
{
}


Soprano::LiteralValue::LiteralValue( bool b )
    : QVariant( b ),
      d( new Private )
{
}


Soprano::LiteralValue::LiteralValue( double d )
    : QVariant( d ),
      d( new Private )
{
}


Soprano::LiteralValue::LiteralValue( const char* string )
    : QVariant( QString::fromUtf8( string ) ),
      d( new Private )
{
}


Soprano::LiteralValue::LiteralValue( const QLatin1String& string )
    : QVariant( string ),
      d( new Private )
{
}


Soprano::LiteralValue::LiteralValue( const QString& string )
    : QVariant( string ),
      d( new Private )
{
}


Soprano::LiteralValue::LiteralValue( const QDate& date )
    : QVariant( date ),
      d( new Private )
{
}


Soprano::LiteralValue::LiteralValue( const QTime& time )
    : QVariant( time ),
      d( new Private )
{
}


Soprano::LiteralValue::LiteralValue( const QDateTime& datetime )
    : QVariant( datetime ),
      d( new Private )
{
}


bool Soprano::LiteralValue::isValid() const
{
    return QVariant::isValid();
}


Soprano::LiteralValue& Soprano::LiteralValue::operator=( const LiteralValue& v )
{
    d = v.d;
    QVariant::operator=( v );
    return *this;
}


Soprano::LiteralValue& Soprano::LiteralValue::operator=( int i )
{
    setValue( i );
    if ( typeFromDataTypeUri( d->dataTypeUri ) != QVariant::Int ) {
        d->dataTypeUri = QUrl();
    }
    return *this;
}


Soprano::LiteralValue& Soprano::LiteralValue::operator=( qlonglong i )
{
    d->dataTypeUri = QUrl();
    setValue( i );
    return *this;
}


Soprano::LiteralValue& Soprano::LiteralValue::operator=( uint i )
{
    setValue( i );
    if ( typeFromDataTypeUri( d->dataTypeUri ) != QVariant::UInt ) {
        d->dataTypeUri = QUrl();
    }
    return *this;
}


Soprano::LiteralValue& Soprano::LiteralValue::operator=( qulonglong i )
{
    d->dataTypeUri = QUrl();
    setValue( i );
    return *this;
}


Soprano::LiteralValue& Soprano::LiteralValue::operator=( bool b )
{
    d->dataTypeUri = QUrl();
    setValue( b );
    return *this;
}


Soprano::LiteralValue& Soprano::LiteralValue::operator=( double v )
{
    d->dataTypeUri = QUrl();
    setValue( v );
    return *this;
}


Soprano::LiteralValue& Soprano::LiteralValue::operator=( const QString& string )
{
    d->dataTypeUri = QUrl();
    setValue( string );
    return *this;
}


Soprano::LiteralValue& Soprano::LiteralValue::operator=( const QLatin1String& string )
{
    d->dataTypeUri = QUrl();
    setValue( QString::fromLatin1( string.latin1() ) );
    return *this;
}


Soprano::LiteralValue& Soprano::LiteralValue::operator=( const QDate& date )
{
    d->dataTypeUri = QUrl();
    setValue( date );
    return *this;
}


Soprano::LiteralValue& Soprano::LiteralValue::operator=( const QTime& time )
{
    d->dataTypeUri = QUrl();
    setValue( time );
    return *this;
}


Soprano::LiteralValue& Soprano::LiteralValue::operator=( const QDateTime& datetime )
{
    d->dataTypeUri = QUrl();
    setValue( datetime );
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


int Soprano::LiteralValue::toInt() const
{
    return value<int>();
}


qlonglong Soprano::LiteralValue::toInt64() const
{
    return value<qlonglong>();
}


uint Soprano::LiteralValue::toUnsignedInt() const
{
    return value<uint>();
}


qulonglong Soprano::LiteralValue::toUnsignedInt64() const
{
    return value<qulonglong>();
}


bool Soprano::LiteralValue::toBool() const
{
    return value<bool>();
}


double Soprano::LiteralValue::toDouble() const
{
    return value<double>();
}


// here we use DataTime instead of QT's built-in time conversion
// since the latter does not properly handle fractions of seconds
// in the ISO8601 specification.
QString Soprano::LiteralValue::toString() const
{
    if( isInt() )
        return QString::number( toInt() );
    else if( isInt64() )
        return QString::number( toInt64() );
    else if( isUnsignedInt() )
        return QString::number( toUnsignedInt() );
    else if( isUnsignedInt64() )
        return QString::number( toUnsignedInt64() );
    else if( isBool() )
        return ( toBool() ? QString("true") : QString("false" ) );
    else if( isDouble() ) // FIXME: decide on a proper double encoding or check if there is one in xml schema
        return QString::number( toDouble(), 'e', 10 );
    else if( isDate() )
        return DateTime::toString( toDate() );
    else if( isTime() )
        return DateTime::toString( toTime() );
    else if( isDateTime() )
        return DateTime::toString( toDateTime() );
    else
        return value<QString>();
}


QDate Soprano::LiteralValue::toDate() const
{
    return value<QDate>();
}


QTime Soprano::LiteralValue::toTime() const
{
    return value<QTime>();
}


QDateTime Soprano::LiteralValue::toDateTime() const
{
    return value<QDateTime>();
}


QVariant::Type Soprano::LiteralValue::type() const
{
    return QVariant::type();
}


QVariant Soprano::LiteralValue::variant() const
{
    return *this;
}


QUrl Soprano::LiteralValue::dataTypeUri() const
{
    if ( d->dataTypeUri.isEmpty() ) {
        // EVIL! But the dataTypeUri is just a cache and thus no detach is necessary
        const_cast<Private*>( d.constData() )->dataTypeUri = LiteralValue::dataTypeUriFromType( type() );
    }
    return d->dataTypeUri;
}


bool Soprano::LiteralValue::operator==( const LiteralValue& other ) const
{
    return QVariant::operator==( other );
}


bool Soprano::LiteralValue::operator!=( const LiteralValue& other ) const
{
    return QVariant::operator!=( other );
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
    default:
        qDebug() << "(Soprano::LiteralValue) unknown type: " << type << endl;
        return LiteralValue( value );
    }
}


Soprano::LiteralValue Soprano::LiteralValue::fromString( const QString& value, const QUrl& type )
{
    LiteralValue v = LiteralValue::fromString( value, typeFromDataTypeUri( type ) );
    v.d->dataTypeUri = type;
    return v;
}



QVariant::Type Soprano::LiteralValue::typeFromDataTypeUri( const QUrl& dataTypeUri )
{
    static QHash<QUrl, QVariant::Type> s_xmlSchemaTypes;
    if( s_xmlSchemaTypes.isEmpty() ) {
        s_xmlSchemaTypes.insert( Vocabulary::XMLSchema::INT(), QVariant::Int );
        s_xmlSchemaTypes.insert( Vocabulary::XMLSchema::INTEGER(), QVariant::Int );
        s_xmlSchemaTypes.insert( Vocabulary::XMLSchema::NEGATIVEINTEGER(), QVariant::Int );
        s_xmlSchemaTypes.insert( Vocabulary::XMLSchema::DECIMAL(), QVariant::Int );
        s_xmlSchemaTypes.insert( Vocabulary::XMLSchema::SHORT(), QVariant::Int );
        s_xmlSchemaTypes.insert( Vocabulary::XMLSchema::LONG(), QVariant::LongLong );
        s_xmlSchemaTypes.insert( Vocabulary::XMLSchema::UNSIGNEDINT(), QVariant::UInt );
        s_xmlSchemaTypes.insert( Vocabulary::XMLSchema::UNSIGNEDSHORT(), QVariant::UInt );
        s_xmlSchemaTypes.insert( Vocabulary::XMLSchema::UNSIGNEDLONG(), QVariant::ULongLong );
        s_xmlSchemaTypes.insert( Vocabulary::XMLSchema::BOOLEAN(), QVariant::Bool );
        s_xmlSchemaTypes.insert( Vocabulary::XMLSchema::DOUBLE(), QVariant::Double );
        s_xmlSchemaTypes.insert( Vocabulary::XMLSchema::STRING(), QVariant::String );
        s_xmlSchemaTypes.insert( Vocabulary::XMLSchema::DATE(), QVariant::Date );
        s_xmlSchemaTypes.insert( Vocabulary::XMLSchema::TIME(), QVariant::Time );
        s_xmlSchemaTypes.insert( Vocabulary::XMLSchema::DATETIME(), QVariant::DateTime );
    }

    QHash<QUrl, QVariant::Type>::const_iterator it = s_xmlSchemaTypes.find( dataTypeUri );
    if ( it != s_xmlSchemaTypes.end() ) {
        return it.value();
    }
    else {
        return QVariant::Invalid;
    }
}


QUrl Soprano::LiteralValue::dataTypeUriFromType( QVariant::Type type )
{
    static QHash<int, QUrl> s_variantSchemaTypeHash;
    if( s_variantSchemaTypeHash.isEmpty() ) {
        s_variantSchemaTypeHash.insert( QVariant::Int, Vocabulary::XMLSchema::INT() );
        s_variantSchemaTypeHash.insert( QVariant::LongLong, Vocabulary::XMLSchema::LONG() );
        s_variantSchemaTypeHash.insert( QVariant::UInt, Vocabulary::XMLSchema::UNSIGNEDINT() );
        s_variantSchemaTypeHash.insert( QVariant::ULongLong, Vocabulary::XMLSchema::UNSIGNEDLONG() );
        s_variantSchemaTypeHash.insert( QVariant::Bool, Vocabulary::XMLSchema::BOOLEAN() );
        s_variantSchemaTypeHash.insert( QVariant::Double, Vocabulary::XMLSchema::DOUBLE() );
        s_variantSchemaTypeHash.insert( QVariant::String, Vocabulary::XMLSchema::STRING() );
        s_variantSchemaTypeHash.insert( QVariant::Date, Vocabulary::XMLSchema::DATE() );
        s_variantSchemaTypeHash.insert( QVariant::Time, Vocabulary::XMLSchema::TIME() );
        s_variantSchemaTypeHash.insert( QVariant::DateTime, Vocabulary::XMLSchema::DATETIME() );
    }

    return s_variantSchemaTypeHash[type];
}


QDebug operator<<( QDebug dbg, const Soprano::LiteralValue& v )
{
    dbg << v.toString();
    return dbg;
}
