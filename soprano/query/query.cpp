/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007 Daniele Galdi <daniele.galdi@gmail.com>
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

#include "../node.h"

#include "query.h"

#include <QtCore/QVariant>
#include <QtCore/QList>

//
// Soprano::Query::RTerm
//

Soprano::Query::RTerm::RTerm()
{
}

Soprano::Query::RTerm::~RTerm()
{
}

bool Soprano::Query::RTerm::isVariable() const
{
   return false;
}

bool Soprano::Query::RTerm::isNode() const
{
   return false;
}

//
// Soprano::Query::Variable
//

class Soprano::Query::Variable::Private: public QSharedData {
public:
    Private() {}

    QString name;
};

Soprano::Query::Variable::Variable()
{
    d = new Private;
}

Soprano::Query::Variable::Variable( const QString &name )
{
    d = new Private;
    d->name = name;
}

Soprano::Query::Variable::Variable( const Variable &other )
{
    d = other.d;
}

Soprano::Query::Variable& Soprano::Query::Variable::operator=( const Variable& other )
{
    d = other.d;
    return *this;
}

Soprano::Query::Variable* Soprano::Query::Variable::clone() const
{
    return new Variable( *this );
}

QString Soprano::Query::Variable::name() const
{
    return d->name;
}

//
// Soprano::Query::Numerical
//

// FIXME: not sure if this works with built-in types
Q_DECLARE_METATYPE(float)

class Soprano::Query::Numerical::Private : public QSharedData
{
public:
   Private( const QVariant& v = QVariant() )
     : value( v ) {
   }
   QVariant value;
};

Soprano::Query::Numerical::Numerical()
{
    d = new Private;
}

Soprano::Query::Numerical::Numerical( int value )
{
    d = new Private( value );
}

Soprano::Query::Numerical::Numerical( double value )
{
    d = new Private( value );
}

Soprano::Query::Numerical::Numerical( float value )
{
    d = new Private;
    d->value.setValue( value );
}

Soprano::Query::Numerical::Numerical( const Numerical &other )
{
    d = other.d;
}

Soprano::Query::Numerical::~Numerical()
{
}

Soprano::Query::Numerical* Soprano::Query::Numerical::clone() const
{
    return new Numerical( *this );
}

Soprano::Query::Numerical& Soprano::Query::Numerical::operator=( const Numerical& other )
{
    d = other.d;
    return *this;
}

void Soprano::Query::Numerical::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}

bool Soprano::Query::Numerical::isDecimal()
{
    return isDouble() || isFloat();
}

bool Soprano::Query::Numerical::isDouble()
{
   return d->value.type() == QVariant::Double;
}

double Soprano::Query::Numerical::doubleValue()
{
   return d->value.toDouble();
}

bool Soprano::Query::Numerical::isFloat()
{
   return d->value.userType() == qMetaTypeId<float>();
}

float Soprano::Query::Numerical::floatValue()
{
   return d->value.value<float>();
}

bool Soprano::Query::Numerical::isInteger()
{
   return d->value.type() == QVariant::Int;
}

int Soprano::Query::Numerical::integerValue()
{
   return d->value.toInt();
}

//
// Soprano::Query::Node
//

class Soprano::Query::Node::Private : public QSharedData {
public:
    Private() {}

    Soprano::Node node;
};

Soprano::Query::Node::Node()
{
    d = new Private();
}

Soprano::Query::Node::Node( const Soprano::Node &node )
{
    d = new Private();
    d->node = node;
}

Soprano::Query::Node::Node( const Node &other )
{
    d = other.d;
}

Soprano::Query::Node* Soprano::Query::Node::clone() const
{
    return new Node( *this );
}

Soprano::Query::Node& Soprano::Query::Node::operator=( const Node& other )
{
    d = other.d;
    return *this;
}

Soprano::Node Soprano::Query::Node::node() const
{
    return d->node;
}

//
// Soprano::Query::Expression
//

Soprano::Query::Expression::Expression()
{
}

Soprano::Query::Expression::~Expression()
{
}

//
// Soprano::Query::BooleanExpression
//

Soprano::Query::BooleanExpression::BooleanExpression()
{
}

Soprano::Query::BooleanExpression::~BooleanExpression()
{
}

//
// Soprano::Query::NumericalExpression
//

Soprano::Query::NumericalExpression::NumericalExpression()
{
}

Soprano::Query::NumericalExpression::~NumericalExpression()
{
}

//
// Soprano::Query::StringExpression
//

Soprano::Query::StringExpression::StringExpression()
{
}

Soprano::Query::StringExpression::~StringExpression()
{
}

//
// Soprano::Query::String
//

class Soprano::Query::String::Private : public QSharedData
{
public:
    Private( const QString& s = QString() )
        : value( s ) {
    }

    QString value;
};

Soprano::Query::String::String()
    : d( new Private() )
{
}

Soprano::Query::String::String( const QString& s )
    : d( new Private( s ) )
{
}

Soprano::Query::String::String( const String& other )
{
    d = other.d;
}

Soprano::Query::String* Soprano::Query::String::clone() const
{
    return new String( *this );
}

QString Soprano::Query::String::value() const
{
    return d->value;
}

