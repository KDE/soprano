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

//
// Variable
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

QString Soprano::Query::Variable::name() const
{
    return d->name;
}

//
// RTerm
//

class Soprano::Query::RTerm::Private {
public:
    Private() {};

    Soprano::Node node;
    Variable binding;
};

Soprano::Query::RTerm::RTerm( const Soprano::Node &node )
{
    d = new Private;
    d->node = node;
}

Soprano::Query::RTerm::RTerm( const Variable &binding )
{
    d = new Private;
    d->binding = binding;
}

Soprano::Query::RTerm::~RTerm()
{
    delete d;
}

bool Soprano::Query::RTerm::isBinding() const
{
    return !d->node.isValid();
}

Soprano::Node Soprano::Query::RTerm::node() const
{
    return d->node;
}

Soprano::Query::Variable Soprano::Query::RTerm::binding() const
{
    return d->binding;
}

//
// Integer
//

Soprano::Query::Integer::Integer( int value )
    :m_value(value)
{
}

Soprano::Query::Integer::Integer( const Integer &other )
{
    m_value = other.m_value;
}

int Soprano::Query::Integer::value() const
{
    return m_value;
}

//
// Float
//

Soprano::Query::Float::Float( float value )
    :m_value(value)
{
}

Soprano::Query::Float::Float( const Float &other )
{
    m_value = other.m_value;
}

float Soprano::Query::Float::value() const
{
    return m_value;
}

//
// Double
//

Soprano::Query::Double::Double( double value )
    :m_value(value)
{
}

Soprano::Query::Double::Double( const Double &other )
{
    m_value = other.m_value;
}

double Soprano::Query::Double::value() const
{
    return m_value;
}

////////////////////////////////////////////////////////////////////////
// Unary Expressions                                                  //
////////////////////////////////////////////////////////////////////////

Soprano::Query::UnaryBooleanExpression::UnaryBooleanExpression( BooleanExpression *expression )
    :m_expression( expression )
{
}

void Soprano::Query::UnaryBooleanExpression::setExpression( BooleanExpression *expression )
{
    m_expression = expression;
}

Soprano::Query::BooleanExpression *Soprano::Query::UnaryBooleanExpression::expression()
{
    return m_expression;
}


Soprano::Query::UnaryRTermBooleanExpression::UnaryRTermBooleanExpression( RTerm *rterm )
    :m_rterm(rterm)
{
}

void Soprano::Query::UnaryRTermBooleanExpression::setRTerm( RTerm *rterm )
{
    m_rterm = rterm;
}

Soprano::Query::RTerm *Soprano::Query::UnaryRTermBooleanExpression::rterm()
{
    return m_rterm;
}


Soprano::Query::UnaryRTermStringExpression::UnaryRTermStringExpression( RTerm *rterm )
    :m_rterm(rterm)
{
}

void Soprano::Query::UnaryRTermStringExpression::setRTerm( RTerm *rterm )
{
    m_rterm = rterm;
}

Soprano::Query::RTerm *Soprano::Query::UnaryRTermStringExpression::rterm()
{
    return m_rterm;
}


Soprano::Query::UnaryNumericalExpression::UnaryNumericalExpression( NumericalExpression *expression )
    :m_expression( expression )
{
}

void Soprano::Query::UnaryNumericalExpression::setExpression( NumericalExpression *expression )
{
    m_expression = expression;
}

Soprano::Query::NumericalExpression *Soprano::Query::UnaryNumericalExpression::expression()
{
    return m_expression;
}


Soprano::Query::Not::Not( BooleanExpression *expression )
    :UnaryBooleanExpression( expression )
{
}

void Soprano::Query::Not::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}


Soprano::Query::UnaryPlus::UnaryPlus( NumericalExpression *expression )
    :UnaryNumericalExpression( expression )
{
}

void Soprano::Query::UnaryPlus::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}


Soprano::Query::UnaryMinus::UnaryMinus( NumericalExpression *expression )
    :UnaryNumericalExpression( expression )
{
}

void Soprano::Query::UnaryMinus::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}


Soprano::Query::IsBound::IsBound( Variable *variable )
    :m_variable( variable )
{
}

void Soprano::Query::IsBound::setVariable( Variable *variable )
{
    m_variable = variable;
}

Soprano::Query::Variable *Soprano::Query::IsBound::variable()
{
    return m_variable;
}

void Soprano::Query::IsBound::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}


Soprano::Query::IsIRI::IsIRI( RTerm *rterm )
    :UnaryRTermBooleanExpression( rterm )
{
}

