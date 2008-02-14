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

#ifndef SOPRANO_QUERY_API_H
#define SOPRANO_QUERY_API_H

#include <QtCore/QUrl>
#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QDateTime>
#include <QtCore/QVariant>
#include <QtCore/QSharedData>

#include "node.h"
#include "soprano_export.h"

// FIXME: - set operations such as intersect
//        - I think it would be sufficient to have equal on expressions rather
//          then one for strings and one or numericals and so on
//        - String operations such as concat
//        - We have no way of representing OFFSET or LIMIT yet
//        - There is no way to convert a numerical into string for comparision
//        - Numerical is completely useless ATM as there is no way to compare it
//          to anything but other numericals, i.e. const expressions.

namespace Soprano
{
    namespace Query {

        class ExpressionVisitor;

        class SOPRANO_EXPORT Expression {
        public:
            virtual ~Expression();
        
            // FIXME: if we want to keep the visitor pattern stuff we need two methods:
            // one const and one non-const, the const one should call a visitor method
            // with a const parameter. It is ugly but I see no other way to keep this stuff.
            virtual void accept( ExpressionVisitor *visitor ) const = 0;

            virtual Expression* clone() const = 0;
            
        protected:
            Expression();
        };
    
        class SOPRANO_EXPORT RTerm {
        public:
            virtual ~RTerm();

            virtual RTerm* clone() const = 0;

            virtual bool isVariable() const;
            virtual bool isNode() const;

        protected:
            RTerm();
        };


        class SOPRANO_EXPORT Variable : public RTerm {
        public:
            Variable();
            Variable( const QString &name );
            Variable( const Variable &other );
            ~Variable();

            Variable& operator=( const Variable& );

            Variable* clone() const;

            bool isVariable() const { return true; }

            QString name() const;

        private:
            class Private;
            QSharedDataPointer<Private> d;
        };

        class SOPRANO_EXPORT Node : public RTerm {
        public:
            Node();
            Node( const Soprano::Node &node );
            Node( const Node &other );

            Node* clone() const;
     
            Node& operator=( const Node& );
     
            bool isNode() const { return true; }

            Soprano::Node node() const;
        
        private:
            class Private : public QSharedData {
            public:
                Private();
                Soprano::Node node;
            };
            QSharedDataPointer<Private> d;
        };
        
    
        ////////////////////////////////////////////////////////////////////////
        // Expressions                                                        //
        ////////////////////////////////////////////////////////////////////////


        // An expression that return a xsd:boolean
        class SOPRANO_EXPORT BooleanExpression: public Expression {
        public:
            virtual ~BooleanExpression();

            virtual BooleanExpression* clone() const = 0;

        protected:
            BooleanExpression();
        };

        class Numerical;

        // An expression that return a xsd:integer, xsd:decimal, xsd:float, and xsd:double
        class SOPRANO_EXPORT NumericalExpression: public Expression {
        public:
            virtual ~NumericalExpression();

            NumericalExpression* clone() const = 0;

        protected:
            NumericalExpression();
        };

        /**
         * A numerical constant.
         */
        // FIXME: we also need a DateTime expression. Could we also use a generic LiteralExpression
        //        instead? After all Numerical already forces a switch statement. Why not adding some
        //        more? But then what is the difference between Query::Node with a LiteralValue node
        //        and a Query::LiteralValue?
        class SOPRANO_EXPORT Numerical : public NumericalExpression {
        public:
            Numerical();
            Numerical( int    value );
            Numerical( double value );
            Numerical( float  value );
            Numerical( const Numerical &other );
            ~Numerical();
        
            Numerical& operator=( const Numerical& );

            Numerical* clone() const;

            void accept( ExpressionVisitor *visitor ) const;
        
            bool isDecimal();

            bool isDouble();
            double doubleValue();

            bool isFloat();
            float floatValue();
            
            bool isInteger();
            int integerValue();
            
        private:
            class Private : public QSharedData
            {
            public:
                Private( const QVariant& v = QVariant() );
                QVariant value;
            };
            QSharedDataPointer<Private> d;
        };
   
        // An expression that return a xsd:string or rdfs:Datatype
        class SOPRANO_EXPORT StringExpression: public Expression {
        public:
            virtual ~StringExpression();

            StringExpression* clone() const = 0;