void Soprano::Query::String::setValue( const QString& s )
{
    d->value = s;
}

void Soprano::Query::String::accept( ExpressionVisitor* visitor )
{
    visitor->visit( this );
}


//
// Soprano::Query::UnaryBooleanExpression
//

class Soprano::Query::UnaryBooleanExpression::Private : public QSharedData
{
public:
    Private( BooleanExpression* e = 0 )
        : expression( e ) {
    }
    Private( const Private& other )
        : expression( 0 ) {
        if ( other.expression ) {
            expression = other.expression->clone();
        }
    }
    ~Private() {
        delete expression;
    }

    BooleanExpression* expression;
};

Soprano::Query::UnaryBooleanExpression::UnaryBooleanExpression( BooleanExpression *expression )
    : d( new Private( expression ) )
{
}

void Soprano::Query::UnaryBooleanExpression::setExpression( BooleanExpression *expression )
{
    d->expression = expression;
}

const Soprano::Query::BooleanExpression *Soprano::Query::UnaryBooleanExpression::expression() const
{
    return d->expression;
}

//
// Soprano::Query::UnaryRTermBooleanExpression
//

class Soprano::Query::UnaryRTermBooleanExpression::Private : public QSharedData
{
public:
    Private( RTerm* e = 0 )
        : term( e ) {
    }
    Private( const Private& other )
        : term( 0 ) {
        if ( other.term ) {
            term = other.term->clone();
        }
    }
    ~Private() {
        delete term;
    }

    RTerm* term;
};

Soprano::Query::UnaryRTermBooleanExpression::UnaryRTermBooleanExpression( RTerm *rterm )
    : d ( new Private( rterm ) )
{
}

void Soprano::Query::UnaryRTermBooleanExpression::setRTerm( RTerm *rterm )
{
    d->term = rterm;
}

const Soprano::Query::RTerm *Soprano::Query::UnaryRTermBooleanExpression::rterm() const
{
    return d->term;
}

//
// Soprano::Query::UnaryRTermStringExpression
//

class Soprano::Query::UnaryRTermStringExpression::Private : public QSharedData
{
public:
    Private( RTerm* e = 0 )
        : term( e ) {
    }
    Private( const Private& other )
        : term( 0 ) {
        if ( other.term ) {
            term = other.term->clone();
        }
    }
    ~Private() {
        delete term;
    }

    RTerm* term;
};

Soprano::Query::UnaryRTermStringExpression::UnaryRTermStringExpression( RTerm *rterm )
    : d ( new Private( rterm ) )
{
}

void Soprano::Query::UnaryRTermStringExpression::setRTerm( RTerm *rterm )
{
    d->term = rterm;
}

const Soprano::Query::RTerm *Soprano::Query::UnaryRTermStringExpression::rterm() const
{
    return d->term;
}

//
// Soprano::Query::UnaryNumericalExpression
//

class Soprano::Query::UnaryNumericalExpression::Private : public QSharedData
{
public:
    Private( NumericalExpression* e = 0 )
        : expression( e ) {
    }
    Private( const Private& other )
        : expression( 0 ) {
        if ( other.expression ) {
            expression = other.expression->clone();
        }
    }
    ~Private() {
        delete expression;
    }

    NumericalExpression* expression;
};

Soprano::Query::UnaryNumericalExpression::UnaryNumericalExpression( NumericalExpression *expression )
    : d( new Private(  expression ) )
{
}

void Soprano::Query::UnaryNumericalExpression::setExpression( NumericalExpression *expression )
{
    d->expression = expression;
}

const Soprano::Query::NumericalExpression *Soprano::Query::UnaryNumericalExpression::expression() const
{
    return d->expression;
}

//
// Soprano::Query::Not
//

Soprano::Query::Not::Not( BooleanExpression *expression )
    :UnaryBooleanExpression( expression )
{
}

Soprano::Query::Not::~Not()
{
}

void Soprano::Query::Not::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}

Soprano::Query::Not* Soprano::Query::Not::clone() const
{
    return new Not( *this );
}

//
// Soprano::Query::Negate
//

Soprano::Query::Negate::Negate( NumericalExpression *expression )
    :UnaryNumericalExpression( expression )
{
}

Soprano::Query::Negate::~Negate()
{
}

void Soprano::Query::Negate::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}

Soprano::Query::Negate* Soprano::Query::Negate::clone() const
{
    return new Negate( *this );
}

//
// Soprano::Query::IsBound
//

class Soprano::Query::IsBound::Private : public QSharedData
{
public:
    Private( Variable* v = 0 )
        : variable( v ) {
    }
    Private( const Private& other )
        : variable( 0 ) {
        if ( other.variable ) {
            variable = variable->clone();
        }
    }
    ~Private() {
        delete variable;
    }

    Variable* variable;
};

Soprano::Query::IsBound::IsBound()
    : d( new Private() )
{
}


Soprano::Query::IsBound::IsBound( Variable *variable )
    : d( new Private( variable ) )
{
}

Soprano::Query::IsBound::IsBound( const IsBound &other )
{
    d = other.d;
}

Soprano::Query::IsBound::~IsBound()
{
}

