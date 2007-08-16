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
// RTerm
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

Soprano::Query::Variable& Soprano::Query::Variable::operator=( const Variable& other )
{
    d = other.d;
    return *this;
}

QString Soprano::Query::Variable::name() const
{
    return d->name;
}


//
// Numerical
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
// Node
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


Soprano::Query::Node& Soprano::Query::Node::operator=( const Node& other )
{
    d = other.d;
    return *this;
}

Soprano::Node Soprano::Query::Node::node() const
{
    return d->node;
}


////////////////////////////////////////////////////////////////////////
// Expressions                                                        //
////////////////////////////////////////////////////////////////////////

Soprano::Query::Expression::Expression()
{
}

Soprano::Query::Expression::~Expression()
{
}

Soprano::Query::BooleanExpression::BooleanExpression()
{
}

Soprano::Query::BooleanExpression::~BooleanExpression()
{
}

Soprano::Query::NumericalExpression::NumericalExpression()
{
}

Soprano::Query::NumericalExpression::~NumericalExpression()
{
}

Soprano::Query::StringExpression::StringExpression()
{
}

Soprano::Query::StringExpression::~StringExpression()
{
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

////////////////////////////////////////////////////////////////////////
// Query                                                              //
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
    Private(): condition(0) {};

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