        protected:
            StringExpression();
        };

        /**
         * A string constant
         */
        class SOPRANO_EXPORT String : public StringExpression {
        public:
            String();
            String( const QString &value );
            String( const String &other );

            String* clone() const;

            QString value() const;
            void setValue( const QString &value );
    
            void accept( ExpressionVisitor *visitor ) const;

        private:
            class Private : public QSharedData
            {
            public:
                Private( const QString& s = QString() );
                QString value;
            };
            QSharedDataPointer<Private> d;
        };

        ////////////////////////////////////////////////////////////////////////
        // Unary Expressions                                                  //
        ////////////////////////////////////////////////////////////////////////

        class SOPRANO_EXPORT UnaryBooleanExpression: public BooleanExpression {
        public:
            UnaryBooleanExpression( BooleanExpression *expression );

            void setExpression( BooleanExpression *expression );
            const BooleanExpression *expression() const;

        private:
            class Private : public QSharedData
            {
            public:
                Private( BooleanExpression* e = 0 );
                Private( const Private& other );
                ~Private();
                BooleanExpression* expression;
            };
            QSharedDataPointer<Private> d;
        };

        class SOPRANO_EXPORT UnaryRTermExpressionBase {
        public:
            UnaryRTermExpressionBase( RTerm *rterm );
        
            void setRTerm( RTerm *expression );
            const RTerm *rterm() const;

        private:
            class Private : public QSharedData
            {
            public:
                Private( RTerm* e = 0 );
                Private( const Private& other );
                ~Private();
                RTerm* term;
            };
            QSharedDataPointer<Private> d;
        };

        class SOPRANO_EXPORT UnaryRTermBooleanExpression: public BooleanExpression, UnaryRTermExpressionBase {
        public:
            UnaryRTermBooleanExpression( RTerm *rterm );
        };

        class SOPRANO_EXPORT UnaryRTermStringExpression: public StringExpression, UnaryRTermExpressionBase {
        public:
            UnaryRTermStringExpression( RTerm *rterm );
        };

        class SOPRANO_EXPORT UnaryRTermNumericalExpression: public NumericalExpression, UnaryRTermExpressionBase {
        public:
            UnaryRTermNumericalExpression( RTerm *rterm );
        };

        class SOPRANO_EXPORT UnaryNumericalExpression: public NumericalExpression {
        public:
            UnaryNumericalExpression( NumericalExpression *expression );

            void setExpression( NumericalExpression *expression );
            const NumericalExpression *expression() const;

        private:
            class Private : public QSharedData
            {
            public:
                Private( NumericalExpression* e = 0 );
                Private( const Private& other );
                ~Private();
                NumericalExpression* expression;
            };
            QSharedDataPointer<Private> d;
        };

        /* XQUERY Tests */

        class SOPRANO_EXPORT Not: public UnaryBooleanExpression {
        public:
            Not();
            Not( BooleanExpression *expression );
            ~Not();
        
            Not* clone() const;
        
            void accept( ExpressionVisitor *visitor ) const;
        };

        class Negate: public UnaryNumericalExpression  {
        public:
            Negate();
            Negate( NumericalExpression *expression );
            ~Negate();

            Negate* clone() const;
        
            void accept( ExpressionVisitor *visitor ) const;
        };

        /* 
         * SPARQL Tests as defined in
         * - http://www.w3.org/TR/2005/WD-rdf-sparql-query-20051123/#SparqlOps
         */

        class SOPRANO_EXPORT IsBound: public BooleanExpression {
        public:
            IsBound();
            IsBound( Variable *variable );
            IsBound( const IsBound &other );
            virtual ~IsBound();
            
            IsBound operator=( const IsBound &other );

            void setVariable( Variable *variable );
            const Variable *variable() const;

            IsBound* clone() const;
            
            void accept( ExpressionVisitor *visitor ) const;
            
        private:
            class Private : public QSharedData
            {
            public:
                Private( Variable* v = 0 );
                Private( const Private& other );
                ~Private();
                Variable* variable;
            };
            QSharedDataPointer<Private> d;
        };

        class SOPRANO_EXPORT IsIRI: public UnaryRTermBooleanExpression {
        public:
            IsIRI( RTerm *rterm );
            ~IsIRI();

            IsIRI* clone() const;