Soprano::Query::IsBound Soprano::Query::IsBound::operator=( const IsBound &other )
{
    d = other.d;
    return *this;
}

void Soprano::Query::IsBound::setVariable( Variable *variable )
{
    d->variable = variable;
}

const Soprano::Query::Variable *Soprano::Query::IsBound::variable() const
{
    return d->variable;
}

void Soprano::Query::IsBound::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}

Soprano::Query::IsBound* Soprano::Query::IsBound::clone() const
{
    return new IsBound( *this );
}


//
// Soprano::Query::IsIRI
//

Soprano::Query::IsIRI::IsIRI( RTerm *rterm )
    :UnaryRTermBooleanExpression( rterm )
{
}

Soprano::Query::IsIRI::~IsIRI()
{
}

void Soprano::Query::IsIRI::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}

Soprano::Query::IsIRI* Soprano::Query::IsIRI::clone() const
{
    return new IsIRI( *this );
}

//
// Soprano::Query::IsBlank
//

Soprano::Query::IsBlank::IsBlank( RTerm *rterm )
    :UnaryRTermBooleanExpression( rterm )
{
}

Soprano::Query::IsBlank::~IsBlank()
{
}

void Soprano::Query::IsBlank::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}

Soprano::Query::IsBlank* Soprano::Query::IsBlank::clone() const
{
    return new IsBlank( *this );
}

//
// Soprano::Query::IsLiteral
//

Soprano::Query::IsLiteral::IsLiteral( RTerm *rterm )
    :UnaryRTermBooleanExpression( rterm )
{
}

Soprano::Query::IsLiteral::~IsLiteral()
{
}

void Soprano::Query::IsLiteral::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}

Soprano::Query::IsLiteral* Soprano::Query::IsLiteral::clone() const
{
    return new IsLiteral( *this );
}

//
// Soprano::Query::StringValue
//

Soprano::Query::StringValue::StringValue( RTerm *rterm )
    :UnaryRTermStringExpression( rterm )
{
}

void Soprano::Query::StringValue::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}

Soprano::Query::StringValue* Soprano::Query::StringValue::clone() const
{
    return new StringValue( *this );
}


//
// Soprano::Query::LangValue
//

Soprano::Query::LangValue::LangValue( RTerm *rterm )
    :UnaryRTermStringExpression( rterm )
{
}

void Soprano::Query::LangValue::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}

Soprano::Query::LangValue* Soprano::Query::LangValue::clone() const
{
    return new LangValue( *this );
}

//
// Soprano::Query::DataTypeValue
//

Soprano::Query::DataTypeValue::DataTypeValue( RTerm *rterm )
    :UnaryRTermStringExpression( rterm )
{
}

void Soprano::Query::DataTypeValue::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}

Soprano::Query::DataTypeValue* Soprano::Query::DataTypeValue::clone() const
{
    return new DataTypeValue( *this );
}

//
// Soprano::Query::BinaryBooleanExpression
//

class Soprano::Query::BinaryBooleanExpression::Private : public QSharedData
{
public:
    Private( BooleanExpression *first_ = 0, BooleanExpression *second_ = 0 )
        : first( first_ ),
          second( second_ ) {
    }
    Private( const Private& other )
        : first( 0 ),
          second( 0 ) {
        if ( other.first ) {
            first = other.first->clone();
        }
        if ( other.second ) {
            second = other.second->clone();
        }
    }
    ~Private() {
        delete first;
        delete second;
    }

    BooleanExpression* first;
    BooleanExpression* second;
};

Soprano::Query::BinaryBooleanExpression::BinaryBooleanExpression( BooleanExpression *first, BooleanExpression *second )
    : d( new Private( first, second ) )
{
}

void Soprano::Query::BinaryBooleanExpression::setFirst( BooleanExpression *first )
{
    d->first = first;
}

void Soprano::Query::BinaryBooleanExpression::setSecond( BooleanExpression *second )
{
    d->second = second;
}

const Soprano::Query::BooleanExpression *Soprano::Query::BinaryBooleanExpression::first() const
{
   return d->first;
}

const Soprano::Query::BooleanExpression *Soprano::Query::BinaryBooleanExpression::second() const
{
   return d->second;
}

//
// Soprano::Query::BinaryNumericalBooleanExpression
//

class Soprano::Query::BinaryNumericalBooleanExpression::Private : public QSharedData
{
public:
    Private( NumericalExpression *first_ = 0, NumericalExpression *second_ = 0 )
        : first( first_ ),
          second( second_ ) {
    }
    Private( const Private& other )
        : first( 0 ),
          second( 0 ) {
        if ( other.first ) {
            first = other.first->clone();
        }
        if ( other.second ) {
            second = other.second->clone();
        }
    }
    ~Private() {
        delete first;
        delete second;
    }

    NumericalExpression* first;
    NumericalExpression* second;
};

Soprano::Query::BinaryNumericalBooleanExpression::BinaryNumericalBooleanExpression( NumericalExpression *first, NumericalExpression *second )
    : d( new Private( first, second ) )
{
}

void Soprano::Query::BinaryNumericalBooleanExpression::setFirst( NumericalExpression *first )
{
    d->first = first;
}