void Soprano::Query::IsIRI::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}


Soprano::Query::IsBlank::IsBlank( RTerm *rterm )
    :UnaryRTermBooleanExpression( rterm )
{
}

void Soprano::Query::IsBlank::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}


Soprano::Query::IsLiteral::IsLiteral( RTerm *rterm )
    :UnaryRTermBooleanExpression( rterm )
{
}

void Soprano::Query::IsLiteral::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}


Soprano::Query::StringValue::StringValue( RTerm *rterm )
    :UnaryRTermStringExpression( rterm )
{
}

void Soprano::Query::StringValue::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}


Soprano::Query::LangValue::LangValue( RTerm *rterm )
    :UnaryRTermStringExpression( rterm )
{
}

void Soprano::Query::LangValue::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}


Soprano::Query::DataTypeValue::DataTypeValue( RTerm *rterm )
    :UnaryRTermStringExpression( rterm )
{
}

void Soprano::Query::DataTypeValue::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}


////////////////////////////////////////////////////////////////////////
// Binary Expression                                                  //
////////////////////////////////////////////////////////////////////////


Soprano::Query::BinaryBooleanExpression::BinaryBooleanExpression( BooleanExpression *first, BooleanExpression *second )
    :m_first(first), m_second(second)
{
}

void Soprano::Query::BinaryBooleanExpression::setFirst( BooleanExpression *first )
{
    m_first = first;
}

void Soprano::Query::BinaryBooleanExpression::setSecond( BooleanExpression *second )
{
    m_second = second;
}

Soprano::Query::BooleanExpression *Soprano::Query::BinaryBooleanExpression::first()
{
   return m_first;
}

Soprano::Query::BooleanExpression *Soprano::Query::BinaryBooleanExpression::second()
{
   return m_second;
}


Soprano::Query::BinaryNumericalBooleanExpression::BinaryNumericalBooleanExpression( NumericalExpression *first, NumericalExpression *second )
    :m_first(first), m_second(second)
{
}

void Soprano::Query::BinaryNumericalBooleanExpression::setFirst( NumericalExpression *first )
{
    m_first = first;
}

void Soprano::Query::BinaryNumericalBooleanExpression::setSecond( NumericalExpression *second )
{
    m_second = second;
}

Soprano::Query::NumericalExpression *Soprano::Query::BinaryNumericalBooleanExpression::first()
{
   return m_first;
}

Soprano::Query::NumericalExpression *Soprano::Query::BinaryNumericalBooleanExpression::second()
{
   return m_second;
}


Soprano::Query::BinaryStringBooleanExpression::BinaryStringBooleanExpression( StringExpression *first, StringExpression *second )
    :m_first(first), m_second(second)
{
}

void Soprano::Query::BinaryStringBooleanExpression::setFirst( StringExpression *first )
{
    m_first = first;
}

void Soprano::Query::BinaryStringBooleanExpression::setSecond( StringExpression *second )
{
    m_second = second;
}

Soprano::Query::StringExpression *Soprano::Query::BinaryStringBooleanExpression::first()
{
   return m_first;
}

Soprano::Query::StringExpression *Soprano::Query::BinaryStringBooleanExpression::second()
{
   return m_second;
}


Soprano::Query::BinaryDateTimeBooleanExpression::BinaryDateTimeBooleanExpression( QDateTime *first, QDateTime *second )
    :m_first(first), m_second(second)
{
}

void Soprano::Query::BinaryDateTimeBooleanExpression::setFirst( QDateTime *first )
{
    m_first = first;
}

void Soprano::Query::BinaryDateTimeBooleanExpression::setSecond( QDateTime *second )
{
    m_second = second;
}

QDateTime *Soprano::Query::BinaryDateTimeBooleanExpression::first()
{
   return m_first;
}

QDateTime *Soprano::Query::BinaryDateTimeBooleanExpression::second()
{
   return m_second;
}


Soprano::Query::BinaryRTermBooleanExpression::BinaryRTermBooleanExpression( RTerm *first, RTerm *second )
    :m_first(first), m_second(second)
{
}

void Soprano::Query::BinaryRTermBooleanExpression::setFirst( RTerm *first )
{
    m_first = first;
}

void Soprano::Query::BinaryRTermBooleanExpression::setSecond( RTerm *second )
{
    m_second = second;
}

Soprano::Query::RTerm *Soprano::Query::BinaryRTermBooleanExpression::first()
{
   return m_first;
}

