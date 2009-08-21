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

class Soprano::Query::Variable::Private : public QSharedData {
public:
    Private();
    QString name;
};

Soprano::Query::Variable::Private::Private()
{
}

Soprano::Query::Variable::Variable()
    : RTerm()
{
    d = new Private;
}

Soprano::Query::Variable::~Variable()
{
}

Soprano::Query::Variable::Variable( const QString &name )
    : RTerm()
{
    d = new Private;
    d->name = name;
}

Soprano::Query::Variable::Variable( const Variable &other )
    : RTerm( other )
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

Soprano::Query::Numerical::Private::Private( const QVariant& v )
 : value( v ) {
}

Soprano::Query::Numerical::Numerical()
    : NumericalExpression()
{
    d = new Private;
}

Soprano::Query::Numerical::Numerical( int value )
    : NumericalExpression()
{
    d = new Private( value );
}

Soprano::Query::Numerical::Numerical( double value )
    : NumericalExpression()
{
    d = new Private( value );
}

Soprano::Query::Numerical::Numerical( float value )
    : NumericalExpression()
{
    d = new Private;
    d->value.setValue( value );
}

Soprano::Query::Numerical::Numerical( const Numerical &other )
    : NumericalExpression( other )
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

void Soprano::Query::Numerical::accept( ExpressionVisitor *visitor ) const
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

Soprano::Query::Node::Private::Private() {
}

Soprano::Query::Node::Node()
    : RTerm()
{
    d = new Private();
}

Soprano::Query::Node::Node( const Soprano::Node &node )
    : RTerm()
{
    d = new Private();
    d->node = node;
}

Soprano::Query::Node::Node( const Node &other )
    : RTerm()
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

Soprano::Query::String::Private::Private( const QString& s )
    : value( s ) {
}

Soprano::Query::String::String()
    : StringExpression(),
      d( new Private() )
{
}

Soprano::Query::String::String( const QString& s )
    : StringExpression(),
      d( new Private( s ) )
{
}

Soprano::Query::String::String( const String& other )
    : StringExpression()
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

void Soprano::Query::String::accept( ExpressionVisitor* visitor ) const
{
    visitor->visit( this );
}


//
// Soprano::Query::UnaryBooleanExpression
//

Soprano::Query::UnaryBooleanExpression::Private::Private( BooleanExpression* e )
    : QSharedData(),
      expression( e )
{
}

Soprano::Query::UnaryBooleanExpression::Private::Private( const Private& other )
    : QSharedData(),
      expression( 0 )
{
    if ( other.expression ) {
        expression = other.expression->clone();
    }
}

Soprano::Query::UnaryBooleanExpression::Private::~Private()
{
    delete expression;
}


Soprano::Query::UnaryBooleanExpression::UnaryBooleanExpression( BooleanExpression *expression )
    : d( new Private( expression ) )
{
}

void Soprano::Query::UnaryBooleanExpression::setExpression( BooleanExpression *expression )
{
    if ( expression != d->expression ) {
        delete d->expression;
        d->expression = expression;
    }
}

const Soprano::Query::BooleanExpression *Soprano::Query::UnaryBooleanExpression::expression() const
{
    return d->expression;
}

//
// Soprano::Query::UnaryRTermExpressionBase
//

Soprano::Query::UnaryRTermExpressionBase::Private::Private( RTerm* e )
    : QSharedData(),
      term( e )
{
}

Soprano::Query::UnaryRTermExpressionBase::Private::Private( const Private& other )
    : QSharedData(),
      term( 0 )
{
    if ( other.term ) {
        term = other.term->clone();
    }
}

Soprano::Query::UnaryRTermExpressionBase::Private::~Private()
{
    delete term;
}


Soprano::Query::UnaryRTermExpressionBase::UnaryRTermExpressionBase( RTerm *rterm )
    : d ( new Private( rterm ) )
{
}

void Soprano::Query::UnaryRTermExpressionBase::setRTerm( RTerm *rterm )
{
    d->term = rterm;
}

const Soprano::Query::RTerm *Soprano::Query::UnaryRTermExpressionBase::rterm() const
{
    return d->term;
}

//
// Soprano::Query::UnaryRTermBooleanExpression
//

Soprano::Query::UnaryRTermBooleanExpression::UnaryRTermBooleanExpression( RTerm *rterm )
    : UnaryRTermExpressionBase( rterm )
{
}

//
// Soprano::Query::UnaryRTermStringExpression
//