void Soprano::Query::BinaryNumericalBooleanExpression::setSecond( NumericalExpression *second )
{
    d->second = second;
}

const Soprano::Query::NumericalExpression *Soprano::Query::BinaryNumericalBooleanExpression::first() const
{
   return d->first;
}

const Soprano::Query::NumericalExpression *Soprano::Query::BinaryNumericalBooleanExpression::second() const
{
   return d->second;
}

//
// Soprano::Query::BinaryStringBooleanExpression
//

class Soprano::Query::BinaryStringBooleanExpression::Private : public QSharedData
{
public:
    Private( StringExpression *first_ = 0, StringExpression *second_ = 0 )
        : first( first_ ),
          second( second_ ) {
    }
    Private( const Private& other )
        : first( 0 ),
          second( 0 ) {
        if ( other.first ) {
            first = other.first->clone();
        }
        if ( other.second ) {
            second = other.second->clone();
        }
    }
    ~Private() {
        delete first;
        delete second;
    }

    StringExpression* first;
    StringExpression* second;
};

Soprano::Query::BinaryStringBooleanExpression::BinaryStringBooleanExpression( StringExpression *first, StringExpression *second )
    : d( new Private( first, second ) )
{
}

void Soprano::Query::BinaryStringBooleanExpression::setFirst( StringExpression *first )
{
    d->first = first;
}

void Soprano::Query::BinaryStringBooleanExpression::setSecond( StringExpression *second )
{
    d->second = second;
}

const Soprano::Query::StringExpression *Soprano::Query::BinaryStringBooleanExpression::first() const
{
   return d->first;
}

const Soprano::Query::StringExpression *Soprano::Query::BinaryStringBooleanExpression::second() const
{
   return d->second;
}

//
// Soprano::Query::BinaryDateTimeBooleanExpression
//

class Soprano::Query::BinaryDateTimeBooleanExpression::Private : public QSharedData
{
public:
    Private( const QDateTime first_ = QDateTime(), const QDateTime& second_ = QDateTime() )
        : first( first_ ),
          second( second_ ) {
    }

    QDateTime first;
    QDateTime second;
};

Soprano::Query::BinaryDateTimeBooleanExpression::BinaryDateTimeBooleanExpression( const QDateTime &first, const QDateTime &second )
    : d( new Private( first, second ) )
{
}

void Soprano::Query::BinaryDateTimeBooleanExpression::setFirst( const QDateTime &first )
{
    d->first = first;
}

void Soprano::Query::BinaryDateTimeBooleanExpression::setSecond( const QDateTime &second )
{
    d->second = second;
}

QDateTime Soprano::Query::BinaryDateTimeBooleanExpression::first() const
{
   return d->first;
}

QDateTime Soprano::Query::BinaryDateTimeBooleanExpression::second() const
{
   return d->second;
}

//
// Soprano::Query::BinaryRTermBooleanExpression
//

class Soprano::Query::BinaryRTermBooleanExpression::Private : public QSharedData
{
public:
    Private( RTerm *first_ = 0, RTerm *second_ = 0 )
        : first( first_ ),
          second( second_ ) {
    }
    Private( const Private& other )
        : first( 0 ),
          second( 0 ) {
        if ( other.first ) {
            first = other.first->clone();
        }
        if ( other.second ) {
            second = other.second->clone();
        }
    }
    ~Private() {
        delete first;
        delete second;
    }

    RTerm* first;
    RTerm* second;
};

Soprano::Query::BinaryRTermBooleanExpression::BinaryRTermBooleanExpression( RTerm *first, RTerm *second )
    : d( new Private( first, second ) )
{
}

void Soprano::Query::BinaryRTermBooleanExpression::setFirst( RTerm *first )
{
    d->first = first;
}

void Soprano::Query::BinaryRTermBooleanExpression::setSecond( RTerm *second )
{
    d->second = second;
}

const Soprano::Query::RTerm *Soprano::Query::BinaryRTermBooleanExpression::first() const
{
   return d->first;
}

const Soprano::Query::RTerm *Soprano::Query::BinaryRTermBooleanExpression::second() const
{
   return d->second;
}

//
// Soprano::Query::BinaryNumericalExpression
//

class Soprano::Query::BinaryNumericalExpression::Private : public QSharedData
{
public:
    Private( NumericalExpression *first_ = 0, NumericalExpression *second_ = 0 )
        : first( first_ ),
          second( second_ ) {
    }
    Private( const Private& other )
        : first( 0 ),
          second( 0 ) {
        if ( other.first ) {
            first = other.first->clone();
        }
        if ( other.second ) {
            second = other.second->clone();
        }
    }
    ~Private() {
        delete first;
        delete second;
    }

    NumericalExpression* first;
    NumericalExpression* second;
};

Soprano::Query::BinaryNumericalExpression::BinaryNumericalExpression( NumericalExpression *first, NumericalExpression *second )
    : d( new Private( first, second ) )
{
}

void Soprano::Query::BinaryNumericalExpression::setFirst( NumericalExpression *first )
{
    d->first = first;
}

void Soprano::Query::BinaryNumericalExpression::setSecond( NumericalExpression *second )
{
    d->second = second;
}