            void accept( ExpressionVisitor *visitor ) const;
        };

        class SOPRANO_EXPORT IsBlank: public UnaryRTermBooleanExpression {
        public:
            IsBlank( RTerm *rterm );
            ~IsBlank();

            IsBlank* clone() const;

            void accept( ExpressionVisitor *visitor ) const;
        };

        class SOPRANO_EXPORT IsLiteral: public UnaryRTermBooleanExpression {
        public:
            IsLiteral( RTerm *rterm );
            ~IsLiteral();

            IsLiteral* clone() const;

            void accept( ExpressionVisitor *visitor ) const;
        };

        /*
         * SPARQL Accessors
         */
        
        class SOPRANO_EXPORT StringValue: public UnaryRTermStringExpression {
        public:
            StringValue( RTerm *rterm );

            StringValue *clone() const;

            void accept( ExpressionVisitor *visitor ) const;
        };

        class SOPRANO_EXPORT NumericalValue: public UnaryRTermNumericalExpression {
        public:
            NumericalValue( RTerm *rterm );

            NumericalValue *clone() const;

            void accept( ExpressionVisitor *visitor ) const;
        };

        class SOPRANO_EXPORT LangValue: public UnaryRTermStringExpression {
        public:
            // RTerm must be a literal
            LangValue( RTerm *rterm );

            LangValue* clone() const;

            void accept( ExpressionVisitor *visitor ) const;
        };

        class SOPRANO_EXPORT DataTypeValue: public UnaryRTermStringExpression {
        public:
            // RTerm must be a literal
            DataTypeValue( RTerm *rterm );

            DataTypeValue* clone() const;

            void accept( ExpressionVisitor *visitor ) const;
        };

        ////////////////////////////////////////////////////////////////////////
        // Binary Expression                                                  //
        ////////////////////////////////////////////////////////////////////////

        class SOPRANO_EXPORT BooleanSetExpression: public BooleanExpression {
        public:
            BooleanSetExpression();

            void addConditon( BooleanExpression *first );
            int count() const;
            const BooleanExpression* condition( int i ) const;
            const BooleanExpression* operator[]( int i ) const;
    
        private:
            class Private : public QSharedData
            {
            public:
                Private();
                Private( const Private& other );
                ~Private();
                QList<BooleanExpression*> conditions;
            };
            QSharedDataPointer<Private> d;
        };

        class SOPRANO_EXPORT BinaryNumericalBooleanExpression: public BooleanExpression {
        public:
            BinaryNumericalBooleanExpression( NumericalExpression *first, NumericalExpression *second );

            void setFirst( NumericalExpression *first );
            void setSecond( NumericalExpression *second );

            const NumericalExpression *first() const;
            const NumericalExpression *second() const;
   
        private:
            class Private : public QSharedData
            {
            public:
                Private( NumericalExpression *first_ = 0, NumericalExpression *second_ = 0 );
                Private( const Private& other );
                ~Private();
                NumericalExpression* first;
                NumericalExpression* second;
            };
            QSharedDataPointer<Private> d;
        };

        class SOPRANO_EXPORT BinaryStringBooleanExpression: public BooleanExpression {
        public:
            BinaryStringBooleanExpression( StringExpression *first, StringExpression *second );

            void setFirst( StringExpression *first );
            void setSecond( StringExpression *second );

            const StringExpression *first() const;
            const StringExpression *second() const;
  
        private:
            class Private : public QSharedData
            {
            public:
                Private( StringExpression *first_ = 0, StringExpression *second_ = 0 );
                Private( const Private& other );
                ~Private();
                StringExpression* first;
                StringExpression* second;
            };
            QSharedDataPointer<Private> d;
        };

        class SOPRANO_EXPORT BinaryDateTimeBooleanExpression: public BooleanExpression {
        public:
            BinaryDateTimeBooleanExpression( const QDateTime& first, const QDateTime& second );

            void setFirst( const QDateTime& first );
            void setSecond( const QDateTime& second );

            QDateTime first() const;
            QDateTime second() const;
  
        private:
            class Private : public QSharedData
            {
            public:
                Private( const QDateTime first_ = QDateTime(), const QDateTime& second_ = QDateTime() );
                QDateTime first;
                QDateTime second;
            };
            QSharedDataPointer<Private> d;
        };