Soprano::Query::RTerm *Soprano::Query::BinaryRTermBooleanExpression::second()
{
   return m_second;
}


Soprano::Query::BinaryNumericalExpression::BinaryNumericalExpression( NumericalExpression *first, NumericalExpression *second )
    :m_first(first), m_second(second)
{
}

void Soprano::Query::BinaryNumericalExpression::setFirst( NumericalExpression *first )
{
    m_first = first;
}

void Soprano::Query::BinaryNumericalExpression::setSecond( NumericalExpression *second )
{
    m_second = second;
}

Soprano::Query::NumericalExpression *Soprano::Query::BinaryNumericalExpression::first()
{
   return m_first;
}

Soprano::Query::NumericalExpression *Soprano::Query::BinaryNumericalExpression::second()
{
   return m_second;
}


Soprano::Query::LogicOr::LogicOr( BooleanExpression *first, BooleanExpression *second )
    :BinaryBooleanExpression( first, second )
{
}

void Soprano::Query::LogicOr::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}


Soprano::Query::LogicAnd::LogicAnd( BooleanExpression *first, BooleanExpression *second )
    :BinaryBooleanExpression( first, second )
{
}

void Soprano::Query::LogicAnd::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}


Soprano::Query::NumericalEqual::NumericalEqual( NumericalExpression *first, NumericalExpression *second )
    :BinaryNumericalBooleanExpression( first, second )
{
}

void Soprano::Query::NumericalEqual::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}


Soprano::Query::NumericalNotEqual::NumericalNotEqual( NumericalExpression *first, NumericalExpression *second )
    :BinaryNumericalBooleanExpression( first, second )
{
}

void Soprano::Query::NumericalNotEqual::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}


Soprano::Query::StringEqual::StringEqual( StringExpression *first, StringExpression *second )
    :BinaryStringBooleanExpression( first, second )
{
}

void Soprano::Query::StringEqual::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}


Soprano::Query::StringNotEqual::StringNotEqual( StringExpression *first, StringExpression *second )
    :BinaryStringBooleanExpression( first, second )
{
}

void Soprano::Query::StringNotEqual::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}


Soprano::Query::DateTimeEqual::DateTimeEqual( QDateTime *first, QDateTime *second )
    :BinaryDateTimeBooleanExpression( first, second )
{
}

void Soprano::Query::DateTimeEqual::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}


Soprano::Query::DateTimeNotEqual::DateTimeNotEqual( QDateTime *first, QDateTime *second )
    :BinaryDateTimeBooleanExpression( first, second )
{
}

void Soprano::Query::DateTimeNotEqual::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}


Soprano::Query::NumericalLessThan::NumericalLessThan( NumericalExpression *first, NumericalExpression *second )
    :BinaryNumericalBooleanExpression( first, second )
{
}

void Soprano::Query::NumericalLessThan::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}


Soprano::Query::NumericalGreaterThan::NumericalGreaterThan( NumericalExpression *first, NumericalExpression *second )
    :BinaryNumericalBooleanExpression( first, second )
{
}

void Soprano::Query::NumericalGreaterThan::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}


Soprano::Query::NumericalLessThanEqual::NumericalLessThanEqual( NumericalExpression *first, NumericalExpression *second )
    :BinaryNumericalBooleanExpression( first, second )
{
}

void Soprano::Query::NumericalLessThanEqual::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}


Soprano::Query::NumericalGreaterThanEqual::NumericalGreaterThanEqual( NumericalExpression *first, NumericalExpression *second )
    :BinaryNumericalBooleanExpression( first, second )
{
}

void Soprano::Query::NumericalGreaterThanEqual::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}


Soprano::Query::StringLessThan::StringLessThan( StringExpression *first, StringExpression *second )
    :BinaryStringBooleanExpression( first, second )
{
}

void Soprano::Query::StringLessThan::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}


Soprano::Query::StringGreaterThan::StringGreaterThan( StringExpression *first, StringExpression *second )
    :BinaryStringBooleanExpression( first, second )
{
}

void Soprano::Query::StringGreaterThan::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}


Soprano::Query::StringLessThanEqual::StringLessThanEqual( StringExpression *first, StringExpression *second )
    :BinaryStringBooleanExpression( first, second )
{
}

void Soprano::Query::StringLessThanEqual::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}


Soprano::Query::StringGreaterThanEqual::StringGreaterThanEqual( StringExpression *first, StringExpression *second )
    :BinaryStringBooleanExpression( first, second )
{
}

void Soprano::Query::StringGreaterThanEqual::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}