const Soprano::Query::NumericalExpression *Soprano::Query::BinaryNumericalExpression::first() const
{
   return d->first;
}

const Soprano::Query::NumericalExpression *Soprano::Query::BinaryNumericalExpression::second() const
{
   return d->second;
}

//
// Soprano::Query::LogicOr
//

Soprano::Query::LogicOr::LogicOr( BooleanExpression *first, BooleanExpression *second )
    :BinaryBooleanExpression( first, second )
{
}

void Soprano::Query::LogicOr::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}

Soprano::Query::LogicOr* Soprano::Query::LogicOr::clone() const
{
    return new LogicOr( *this );
}

//
// Soprano::Query::LogicAnd
//

Soprano::Query::LogicAnd::LogicAnd( BooleanExpression *first, BooleanExpression *second )
    :BinaryBooleanExpression( first, second )
{
}

void Soprano::Query::LogicAnd::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}

Soprano::Query::LogicAnd* Soprano::Query::LogicAnd::clone() const
{
    return new LogicAnd( *this );
}

//
// Soprano::Query::NumericalEqual
//

Soprano::Query::NumericalEqual::NumericalEqual( NumericalExpression *first, NumericalExpression *second )
    :BinaryNumericalBooleanExpression( first, second )
{
}

void Soprano::Query::NumericalEqual::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}

Soprano::Query::NumericalEqual* Soprano::Query::NumericalEqual::clone() const
{
    return new NumericalEqual( *this );
}

//
// Soprano::Query::NumericalNotEqual
//

Soprano::Query::NumericalNotEqual::NumericalNotEqual( NumericalExpression *first, NumericalExpression *second )
    :BinaryNumericalBooleanExpression( first, second )
{
}

void Soprano::Query::NumericalNotEqual::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}

Soprano::Query::NumericalNotEqual* Soprano::Query::NumericalNotEqual::clone() const
{
    return new NumericalNotEqual( *this );
}

//
// Soprano::Query::StringEqual
//

Soprano::Query::StringEqual::StringEqual( StringExpression *first, StringExpression *second )
    :BinaryStringBooleanExpression( first, second )
{
}

void Soprano::Query::StringEqual::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}

Soprano::Query::StringEqual* Soprano::Query::StringEqual::clone() const
{
    return new StringEqual( *this );
}


//
// Soprano::Query::StringNotEqual
//

Soprano::Query::StringNotEqual::StringNotEqual( StringExpression *first, StringExpression *second )
    :BinaryStringBooleanExpression( first, second )
{
}

void Soprano::Query::StringNotEqual::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}

Soprano::Query::StringNotEqual* Soprano::Query::StringNotEqual::clone() const
{
    return new StringNotEqual( *this );
}


//
// Soprano::Query::DateTimeEqual
//

Soprano::Query::DateTimeEqual::DateTimeEqual( const QDateTime& first, const QDateTime& second )
    : BinaryDateTimeBooleanExpression( first, second )
{
}

void Soprano::Query::DateTimeEqual::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}

Soprano::Query::DateTimeEqual* Soprano::Query::DateTimeEqual::clone() const
{
    return new DateTimeEqual( *this );
}


//
// Soprano::Query::DateTimeNotEqual
//

Soprano::Query::DateTimeNotEqual::DateTimeNotEqual( const QDateTime& first, const QDateTime& second )
    :BinaryDateTimeBooleanExpression( first, second )
{
}

void Soprano::Query::DateTimeNotEqual::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}

Soprano::Query::DateTimeNotEqual* Soprano::Query::DateTimeNotEqual::clone() const
{
    return new DateTimeNotEqual( *this );
}


//
// Soprano::Query::NumericalLessThan
//

Soprano::Query::NumericalLessThan::NumericalLessThan( NumericalExpression *first, NumericalExpression *second )
    :BinaryNumericalBooleanExpression( first, second )
{
}

void Soprano::Query::NumericalLessThan::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}

Soprano::Query::NumericalLessThan* Soprano::Query::NumericalLessThan::clone() const
{
    return new NumericalLessThan( *this );
}


//
// Soprano::Query::NumericalGreaterThan
//

Soprano::Query::NumericalGreaterThan::NumericalGreaterThan( NumericalExpression *first, NumericalExpression *second )
    :BinaryNumericalBooleanExpression( first, second )
{
}

void Soprano::Query::NumericalGreaterThan::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}

Soprano::Query::NumericalGreaterThan* Soprano::Query::NumericalGreaterThan::clone() const
{
    return new NumericalGreaterThan( *this );
}


//
// Soprano::Query::NumericalLessThan
//

Soprano::Query::NumericalLessThanEqual::NumericalLessThanEqual( NumericalExpression *first, NumericalExpression *second )
    :BinaryNumericalBooleanExpression( first, second )
{
}

void Soprano::Query::NumericalLessThanEqual::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}

Soprano::Query::NumericalLessThanEqual* Soprano::Query::NumericalLessThanEqual::clone() const
{
    return new NumericalLessThanEqual( *this );
}


//
// Soprano::Query::NumericalGreaterThanEqual
//