        class SOPRANO_EXPORT BinaryRTermBooleanExpression: public BooleanExpression {
        public:
            BinaryRTermBooleanExpression( RTerm *first, RTerm *second );

            void setFirst( RTerm *first );
            void setSecond( RTerm *second );

            const RTerm *first() const;
            const RTerm *second() const;

        private:
            class Private : public QSharedData
            {
            public:
                Private( RTerm *first_ = 0, RTerm *second_ = 0 );
                Private( const Private& other );
                ~Private();
                RTerm* first;
                RTerm* second;
            };
            QSharedDataPointer<Private> d;
        };

        class SOPRANO_EXPORT BinaryNumericalExpression: public NumericalExpression {
        public:
            BinaryNumericalExpression( NumericalExpression *first, NumericalExpression *second );

            void setFirst( NumericalExpression *first );
            void setSecond( NumericalExpression *second );

            const NumericalExpression *first() const;
            const NumericalExpression *second() const;
    
        private:
            class Private : public QSharedData
            {
            public:
                Private( NumericalExpression *first_ = 0, NumericalExpression *second_ = 0 );
                Private( const Private& other );
                ~Private();
                NumericalExpression* first;
                NumericalExpression* second;
            };
            QSharedDataPointer<Private> d;
        };

        class SOPRANO_EXPORT LogicOr: public BooleanSetExpression {
        public:
            LogicOr();

            LogicOr* clone() const;
        
            void accept( ExpressionVisitor *visitor ) const;
        };

        class SOPRANO_EXPORT LogicAnd: public BooleanSetExpression {
        public:
            LogicAnd();
            
            LogicAnd* clone() const;

            void accept( ExpressionVisitor *visitor ) const;
        };

        class SOPRANO_EXPORT NumericalEqual: public BinaryNumericalBooleanExpression {
        public:
            NumericalEqual( NumericalExpression *first, NumericalExpression *second );

            NumericalEqual* clone() const;

            void accept( ExpressionVisitor *visitor ) const;
        };

        // trueg: I think the NotEqual expressions are superfluous since they can be done via Not(Equal(expr, expr))
        class SOPRANO_EXPORT NumericalNotEqual: public BinaryNumericalBooleanExpression {
        public:
            NumericalNotEqual( NumericalExpression *first, NumericalExpression *second );

            NumericalNotEqual* clone() const;

            void accept( ExpressionVisitor *visitor ) const;
        };

        class SOPRANO_EXPORT StringEqual: public BinaryStringBooleanExpression {
        public:
            StringEqual( StringExpression *first, StringExpression *second );

            StringEqual* clone() const;

            void accept( ExpressionVisitor *visitor ) const;
        };

        class SOPRANO_EXPORT StringNotEqual: public BinaryStringBooleanExpression {
        public:
            StringNotEqual( StringExpression *first, StringExpression *second );

            StringNotEqual* clone() const;

            void accept( ExpressionVisitor *visitor ) const;
        };

        class SOPRANO_EXPORT DateTimeEqual: public BinaryDateTimeBooleanExpression {
        public:
            DateTimeEqual( const QDateTime& first, const QDateTime& second );

            DateTimeEqual* clone() const;

            void accept( ExpressionVisitor *visitor ) const;
        }; 

        class SOPRANO_EXPORT DateTimeNotEqual: public BinaryDateTimeBooleanExpression {
        public:
            DateTimeNotEqual( const QDateTime& first, const QDateTime& second );

            DateTimeNotEqual* clone() const;

            void accept( ExpressionVisitor *visitor ) const;
        };

        class SOPRANO_EXPORT NumericalLessThan: public BinaryNumericalBooleanExpression {
        public:
            NumericalLessThan( NumericalExpression *first, NumericalExpression *second );

            NumericalLessThan* clone() const;

            void accept( ExpressionVisitor *visitor ) const;
        };

        class SOPRANO_EXPORT NumericalGreaterThan: public BinaryNumericalBooleanExpression {
        public:
            NumericalGreaterThan( NumericalExpression *first, NumericalExpression *second );

            NumericalGreaterThan* clone() const;

            void accept( ExpressionVisitor *visitor ) const;
        };