Soprano::Query::UnaryRTermStringExpression::UnaryRTermStringExpression( RTerm *rterm )
    : UnaryRTermExpressionBase( rterm )
{
}

//
// Soprano::Query::UnaryRTermNumericalExpression
//

Soprano::Query::UnaryRTermNumericalExpression::UnaryRTermNumericalExpression( RTerm *rterm )
    : UnaryRTermExpressionBase( rterm )
{
}

//
// Soprano::Query::UnaryNumericalExpression
//

Soprano::Query::UnaryNumericalExpression::Private::Private( NumericalExpression* e )
    : QSharedData(),
      expression( e )
{
}

Soprano::Query::UnaryNumericalExpression::Private::Private( const Private& other )
    : QSharedData(),
      expression( 0 )
{
    if ( other.expression ) {
        expression = other.expression->clone();
    }
}

Soprano::Query::UnaryNumericalExpression::Private::~Private() {
    delete expression;
}

Soprano::Query::UnaryNumericalExpression::UnaryNumericalExpression( NumericalExpression *expression )
    : d( new Private(  expression ) )
{
}

void Soprano::Query::UnaryNumericalExpression::setExpression( NumericalExpression *expression )
{
    if ( expression != d->expression ) {
        delete d->expression;
        d->expression = expression;
    }
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

void Soprano::Query::Not::accept( ExpressionVisitor *visitor ) const
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


void Soprano::Query::Negate::accept( ExpressionVisitor *visitor ) const
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

Soprano::Query::IsBound::Private::Private( Variable* v )
    : QSharedData(),
      variable( v )
{
}

Soprano::Query::IsBound::Private::Private( const Private& other )
    : QSharedData(),
      variable( 0 )
{
    if ( other.variable ) {
        variable = other.variable->clone();
    }
}

Soprano::Query::IsBound::Private::~Private()
{
    delete variable;
}

Soprano::Query::IsBound::IsBound()
    : BooleanExpression(),
      d( new Private() )
{
}


Soprano::Query::IsBound::IsBound( Variable *variable )
    : BooleanExpression(),
      d( new Private( variable ) )
{
}

Soprano::Query::IsBound::IsBound( const IsBound &other )
    : BooleanExpression()
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

void Soprano::Query::IsBound::accept( ExpressionVisitor *visitor ) const
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

void Soprano::Query::IsIRI::accept( ExpressionVisitor *visitor ) const
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

void Soprano::Query::IsBlank::accept( ExpressionVisitor *visitor ) const
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

void Soprano::Query::IsLiteral::accept( ExpressionVisitor *visitor ) const
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

void Soprano::Query::StringValue::accept( ExpressionVisitor *visitor ) const
{
    visitor->visit( this );
}

Soprano::Query::StringValue* Soprano::Query::StringValue::clone() const
{
    return new StringValue( *this );
}


//
// Soprano::Query::NumericalValue
//

Soprano::Query::NumericalValue::NumericalValue( RTerm *rterm )
    : UnaryRTermNumericalExpression( rterm )
{
}

void Soprano::Query::NumericalValue::accept( ExpressionVisitor *visitor ) const
{
    visitor->visit( this );
}

Soprano::Query::NumericalValue* Soprano::Query::NumericalValue::clone() const
{
    return new NumericalValue( *this );
}


//
// Soprano::Query::LangValue
//

Soprano::Query::LangValue::LangValue( RTerm *rterm )
    :UnaryRTermStringExpression( rterm )
{
}

void Soprano::Query::LangValue::accept( ExpressionVisitor *visitor ) const
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

void Soprano::Query::DataTypeValue::accept( ExpressionVisitor *visitor ) const
{
    visitor->visit( this );
}

Soprano::Query::DataTypeValue* Soprano::Query::DataTypeValue::clone() const
{
    return new DataTypeValue( *this );
}

//
// Soprano::Query::BooleanSetExpression
//

Soprano::Query::BooleanSetExpression::Private::Private()
    : QSharedData()
{
}

Soprano::Query::BooleanSetExpression::Private::Private( const Private& other )
    : QSharedData()
{
    Q_FOREACH( BooleanExpression* condition, other.conditions ) {
        conditions.append( condition->clone() );
    }
}

Soprano::Query::BooleanSetExpression::Private::~Private()
{
    Q_FOREACH( BooleanExpression* condition, conditions ) {
        delete condition;
    }
}

Soprano::Query::BooleanSetExpression::BooleanSetExpression()
    : d( new Private() )
{
}

void Soprano::Query::BooleanSetExpression::addConditon( BooleanExpression *first )
{
    d->conditions.append( first );
}

int Soprano::Query::BooleanSetExpression::count() const
{
    return d->conditions.count();
}

const Soprano::Query::BooleanExpression* Soprano::Query::BooleanSetExpression::condition( int i ) const
{
    return d->conditions[i];
}

const Soprano::Query::BooleanExpression* Soprano::Query::BooleanSetExpression::operator[]( int i ) const
{
    return d->conditions[i];
}

//
// Soprano::Query::BinaryNumericalBooleanExpression
//

Soprano::Query::BinaryNumericalBooleanExpression::Private::Private( NumericalExpression *first_,
                                                                    NumericalExpression *second_ )
    : QSharedData(),
      first( first_ ),
      second( second_ )
{
}

Soprano::Query::BinaryNumericalBooleanExpression::Private::Private( const Private& other )
    : QSharedData(),
      first( 0 ),
      second( 0 )
{
    if ( other.first ) {
        first = other.first->clone();
    }
    if ( other.second ) {
        second = other.second->clone();
    }
}

Soprano::Query::BinaryNumericalBooleanExpression::Private::~Private()
{
    delete first;
    delete second;
}

Soprano::Query::BinaryNumericalBooleanExpression::BinaryNumericalBooleanExpression( NumericalExpression *first,
                                                                                    NumericalExpression *second )
    : BooleanExpression(),
      d( new Private( first, second ) )
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

Soprano::Query::BinaryStringBooleanExpression::Private::Private( StringExpression *first_,
                                                                 StringExpression *second_ )
    : QSharedData(),
      first( first_ ),
      second( second_ )
{
}

Soprano::Query::BinaryStringBooleanExpression::Private::Private( const Private& other )
    : QSharedData(),
      first( 0 ),
      second( 0 )
{
    if ( other.first ) {
        first = other.first->clone();
    }
    if ( other.second ) {
        second = other.second->clone();
    }
}

Soprano::Query::BinaryStringBooleanExpression::Private::~Private()
{
    delete first;
    delete second;
}

Soprano::Query::BinaryStringBooleanExpression::BinaryStringBooleanExpression( StringExpression *first,
                                                                              StringExpression *second )
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

Soprano::Query::BinaryDateTimeBooleanExpression::Private::Private(
    const QDateTime first_, const QDateTime& second_ )
    : first( first_ ),
      second( second_ ) {
}

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

Soprano::Query::BinaryRTermBooleanExpression::Private::Private( RTerm *first_,
                                                                RTerm *second_ )
    : QSharedData(),
      first( first_ ),
      second( second_ )
{
}

Soprano::Query::BinaryRTermBooleanExpression::Private::Private( const Private& other )
    : QSharedData(),
      first( 0 ),
      second( 0 )
{
    if ( other.first ) {
        first = other.first->clone();
    }
    if ( other.second ) {
        second = other.second->clone();
    }
}

Soprano::Query::BinaryRTermBooleanExpression::Private::~Private()
{
    delete first;
    delete second;
}

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

Soprano::Query::BinaryNumericalExpression::Private::Private( NumericalExpression *first_,
                                                             NumericalExpression *second_ )
    : QSharedData(),
      first( first_ ),
      second( second_ )
{
}

Soprano::Query::BinaryNumericalExpression::Private::Private( const Private& other )
    : QSharedData(),
      first( 0 ),
      second( 0 )
{
    if ( other.first ) {
        first = other.first->clone();
    }
    if ( other.second ) {
        second = other.second->clone();
    }
}

Soprano::Query::BinaryNumericalExpression::Private::~Private()
{
    delete first;
    delete second;
}

Soprano::Query::BinaryNumericalExpression::BinaryNumericalExpression( NumericalExpression *first,
                                                                      NumericalExpression *second )
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

Soprano::Query::LogicOr::LogicOr()
    : BooleanSetExpression()
{
}

void Soprano::Query::LogicOr::accept( ExpressionVisitor *visitor ) const
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

Soprano::Query::LogicAnd::LogicAnd()
    :BooleanSetExpression()
{
}

void Soprano::Query::LogicAnd::accept( ExpressionVisitor *visitor ) const
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

void Soprano::Query::NumericalEqual::accept( ExpressionVisitor *visitor ) const
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

void Soprano::Query::NumericalNotEqual::accept( ExpressionVisitor *visitor ) const
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

void Soprano::Query::StringEqual::accept( ExpressionVisitor *visitor ) const
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

void Soprano::Query::StringNotEqual::accept( ExpressionVisitor *visitor ) const
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

void Soprano::Query::DateTimeEqual::accept( ExpressionVisitor *visitor ) const
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

void Soprano::Query::DateTimeNotEqual::accept( ExpressionVisitor *visitor ) const
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

void Soprano::Query::NumericalLessThan::accept( ExpressionVisitor *visitor ) const
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

void Soprano::Query::NumericalGreaterThan::accept( ExpressionVisitor *visitor ) const
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

void Soprano::Query::NumericalLessThanEqual::accept( ExpressionVisitor *visitor ) const
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

void Soprano::Query::NumericalGreaterThanEqual::accept( ExpressionVisitor *visitor ) const
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

void Soprano::Query::StringLessThan::accept( ExpressionVisitor *visitor ) const
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

void Soprano::Query::StringGreaterThan::accept( ExpressionVisitor *visitor ) const
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

void Soprano::Query::StringLessThanEqual::accept( ExpressionVisitor *visitor ) const
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

void Soprano::Query::StringGreaterThanEqual::accept( ExpressionVisitor *visitor ) const
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

void Soprano::Query::DateTimeLessThan::accept( ExpressionVisitor *visitor ) const
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

void Soprano::Query::DateTimeGreaterThan::accept( ExpressionVisitor *visitor ) const
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

void Soprano::Query::DateTimeLessThanEqual::accept( ExpressionVisitor *visitor ) const
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

void Soprano::Query::DateTimeGreaterThanEqual::accept( ExpressionVisitor *visitor ) const
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

void Soprano::Query::NumericalMultiply::accept( ExpressionVisitor *visitor ) const
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

void Soprano::Query::NumericalDivide::accept( ExpressionVisitor *visitor ) const
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

void Soprano::Query::NumericalAdd::accept( ExpressionVisitor *visitor ) const
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

void Soprano::Query::NumericalSubtract::accept( ExpressionVisitor *visitor ) const
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

void Soprano::Query::RTermEqual::accept( ExpressionVisitor *visitor ) const
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

void Soprano::Query::RTermNotEqual::accept( ExpressionVisitor *visitor ) const
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

void Soprano::Query::LangMatches::accept( ExpressionVisitor *visitor ) const
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

Soprano::Query::Regexp::Private::Private()
    : QSharedData(),
      expression(0)
{
}

Soprano::Query::Regexp::Private::Private( StringExpression *ex, const QString &p, const QString& f )
    : QSharedData(),
      expression( ex ),
      pattern( p ),
      flags( f )
{
}

Soprano::Query::Regexp::Private::Private( const Private& other)
    : QSharedData(),
      expression( 0 ),
      pattern( other.pattern ),
      flags( other.flags )
{
    if ( other.expression ) {
        expression = other.expression->clone();
    }
}

Soprano::Query::Regexp::Private::~Private()
{
    delete expression;
}

Soprano::Query::Regexp::Regexp()
    : BooleanExpression()
{
    d = new Private();
}

Soprano::Query::Regexp::Regexp( StringExpression *expression, const QString &pattern )
    : BooleanExpression(),
      d( new Private( expression, pattern ) )
{
}

Soprano::Query::Regexp::Regexp( StringExpression *expression, const QString &pattern, const QString &flags )
    : BooleanExpression(),
      d( new Private( expression, pattern, flags ) )
{
}

Soprano::Query::Regexp::Regexp( const Regexp& other )
    : BooleanExpression()
{
    d = other.d;
}

Soprano::Query::Regexp::~Regexp()
{
}

Soprano::Query::Regexp& Soprano::Query::Regexp::operator=( const Regexp& other )
{
    d = other.d;
    return *this;
}

void Soprano::Query::Regexp::setExpression( StringExpression *expression )
{
    if ( expression != d->expression ) {
        delete d->expression;
        d->expression = expression;
    }
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

Soprano::Query::Regexp* Soprano::Query::Regexp::clone() const
{
    return new Regexp( *this );
}

void Soprano::Query::Regexp::accept( ExpressionVisitor *visitor ) const
{
    visitor->visit( this );
}

//
// Soprano::Query::Prefix
//

Soprano::Query::Prefix::Private::Private() {
}

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

Soprano::Query::Prefix::~Prefix()
{
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
// Soprano::Query::TriplePattern
//

Soprano::Query::TriplePattern::Private::Private()
  : QSharedData(),
    subject( 0 ),
    predicate( 0 ),
    object( 0 )
{
}

Soprano::Query::TriplePattern::Private::Private( const Private& other )
  : QSharedData(),
    subject( 0 ),
    predicate( 0 ),
    object( 0 )
{
    if ( other.subject ) {
        subject = other.subject->clone();
    }
    if ( other.predicate ) {
        predicate = other.predicate->clone();
    }
    if ( other.object ) {
        object = other.object->clone();
    }
}

Soprano::Query::TriplePattern::Private::~Private()
{
    delete subject;
    delete predicate;
    delete object;
}

Soprano::Query::TriplePattern::TriplePattern()
    : BooleanExpression()
{
    d = new Private();
}

Soprano::Query::TriplePattern::TriplePattern( RTerm* subject, RTerm* predicate, RTerm* object )
    : BooleanExpression()
{
    d = new Private();
    d->subject = subject;
    d->predicate = predicate;
    d->object = object;
}

Soprano::Query::TriplePattern::TriplePattern( const TriplePattern &other )
    : BooleanExpression()
{
    d = other.d;
}

Soprano::Query::TriplePattern::~TriplePattern()
{
}

Soprano::Query::TriplePattern& Soprano::Query::TriplePattern::operator=( const TriplePattern& other )
{
    d = other.d;
    return *this;
}

void Soprano::Query::TriplePattern::setSubject( RTerm *subject )
{
    delete d->subject;
    d->subject = subject;
}

const Soprano::Query::RTerm *Soprano::Query::TriplePattern::subject() const
{
    return d->subject;
}

void Soprano::Query::TriplePattern::setPredicate( RTerm *predicate )
{
    delete d->predicate;
    d->predicate = predicate;
}

const Soprano::Query::RTerm *Soprano::Query::TriplePattern::predicate() const
{
   return d->predicate;
}

void Soprano::Query::TriplePattern::setObject( RTerm *object )
{
   delete d->object;
   d->object = object;
}

const Soprano::Query::RTerm *Soprano::Query::TriplePattern::object() const
{
   return d->object;
}

Soprano::Query::TriplePattern* Soprano::Query::TriplePattern::clone() const
{
    return new TriplePattern( *this );
}

void Soprano::Query::TriplePattern::accept( ExpressionVisitor *visitor) const
{
    visitor->visit( this );
}


//
// Soprano::Query::Query
//

Soprano::Query::Query::Private::Private()
    : QSharedData(),
      condition(0)
{
}

Soprano::Query::Query::Private::Private( const Private& other )
    : QSharedData(),
      condition(0),
      queryTerms( other.queryTerms ),
      type( other.type )
{
    if ( other.condition ) {
        condition = other.condition->clone();
    }
}

Soprano::Query::Query::Private::~Private()
{
    delete condition;
}

Soprano::Query::Query::Query()
{
   d = new Private();
}

Soprano::Query::Query::Query( QueryType type )
{
   d = new Private();
   d->type = type;
}

Soprano::Query::Query::Query( const Query &other )
{
    d = other.d;
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

const Soprano::Query::QueryTerms Soprano::Query::Query::queryTerms() const
{
    return d->queryTerms;
}

bool Soprano::Query::Query::isValid() const
{
    return type() == INVALID_QUERY;
}


//
// Soprano::Query::QueryTerms
//

Soprano::Query::QueryTerms::Private::Private()
    : QSharedData()
{
}

Soprano::Query::QueryTerms::Private::Private( const Private& other)
    : QSharedData()
{
   Q_FOREACH( Variable* v, other.variables ) {
      variables.append( v->clone() );
   }
}

Soprano::Query::QueryTerms::Private::~Private()
{
   Q_FOREACH( Variable* r, variables ) {
      delete r;
   }
}

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


void Soprano::Query::QueryTerms::addVariable( Variable *variable )
{
   d->variables.append( variable );
}

QList<const Soprano::Query::Variable*> Soprano::Query::QueryTerms::variables() const
{
    QList<const Variable*> l;
    Q_FOREACH( Variable* v, d->variables ) {
        l.append( v );
    }
    return l;
}

bool Soprano::Query::QueryTerms::selectAll() const
{
    return d->variables.isEmpty();
}