Soprano::Query::NumericalGreaterThanEqual::NumericalGreaterThanEqual( NumericalExpression *first, NumericalExpression *second )
    :BinaryNumericalBooleanExpression( first, second )
{
}

void Soprano::Query::NumericalGreaterThanEqual::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}

Soprano::Query::NumericalGreaterThanEqual* Soprano::Query::NumericalGreaterThanEqual::clone() const
{
    return new NumericalGreaterThanEqual( *this );
}


//
// Soprano::Query::StringLessThan
//

Soprano::Query::StringLessThan::StringLessThan( StringExpression *first, StringExpression *second )
    :BinaryStringBooleanExpression( first, second )
{
}

void Soprano::Query::StringLessThan::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}

Soprano::Query::StringLessThan* Soprano::Query::StringLessThan::clone() const
{
    return new StringLessThan( *this );
}


//
// Soprano::Query::StringGreaterThan
//

Soprano::Query::StringGreaterThan::StringGreaterThan( StringExpression *first, StringExpression *second )
    :BinaryStringBooleanExpression( first, second )
{
}

void Soprano::Query::StringGreaterThan::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}


Soprano::Query::StringGreaterThan* Soprano::Query::StringGreaterThan::clone() const
{
    return new StringGreaterThan( *this );
}

//
// Soprano::Query::StringLessThanEqual
//

Soprano::Query::StringLessThanEqual::StringLessThanEqual( StringExpression *first, StringExpression *second )
    :BinaryStringBooleanExpression( first, second )
{
}

void Soprano::Query::StringLessThanEqual::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}

Soprano::Query::StringLessThanEqual* Soprano::Query::StringLessThanEqual::clone() const
{
    return new StringLessThanEqual( *this );
}

//
// Soprano::Query::StringGreaterThanEqual
//

Soprano::Query::StringGreaterThanEqual::StringGreaterThanEqual( StringExpression *first, StringExpression *second )
    :BinaryStringBooleanExpression( first, second )
{
}

void Soprano::Query::StringGreaterThanEqual::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}

Soprano::Query::StringGreaterThanEqual* Soprano::Query::StringGreaterThanEqual::clone() const
{
    return new StringGreaterThanEqual( *this );
}

//
// Soprano::Query::DateTimeLessThan
//

Soprano::Query::DateTimeLessThan::DateTimeLessThan( const QDateTime& first, const QDateTime& second )
    :BinaryDateTimeBooleanExpression( first, second )
{
}

void Soprano::Query::DateTimeLessThan::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}

Soprano::Query::DateTimeLessThan* Soprano::Query::DateTimeLessThan::clone() const
{
    return new DateTimeLessThan( *this );
}

//
// Soprano::Query::DateTimeGreaterThan
//

Soprano::Query::DateTimeGreaterThan::DateTimeGreaterThan( const QDateTime& first, const QDateTime& second )
    :BinaryDateTimeBooleanExpression( first, second )
{
}

void Soprano::Query::DateTimeGreaterThan::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}

Soprano::Query::DateTimeGreaterThan* Soprano::Query::DateTimeGreaterThan::clone() const
{
    return new DateTimeGreaterThan( *this );
}

//
// Soprano::Query::DateTimeLessThanEqual
//

Soprano::Query::DateTimeLessThanEqual::DateTimeLessThanEqual( const QDateTime& first, const QDateTime& second )
    :BinaryDateTimeBooleanExpression( first, second )
{
}

void Soprano::Query::DateTimeLessThanEqual::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}

Soprano::Query::DateTimeLessThanEqual* Soprano::Query::DateTimeLessThanEqual::clone() const
{
    return new DateTimeLessThanEqual( *this );
}

//
// Soprano::Query::DateTimeGreaterThanEqual
//

Soprano::Query::DateTimeGreaterThanEqual::DateTimeGreaterThanEqual( const QDateTime& first, const QDateTime& second )
    :BinaryDateTimeBooleanExpression( first, second )
{
}

void Soprano::Query::DateTimeGreaterThanEqual::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}

Soprano::Query::DateTimeGreaterThanEqual* Soprano::Query::DateTimeGreaterThanEqual::clone() const
{
    return new DateTimeGreaterThanEqual( *this );
}

//
// Soprano::Query::NumericalMultiply
//

Soprano::Query::NumericalMultiply::NumericalMultiply( NumericalExpression *first, NumericalExpression *second )
    :BinaryNumericalExpression( first, second )
{
}

void Soprano::Query::NumericalMultiply::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}

Soprano::Query::NumericalMultiply* Soprano::Query::NumericalMultiply::clone() const
{
    return new NumericalMultiply( *this );
}

//
// Soprano::Query::NumericalDivide
//

Soprano::Query::NumericalDivide::NumericalDivide( NumericalExpression *first, NumericalExpression *second )
    :BinaryNumericalExpression( first, second )
{
}

void Soprano::Query::NumericalDivide::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}

Soprano::Query::NumericalDivide* Soprano::Query::NumericalDivide::clone() const
{
    return new NumericalDivide( *this );
}

//
// Soprano::Query::NumericalAdd
//

Soprano::Query::NumericalAdd::NumericalAdd( NumericalExpression *first, NumericalExpression *second )
    :BinaryNumericalExpression( first, second )
{
}