        class SOPRANO_EXPORT NumericalLessThanEqual: public BinaryNumericalBooleanExpression {
        public:
            NumericalLessThanEqual( NumericalExpression *first, NumericalExpression *second );

            NumericalLessThanEqual* clone() const;

            void accept( ExpressionVisitor *visitor ) const;
        };

        class SOPRANO_EXPORT NumericalGreaterThanEqual: public BinaryNumericalBooleanExpression {
        public:
            NumericalGreaterThanEqual( NumericalExpression *first, NumericalExpression *second );

            NumericalGreaterThanEqual* clone() const;
        
            void accept( ExpressionVisitor *visitor ) const;
        };

        class SOPRANO_EXPORT StringLessThan: public BinaryStringBooleanExpression {
        public:
            StringLessThan( StringExpression *first, StringExpression *second );

            StringLessThan* clone() const;

            void accept( ExpressionVisitor *visitor ) const;
        };

        class SOPRANO_EXPORT StringGreaterThan: public BinaryStringBooleanExpression {
        public:
            StringGreaterThan( StringExpression *first, StringExpression *second );

            StringGreaterThan* clone() const;
        
            void accept( ExpressionVisitor *visitor ) const;
        };

        class SOPRANO_EXPORT StringLessThanEqual: public BinaryStringBooleanExpression {
        public:
            StringLessThanEqual( StringExpression *first, StringExpression *second );

            StringLessThanEqual* clone() const;

            void accept( ExpressionVisitor *visitor ) const;
        };

        class SOPRANO_EXPORT StringGreaterThanEqual: public BinaryStringBooleanExpression {
        public:
            StringGreaterThanEqual( StringExpression *first, StringExpression *second );

            StringGreaterThanEqual* clone() const;

            void accept( ExpressionVisitor *visitor ) const;
        };

        class SOPRANO_EXPORT DateTimeLessThan: public BinaryDateTimeBooleanExpression {
        public:
            DateTimeLessThan( const QDateTime& first, const QDateTime& second );

            DateTimeLessThan* clone() const;
        
            void accept( ExpressionVisitor *visitor ) const;
        };

        class SOPRANO_EXPORT DateTimeGreaterThan: public BinaryDateTimeBooleanExpression {
        public:
            DateTimeGreaterThan( const QDateTime& first, const QDateTime& second );

            DateTimeGreaterThan* clone() const;

            void accept( ExpressionVisitor *visitor ) const;
        };

        class SOPRANO_EXPORT DateTimeLessThanEqual: public BinaryDateTimeBooleanExpression {
        public:
            DateTimeLessThanEqual( const QDateTime& first, const QDateTime& second );

            DateTimeLessThanEqual* clone() const;

            void accept( ExpressionVisitor *visitor ) const;
        };

        class SOPRANO_EXPORT DateTimeGreaterThanEqual: public BinaryDateTimeBooleanExpression {
        public:
            DateTimeGreaterThanEqual( const QDateTime& first, const QDateTime& second );

            DateTimeGreaterThanEqual* clone() const;

            void accept( ExpressionVisitor *visitor ) const;
        };

        class SOPRANO_EXPORT NumericalMultiply: public BinaryNumericalExpression {
        public:
            NumericalMultiply( NumericalExpression *first, NumericalExpression *second );

            NumericalMultiply* clone() const;

            void accept( ExpressionVisitor *visitor ) const; 
        };

        class SOPRANO_EXPORT NumericalDivide: public BinaryNumericalExpression {
        public:
            NumericalDivide( NumericalExpression *first, NumericalExpression *second );

            NumericalDivide* clone() const;

            void accept( ExpressionVisitor *visitor ) const;
        };

        class SOPRANO_EXPORT NumericalAdd: public BinaryNumericalExpression {
        public:
            NumericalAdd( NumericalExpression *first, NumericalExpression *second );

            NumericalAdd* clone() const;

            void accept( ExpressionVisitor *visitor ) const;
        };

        class SOPRANO_EXPORT NumericalSubtract: public BinaryNumericalExpression {
        public:
            NumericalSubtract( NumericalExpression *first, NumericalExpression *second );

            NumericalSubtract* clone() const;

            void accept( ExpressionVisitor *visitor ) const;
        };

        /*          
         * SPARQL Tests as defined in:         
         * - http://www.w3.org/TR/2005/WD-rdf-sparql-query-20051123/#SparqlOps       
         */