Soprano::Query::DateTimeLessThan::DateTimeLessThan( QDateTime *first, QDateTime *second )
    :BinaryDateTimeBooleanExpression( first, second )
{
}

void Soprano::Query::DateTimeLessThan::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}


Soprano::Query::DateTimeGreaterThan::DateTimeGreaterThan( QDateTime *first, QDateTime *second )
    :BinaryDateTimeBooleanExpression( first, second )
{
}

void Soprano::Query::DateTimeGreaterThan::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}


Soprano::Query::DateTimeLessThanEqual::DateTimeLessThanEqual( QDateTime *first, QDateTime *second )
    :BinaryDateTimeBooleanExpression( first, second )
{
}

void Soprano::Query::DateTimeLessThanEqual::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}


Soprano::Query::DateTimeGreaterThanEqual::DateTimeGreaterThanEqual( QDateTime *first, QDateTime *second )
    :BinaryDateTimeBooleanExpression( first, second )
{
}

void Soprano::Query::DateTimeGreaterThanEqual::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}


Soprano::Query::NumericalMultiply::NumericalMultiply( NumericalExpression *first, NumericalExpression *second )
    :BinaryNumericalExpression( first, second )
{
}

void Soprano::Query::NumericalMultiply::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}


Soprano::Query::NumericalDivide::NumericalDivide( NumericalExpression *first, NumericalExpression *second )
    :BinaryNumericalExpression( first, second )
{
}

void Soprano::Query::NumericalDivide::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}


Soprano::Query::NumericalAdd::NumericalAdd( NumericalExpression *first, NumericalExpression *second )
    :BinaryNumericalExpression( first, second )
{
}

void Soprano::Query::NumericalAdd::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}


Soprano::Query::NumericalSubtract::NumericalSubtract( NumericalExpression *first, NumericalExpression *second )
    :BinaryNumericalExpression( first, second )
{
}

void Soprano::Query::NumericalSubtract::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}


Soprano::Query::RTermEqual::RTermEqual( RTerm *first, RTerm *second )
    :BinaryRTermBooleanExpression( first, second )
{
}

void Soprano::Query::RTermEqual::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}


Soprano::Query::RTermNotEqual::RTermNotEqual( RTerm *first, RTerm *second )
    :BinaryRTermBooleanExpression( first, second )
{
}

void Soprano::Query::RTermNotEqual::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}


Soprano::Query::LangMatches::LangMatches( StringExpression *first, StringExpression *second )
    :BinaryStringBooleanExpression( first, second )
{
}

void Soprano::Query::LangMatches::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}


Soprano::Query::Regexp::Regexp( StringExpression *expression, const QString &pattern )
    :m_expression(expression), m_pattern(pattern)
{
}

Soprano::Query::Regexp::Regexp( StringExpression *expression, const QString &pattern, const QString &flags )
    :m_expression(expression), m_pattern(pattern), m_flags(flags)
{
}

void Soprano::Query::Regexp::setExpression( StringExpression *expression )
{
    m_expression = expression;
}

Soprano::Query::StringExpression *Soprano::Query::Regexp::expression()
{
    return m_expression;
}

void Soprano::Query::Regexp::setPattern( const QString &pattern )
{
    m_pattern = pattern;
}

QString Soprano::Query::Regexp::pattern()
{
    return m_pattern;
}

void Soprano::Query::Regexp::setFlags( const QString &flags )
{
    m_flags = flags;
}

QString Soprano::Query::Regexp::flags()
{
    return m_flags;
}

void Soprano::Query::Regexp::accept( ExpressionVisitor *visitor )
{
    visitor->visit( this );
}

//
// QueryObject
//

Soprano::Query::QueryObject::QueryObject()
    :m_wildCard(false)
{
}

void Soprano::Query::QueryObject::setQueryVerb( const QString &queryVerb )
{
    m_queryVerb = queryVerb;
}

QString Soprano::Query::QueryObject::queryVerb()
{
    return m_queryVerb;
}

void Soprano::Query::QueryObject::setWildCard( bool wildCard )
{
    m_wildCard = wildCard;
}

bool Soprano::Query::QueryObject::isWildCard()
{
    return m_wildCard;
}


////////////////////////////////////////////////////////////////////////
// QueryObject                                                        //
////////////////////////////////////////////////////////////////////////

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

const QString Soprano::Query::Prefix::prefix() const
{
    return d->prefix;
}

const QUrl Soprano::Query::Prefix::uri() const
{
    return d->uri;
}