void Soprano::Query::NumericalAdd::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}

Soprano::Query::NumericalAdd* Soprano::Query::NumericalAdd::clone() const
{
    return new NumericalAdd( *this );
}

//
// Soprano::Query::NumericalSubtract
//

Soprano::Query::NumericalSubtract::NumericalSubtract( NumericalExpression *first, NumericalExpression *second )
    :BinaryNumericalExpression( first, second )
{
}

void Soprano::Query::NumericalSubtract::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}

Soprano::Query::NumericalSubtract* Soprano::Query::NumericalSubtract::clone() const
{
    return new NumericalSubtract( *this );
}

//
// Soprano::Query::RTermEqual
//

Soprano::Query::RTermEqual::RTermEqual( RTerm *first, RTerm *second )
    :BinaryRTermBooleanExpression( first, second )
{
}

void Soprano::Query::RTermEqual::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}

Soprano::Query::RTermEqual* Soprano::Query::RTermEqual::clone() const
{
    return new RTermEqual( *this );
}

//
// Soprano::Query::RTermNotEqual
//

Soprano::Query::RTermNotEqual::RTermNotEqual( RTerm *first, RTerm *second )
    :BinaryRTermBooleanExpression( first, second )
{
}

void Soprano::Query::RTermNotEqual::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}

Soprano::Query::RTermNotEqual* Soprano::Query::RTermNotEqual::clone() const
{
    return new RTermNotEqual( *this );
}

//
// Soprano::Query::LangMatches
//

Soprano::Query::LangMatches::LangMatches( StringExpression *first, StringExpression *second )
    :BinaryStringBooleanExpression( first, second )
{
}

void Soprano::Query::LangMatches::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}

Soprano::Query::LangMatches* Soprano::Query::LangMatches::clone() const
{
    return new LangMatches( *this );
}

//
// Soprano::Query::Regexp
//

class Soprano::Query::Regexp::Private : public QSharedData
{
public:
    Private( StringExpression *ex, const QString &p, const QString& f = QString() )
        : expression( ex ),
          pattern( p ),
          flags( f ) {
    }
    Private( const Private& other)
        : expression( 0 ),
          pattern( other.pattern ),
          flags( other.flags ) {
        if ( other.expression ) {
            expression = other.expression->clone();                         }
    }
    ~Private() {
        delete expression;
    }

    StringExpression *expression;
            QString pattern;
            QString flags;
};

Soprano::Query::Regexp::Regexp( StringExpression *expression, const QString &pattern )
    : d( new Private( expression, pattern ) )
{
}

Soprano::Query::Regexp::Regexp( StringExpression *expression, const QString &pattern, const QString &flags )
    : d( new Private( expression, pattern, flags ) )
{
}

Soprano::Query::Regexp* Soprano::Query::Regexp::clone() const
{
    return new Regexp( *this );
}

void Soprano::Query::Regexp::setExpression( StringExpression *expression )
{
    d->expression = expression;
}

const Soprano::Query::StringExpression *Soprano::Query::Regexp::expression() const
{
    return d->expression;
}

void Soprano::Query::Regexp::setPattern( const QString &pattern )
{
    d->pattern = pattern;
}

QString Soprano::Query::Regexp::pattern()
{
    return d->pattern;
}

void Soprano::Query::Regexp::setFlags( const QString &flags )
{
    d->flags = flags;
}

QString Soprano::Query::Regexp::flags()
{
    return d->flags;
}

void Soprano::Query::Regexp::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}

//
// Soprano::Query::Prefix
//

class Soprano::Query::Prefix::Private: public QSharedData {
public:
    Private() {};

    QString prefix;
    QUrl uri;
};

Soprano::Query::Prefix::Prefix()
{
    d = new Private();
}

Soprano::Query::Prefix::Prefix( const QString &prefix, const QUrl &uri )
{
    d = new Private();
    d->uri = uri;
    d->prefix = prefix;
}

Soprano::Query::Prefix::Prefix( const Prefix &other )
{
    d = other.d;
}

Soprano::Query::Prefix& Soprano::Query::Prefix::operator=( const Prefix& other )
{
    d = other.d;
    return *this;
}

const QString Soprano::Query::Prefix::prefix() const
{
    return d->prefix;
}

const QUrl Soprano::Query::Prefix::uri() const
{
    return d->uri;
}

//
// Soprano::Query::GraphPattern
//

class Soprano::Query::GraphPattern::Private: public QSharedData
{
public:
    Private()
      : subject( 0 ),
        predicate( 0 ),
        object( 0 ),
        context( 0 ),
        optional( false )
    {};

    Private( const Private& other )
      : subject( 0 ),
        predicate( 0 ),
        object( 0 ),
        context( 0 ),
        optional( other.optional ) {
        if ( other.subject ) {
            subject = other.subject->clone();
        }
        if ( other.predicate ) {
            predicate = other.predicate->clone();
        }
        if ( other.object ) {
            object = other.object->clone();
        }
        if ( other.context ) {
            context = other.context->clone();
        }
    }

    ~Private()
    {
        delete subject;
        delete predicate;
        delete object;
        delete context;
    }