        class SOPRANO_EXPORT RTermEqual: public BinaryRTermBooleanExpression {
        public:
            RTermEqual( RTerm *first, RTerm *second );

            RTermEqual* clone() const;

            void accept( ExpressionVisitor *visitor ) const;
        };

        class SOPRANO_EXPORT RTermNotEqual: public BinaryRTermBooleanExpression {
        public:
            RTermNotEqual( RTerm *first, RTerm *second );

            RTermNotEqual* clone() const;

            void accept( ExpressionVisitor *visitor ) const;
        };

        class SOPRANO_EXPORT LangMatches: public BinaryStringBooleanExpression {
        public:
            LangMatches( StringExpression *first, StringExpression *second );

            LangMatches* clone() const;

            void accept( ExpressionVisitor *visitor ) const;
        }; 

        class SOPRANO_EXPORT Regexp: public BooleanExpression {
        public:
            Regexp();
            Regexp( StringExpression *expression, const QString &pattern);
            Regexp( StringExpression *expression, const QString &pattern, const QString &flags );
            Regexp( const Regexp& other );
            ~Regexp();

            Regexp& operator=( const Regexp& other );

            void setExpression( StringExpression *expression );
            const StringExpression *expression() const;

            void setPattern( const QString &pattern );
            QString pattern();

            void setFlags( const QString &flags );
            QString flags();

            Regexp* clone() const;
            
            void accept( ExpressionVisitor *visitor ) const;

        private:
            class Private : public QSharedData
            {
            public:
                Private();
                Private( StringExpression *ex, const QString &p, const QString& f = QString() );
                Private( const Private& other);
                ~Private();
                StringExpression *expression;
                QString pattern;
                QString flags;
            };
            QSharedDataPointer<Private> d;
        }; 

        ////////////////////////////////////////////////////////////////////////
        // ParsedQuery                                                        //
        ////////////////////////////////////////////////////////////////////////

        class SOPRANO_EXPORT Prefix {
        public:
            Prefix();
            Prefix( const QString &prefix, const QUrl &uri );
            Prefix( const Prefix &other );
            ~Prefix();

            Prefix& operator=( const Prefix& other );
        
            const QString prefix() const;
            const QUrl uri() const;

        private:
            class Private : public QSharedData {
            public:
                Private();
                QString prefix;
                QUrl uri;
            };
            QSharedDataPointer<Private> d;
        };

        class SOPRANO_EXPORT TriplePattern : public BooleanExpression {
        public:
            TriplePattern();
            
            /*
             * TriplePattern takes ownership of the RTerm instances.
             */
            TriplePattern( RTerm* subject, RTerm* predicate, RTerm* object );
            TriplePattern( const TriplePattern& other );
            ~TriplePattern();

            TriplePattern& operator=( const TriplePattern& other ); 

            void setSubject( RTerm *subject );
            const RTerm *subject() const;

            void setPredicate( RTerm *predicate );
            const RTerm *predicate() const;

            void setObject( RTerm *object );
            const RTerm *object() const;

            void setContext( RTerm *context );
            const RTerm *context() const;

            TriplePattern* clone() const;

            void accept( ExpressionVisitor* visitor ) const;

        private:
            class Private: public QSharedData
            {
            public:
                Private();
                Private( const Private& other );
                ~Private();
                RTerm *subject;
                RTerm *predicate;
                RTerm *object;
            };
            QSharedDataPointer<Private> d;
        };
        
        class SOPRANO_EXPORT QueryTerms {
        public:
            QueryTerms();
            QueryTerms( const QueryTerms& other );
            ~QueryTerms();
            
            QueryTerms& operator=( const QueryTerms& other);
    
            /*
             * QueryTerms takes ownership of the Variable instances.
             */
            void addVariable( Variable *variable );
        
            QList<const Variable*> variables() const;
            
            bool selectAll() const;
            
        private:
            class Private : public QSharedData
            {
            public:
                Private();
                Private( const Private& other);
                ~Private();
                QList<Variable *> variables;
            };
            QSharedDataPointer<Private> d;
        };
        