    RTerm *subject;
    RTerm *predicate;
    RTerm *object;
    RTerm *context;
    bool optional;
};

Soprano::Query::GraphPattern::GraphPattern()
{
   d = new Private;
}

Soprano::Query::GraphPattern::GraphPattern( RTerm *subject, RTerm *predicate, RTerm *object, RTerm *context, bool optional )
{
    d = new Private;
    d->subject = subject;
    d->predicate = predicate;
    d->object = object;
    d->context = context;
    d->optional = optional;
}

Soprano::Query::GraphPattern::~GraphPattern()
{
}

Soprano::Query::GraphPattern& Soprano::Query::GraphPattern::operator=( const GraphPattern& other )
{
    d = other.d;
    return *this;
}

void Soprano::Query::GraphPattern::setOptional(bool optional)
{
    d->optional = optional;
}

bool Soprano::Query::GraphPattern::optional() const
{
    return d->optional;
}

void Soprano::Query::GraphPattern::setSubject( RTerm *subject )
{
    delete d->subject;
    d->subject = subject;
}

const Soprano::Query::RTerm *Soprano::Query::GraphPattern::subject() const
{
    return d->subject;
}

void Soprano::Query::GraphPattern::setPredicate( RTerm *predicate )
{
    delete d->predicate;
    d->predicate = predicate;
}

const Soprano::Query::RTerm *Soprano::Query::GraphPattern::predicate() const
{
   return d->predicate;
}

void Soprano::Query::GraphPattern::setObject( RTerm *object )
{
   delete d->object;
   d->object = object;
}

const Soprano::Query::RTerm *Soprano::Query::GraphPattern::object() const
{
   return d->object;
}

void Soprano::Query::GraphPattern::setContext( RTerm *context )
{
    delete d->context;
    d->context = context;
}

const Soprano::Query::RTerm *Soprano::Query::GraphPattern::context() const
{
    return d->context;
}

//
// Soprano::Query::Query
//

class Soprano::Query::Query::Private : public QSharedData {
public:
    Private()
        : condition(0) {
    }
    Private( const Private& other )
        : condition(0),
          prefixes( other.prefixes ),
          queryTerms( other.queryTerms ),
          type( other.type ) {
        if ( other.condition ) {
            condition = other.condition->clone();
        }
    }
    ~Private() {
        delete condition;
    }

    BooleanExpression *condition;

    QList<Prefix> prefixes;
    QueryTerms queryTerms;
    QueryType type;
};

Soprano::Query::Query::Query()
{
   d = new Private;
}

Soprano::Query::Query::Query( QueryType type )
{
   d = new Private;
   d->type = type;
}

Soprano::Query::Query::~Query()
{
}

Soprano::Query::Query& Soprano::Query::Query::operator=( const Query& other )
{
    d = other.d;
    return *this;
}

void Soprano::Query::Query::addPrefix( const Prefix &prefix )
{
    d->prefixes.append( prefix );
}

QList<Soprano::Query::Prefix> Soprano::Query::Query::prefixes()
{
    return d->prefixes;
}

void Soprano::Query::Query::setCondition( BooleanExpression* condition )
{
    delete d->condition;
    d->condition = condition;
}

const Soprano::Query::BooleanExpression *Soprano::Query::Query::condition() const
{
    return d->condition;
}

void Soprano::Query::Query::setQueryType( QueryType type )
{
    d->type = type;
}

Soprano::Query::Query::QueryType Soprano::Query::Query::type() const
{
    return d->type;
}

void Soprano::Query::Query::setQueryTerms( const QueryTerms &queryTerms )
{
    d->queryTerms = queryTerms;
}

Soprano::Query::QueryTerms Soprano::Query::Query::terms() const
{
    d->queryTerms;
}

bool Soprano::Query::Query::isValid() const
{
    return type() == INVALID_QUERY;
}


//
// Soprano::Query::QueryTerms
//

class Soprano::Query::QueryTerms::Private : public QSharedData
{
public:
    Private() {}

    Private( const Private& other) {
       Q_FOREACH( RTerm* r, other.rterms ) {
          rterms.append( r->clone() );
       }
    }
    ~Private() {
       Q_FOREACH( RTerm* r, rterms ) {
          delete r;
       }
    }

    QList<RTerm *> rterms;
};

Soprano::Query::QueryTerms::QueryTerms()
{
    d = new Private();
}

Soprano::Query::QueryTerms::QueryTerms( const QueryTerms& other )
{
    d = other.d;
}

Soprano::Query::QueryTerms::~QueryTerms()
{
}

Soprano::Query::QueryTerms& Soprano::Query::QueryTerms::operator=( const QueryTerms& other )
{
    d = other.d;
    return *this;
}


void Soprano::Query::QueryTerms::addQueryTerm( RTerm *rterm )
{
   d->rterms.append( rterm );
}

QList<const Soprano::Query::RTerm*> Soprano::Query::QueryTerms::terms() const
{
    QList<const RTerm*> l;
    Q_FOREACH( RTerm* r, d->rterms ) {
        l.append( r );
    }
    return l;
}

bool Soprano::Query::QueryTerms::selectAll() const
{
    return d->rterms.isEmpty();
}