        /**
         * \class Query query.h Soprano/Query/Query
         *
         * \warning <b>The API is subject to change and far from being stable.</b>
         */
        class SOPRANO_EXPORT Query {
        public:
            // Query types
            enum QueryType {
                INVALID_QUERY,
                GRAPH_QUERY,
                SELECT_QUERY,
                BOOLEAN_QUERY
          
                // TODO: Implement Insert and Delete query
                // INSERT_QUERY = 0x8,
                // DELETE_QUERY = 0x10
            };
        
            Query();
            Query( QueryType type );
            Query( const Query &other );
            ~Query();
        
            Query& operator=( const Query& other );

            bool isValid() const;
        
            void addPrefix( const Prefix &prefix );
            QList<Prefix> prefixes();

            void setCondition( BooleanExpression* condition );
            const BooleanExpression *condition() const;
        
            void setQueryType( QueryType type );
            QueryType type() const;

            void setQueryTerms( const QueryTerms &queryTerms );
            const QueryTerms queryTerms() const;

        private:
            class Private : public QSharedData {
            public:
                Private();
                Private( const Private& other );
                ~Private();
                BooleanExpression *condition;
                QList<Prefix> prefixes;
                QueryTerms queryTerms;
                QueryType type;
            };
            QSharedDataPointer<Private> d;
        };

        ////////////////////////////////////////////////////////////////////////
        // ExpressionVisitor                                                  //
        ////////////////////////////////////////////////////////////////////////
    
        // FIXME: why is there no visit( const Query* ) method?
        class SOPRANO_EXPORT ExpressionVisitor {
        public:
            virtual ~ExpressionVisitor() {}

            virtual void visit( const Not *expression ) = 0;

            virtual void visit( const Negate *expression ) = 0;

            virtual void visit( const IsBound *expression ) = 0;

            virtual void visit( const IsIRI *expression ) = 0;

            virtual void visit( const IsBlank *expression ) = 0;

            virtual void visit( const IsLiteral *expression ) = 0;

            virtual void visit( const StringValue *expression ) = 0;

            virtual void visit( const NumericalValue *expression ) = 0;

            virtual void visit( const LangValue *expression ) = 0;

            virtual void visit( const DataTypeValue *expression ) = 0;

            virtual void visit( const LogicAnd *expression ) = 0;

            virtual void visit( const LogicOr *expression ) = 0;

            virtual void visit( const NumericalEqual *expression ) = 0;

            virtual void visit( const NumericalNotEqual *expression ) = 0;
   
            virtual void visit( const StringEqual *expression ) = 0;
   
            virtual void visit( const StringNotEqual *expression ) = 0;

            virtual void visit( const DateTimeEqual *expression ) = 0;

            virtual void visit( const DateTimeNotEqual *expression ) = 0;

            virtual void visit( const Numerical *expression ) = 0;

            virtual void visit( const NumericalLessThan *expression ) = 0;

            virtual void visit( const NumericalGreaterThan *expression ) = 0;

            virtual void visit( const NumericalLessThanEqual *expression ) = 0;

            virtual void visit( const NumericalGreaterThanEqual *expression ) = 0;

            virtual void visit( const String *expression ) = 0;

            virtual void visit( const StringLessThan *expression ) = 0;

            virtual void visit( const StringGreaterThan *expression ) = 0;

            virtual void visit( const StringLessThanEqual *expression ) = 0;

            virtual void visit( const StringGreaterThanEqual *expression ) = 0;

            virtual void visit( const DateTimeLessThan *expression ) = 0;

            virtual void visit( const DateTimeGreaterThan *expression ) = 0;

            virtual void visit( const DateTimeLessThanEqual *expression ) = 0;

            virtual void visit( const DateTimeGreaterThanEqual *expression ) = 0;

            virtual void visit( const NumericalMultiply *expression ) = 0;

            virtual void visit( const NumericalDivide *expression ) = 0;

            virtual void visit( const NumericalAdd *expression ) = 0;

            virtual void visit( const NumericalSubtract *expression ) = 0;

            virtual void visit( const RTermEqual *expression ) = 0;

            virtual void visit( const RTermNotEqual *expression ) = 0;

            virtual void visit( const LangMatches *expression ) = 0;

            virtual void visit( const Regexp *expression ) = 0;

            virtual void visit( const TriplePattern *expression ) = 0;
        };
    }
}

#endif // SOPRANO_QUERY_API_H
