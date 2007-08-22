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

#include <QUrl>
#include <QList>
#include <QString>
#include <QDateTime>
#include <QtCore/QSharedData>

// FIXME: - set operations such as intersect
//        - I think it would be sufficient to have equal on expressions rather
//          then one for strings and one or numericals and so on
//        - String operations such as concat
//        - We have no way of representing OFFSET or LIMIT yet


namespace Soprano
{
    class Node;

    namespace Query {

        class ExpressionVisitor;

        class Expression {
        public:
            virtual ~Expression();
        
            virtual void accept( ExpressionVisitor *visitor ) = 0;

            virtual Expression* clone() const = 0;
            
        protected:
            Expression();
        };
    
        class RTerm {
        public:
            virtual ~RTerm();

            virtual RTerm* clone() const = 0;

            virtual bool isVariable() const;
            virtual bool isNode() const;

        protected:
            RTerm();
        };


        class Variable : public RTerm {
        public:
            Variable();
            Variable( const QString &name );
            Variable( const Variable &other );

            Variable& operator=( const Variable& );

            Variable* clone() const;

            bool isVariable() const { return true; }

            QString name() const;

        private:
            class Private;
            QSharedDataPointer<Private> d;
        };


        class Node : public RTerm {
        public:
            Node();
            Node( const Soprano::Node &node );
            Node( const Node &other );

            Node* clone() const;
     
            Node& operator=( const Node& );
     
            bool isNode() const { return true; }

            Soprano::Node node() const;
        
        private:
            class Private;
            QSharedDataPointer<Private> d;
        };
        
    
        ////////////////////////////////////////////////////////////////////////
        // Expressions                                                        //
        ////////////////////////////////////////////////////////////////////////


        // An expression that return a xsd:boolean
        class BooleanExpression: public Expression {
        public:
            virtual ~BooleanExpression();

            virtual BooleanExpression* clone() const = 0;

        protected:
            BooleanExpression();
        };

        // An expression that return a xsd:integer, xsd:decimal, xsd:float, and xsd:double
        class NumericalExpression: public Expression {
        public:
            virtual ~NumericalExpression();

            NumericalExpression* clone() const = 0;

        protected:
            NumericalExpression();
        };

        /**
         * A numerical constant.
        */
        class Numerical : public NumericalExpression {
        public:
            Numerical();
            Numerical( int    value );
            Numerical( double value );
            Numerical( float  value );
            Numerical( const Numerical &other );
            ~Numerical();
        
            Numerical& operator=( const Numerical& );

            Numerical* clone() const;

            void accept( ExpressionVisitor *visitor );
        
            bool isDecimal();

            bool isDouble();
            double doubleValue();

            bool isFloat();
            float floatValue();
            
            bool isInteger();
            int integerValue();
            
        private:
            class Private;
            QSharedDataPointer<Private> d;
        };
   
        // An expression that return a xsd:string or rdfs:Datatype
        class StringExpression: public Expression {
        public:
            virtual ~StringExpression();

            StringExpression* clone() const = 0;

        protected:
            StringExpression();
        };

        /**
        * A string constant
        */
        class String : public StringExpression {
        public:
            String();
            String( const QString &value );
            String( const String &other );

            String* clone() const;

            QString value() const;
            void setValue( const QString &value );
    
            void accept( ExpressionVisitor *visitor );

        private:
            class Private;
            QSharedDataPointer<Private> d;
        };

        ////////////////////////////////////////////////////////////////////////
        // Unary Expressions                                                  //
        ////////////////////////////////////////////////////////////////////////

        class UnaryBooleanExpression: public BooleanExpression {
        public:
            UnaryBooleanExpression( BooleanExpression *expression );

            void setExpression( BooleanExpression *expression );
            const BooleanExpression *expression() const;

        private:
            class Private;
            QSharedDataPointer<Private> d;
        };

        class UnaryRTermBooleanExpression: public BooleanExpression {
        public:
            UnaryRTermBooleanExpression( RTerm *rterm );
        
            void setRTerm( RTerm *expression );
            const RTerm *rterm() const;

        private:
            class Private;
            QSharedDataPointer<Private> d;
        };

        class UnaryRTermStringExpression: public StringExpression {
        public:
            UnaryRTermStringExpression( RTerm *rterm );
        
            void setRTerm( RTerm *expression );
            const RTerm *rterm() const;

        private:
            class Private;
            QSharedDataPointer<Private> d;
        };

        class UnaryNumericalExpression: public NumericalExpression {
        public:
            UnaryNumericalExpression( NumericalExpression *expression );

            void setExpression( NumericalExpression *expression );
            const NumericalExpression *expression() const;

        private:
            class Private;
            QSharedDataPointer<Private> d;
        };

        /* XQUERY Tests */

        class Not: public UnaryBooleanExpression {
        public:
            Not();
            Not( BooleanExpression *expression );
            ~Not();
        
            Not* clone() const;
        
            void accept( ExpressionVisitor *visitor );
        };

        class Negate: public UnaryNumericalExpression  {
        public:
            Negate();
            Negate( NumericalExpression *expression );
            ~Negate();

            Negate* clone() const;
        
            void accept( ExpressionVisitor *visitor );
        };

        /* 
         * SPARQL Tests as defined in
         * - http://www.w3.org/TR/2005/WD-rdf-sparql-query-20051123/#SparqlOps
         */

        class IsBound: public BooleanExpression {
        public:
            IsBound();
            IsBound( Variable *variable );
            IsBound( const IsBound &other );
            virtual ~IsBound();
            
            IsBound operator=( const IsBound &other );

            void setVariable( Variable *variable );
            const Variable *variable() const;

            IsBound* clone() const;
            
            void accept( ExpressionVisitor *visitor );
            
        private:
            class Private;
            QSharedDataPointer<Private> d;
        };

        class IsIRI: public UnaryRTermBooleanExpression {
        public:
            IsIRI( RTerm *rterm );
            ~IsIRI();

            IsIRI* clone() const;

            void accept( ExpressionVisitor *visitor );
        };

        class IsBlank: public UnaryRTermBooleanExpression {
        public:
            IsBlank( RTerm *rterm );
            ~IsBlank();

            IsBlank* clone() const;

            void accept( ExpressionVisitor *visitor );
        };

        class IsLiteral: public UnaryRTermBooleanExpression {
        public:
            IsLiteral( RTerm *rterm );
            ~IsLiteral();

            IsLiteral* clone() const;

            void accept( ExpressionVisitor *visitor );
        };

        /*
         * SPARQL Accessors
         */
        
        class StringValue: public UnaryRTermStringExpression {
        public:
            StringValue( RTerm *rterm );

            StringValue *clone() const;

            void accept( ExpressionVisitor *visitor );
        };

        class LangValue: public UnaryRTermStringExpression {
        public:
            // RTerm must be a literal
            LangValue( RTerm *rterm );

            LangValue* clone() const;

            void accept( ExpressionVisitor *visitor );
        };

        class DataTypeValue: public UnaryRTermStringExpression {
        public:
            // RTerm must be a literal
            DataTypeValue( RTerm *rterm );

            DataTypeValue* clone() const;

            void accept( ExpressionVisitor *visitor );
        };

        ////////////////////////////////////////////////////////////////////////
        // Binary Expression                                                  //
        ////////////////////////////////////////////////////////////////////////

        class BinaryBooleanExpression: public BooleanExpression {
        public:
            BinaryBooleanExpression( BooleanExpression *first, BooleanExpression *second );

            void setFirst( BooleanExpression *first );
            void setSecond( BooleanExpression *second );    

            const BooleanExpression *first() const;
            const BooleanExpression *second() const;
    
        private:
            class Private;
            QSharedDataPointer<Private> d;
        };

        class BinaryNumericalBooleanExpression: public BooleanExpression {
        public:
            BinaryNumericalBooleanExpression( NumericalExpression *first, NumericalExpression *second );

            void setFirst( NumericalExpression *first );
            void setSecond( NumericalExpression *second );

            const NumericalExpression *first() const;
            const NumericalExpression *second() const;
   
        private:
            class Private;
            QSharedDataPointer<Private> d;
        };

        class BinaryStringBooleanExpression: public BooleanExpression {
        public:
            BinaryStringBooleanExpression( StringExpression *first, StringExpression *second );

            void setFirst( StringExpression *first );
            void setSecond( StringExpression *second );

            const StringExpression *first() const;
            const StringExpression *second() const;
  
        private:
            class Private;
            QSharedDataPointer<Private> d;
        };

        class BinaryDateTimeBooleanExpression: public BooleanExpression {
        public:
            BinaryDateTimeBooleanExpression( const QDateTime& first, const QDateTime& second );

            void setFirst( const QDateTime& first );
            void setSecond( const QDateTime& second );

            QDateTime first() const;
            QDateTime second() const;
  
        private:
            class Private;
            QSharedDataPointer<Private> d;
        };

        class BinaryRTermBooleanExpression: public BooleanExpression {
        public:
            BinaryRTermBooleanExpression( RTerm *first, RTerm *second );

            void setFirst( RTerm *first );
            void setSecond( RTerm *second );

            const RTerm *first() const;
            const RTerm *second() const;

        private:
            class Private;
            QSharedDataPointer<Private> d;
        };

        class BinaryNumericalExpression: public NumericalExpression {
        public:
            BinaryNumericalExpression( NumericalExpression *first, NumericalExpression *second );

            void setFirst( NumericalExpression *first );
            void setSecond( NumericalExpression *second );

            const NumericalExpression *first() const;
            const NumericalExpression *second() const;
    
        private:
            class Private;
            QSharedDataPointer<Private> d;
        };

        class LogicOr: public BinaryBooleanExpression {
        public:
            LogicOr( BooleanExpression *first, BooleanExpression *second );

            LogicOr* clone() const;
        
            void accept( ExpressionVisitor *visitor );
        };

        class LogicAnd: public BinaryBooleanExpression {
        public:
            LogicAnd( BooleanExpression *first, BooleanExpression *second );
            
            LogicAnd* clone() const;

            void accept( ExpressionVisitor *visitor );
        };

        class NumericalEqual: public BinaryNumericalBooleanExpression {
        public:
            NumericalEqual( NumericalExpression *first, NumericalExpression *second );

            NumericalEqual* clone() const;

            void accept( ExpressionVisitor *visitor );
        };

        // trueg: I think the NotEqual expressions are superfluous since they can be done via Not(Equal(expr, expr))
        class NumericalNotEqual: public BinaryNumericalBooleanExpression {
        public:
            NumericalNotEqual( NumericalExpression *first, NumericalExpression *second );

            NumericalNotEqual* clone() const;

            void accept( ExpressionVisitor *visitor );
        };

        class StringEqual: public BinaryStringBooleanExpression {
        public:
            StringEqual( StringExpression *first, StringExpression *second );

            StringEqual* clone() const;

            void accept( ExpressionVisitor *visitor );
        };

        class StringNotEqual: public BinaryStringBooleanExpression {
        public:
            StringNotEqual( StringExpression *first, StringExpression *second );

            StringNotEqual* clone() const;

            void accept( ExpressionVisitor *visitor );
        };

        class DateTimeEqual: public BinaryDateTimeBooleanExpression {
        public:
            DateTimeEqual( const QDateTime& first, const QDateTime& second );

            DateTimeEqual* clone() const;

            void accept( ExpressionVisitor *visitor );
        }; 

        class DateTimeNotEqual: public BinaryDateTimeBooleanExpression {
        public:
            DateTimeNotEqual( const QDateTime& first, const QDateTime& second );

            DateTimeNotEqual* clone() const;

            void accept( ExpressionVisitor *visitor );
        };

        class NumericalLessThan: public BinaryNumericalBooleanExpression {
        public:
            NumericalLessThan( NumericalExpression *first, NumericalExpression *second );

            NumericalLessThan* clone() const;

            void accept( ExpressionVisitor *visitor );
        };

        class NumericalGreaterThan: public BinaryNumericalBooleanExpression {
        public:
            NumericalGreaterThan( NumericalExpression *first, NumericalExpression *second );

            NumericalGreaterThan* clone() const;

            void accept( ExpressionVisitor *visitor );
        };

        class NumericalLessThanEqual: public BinaryNumericalBooleanExpression {
        public:
            NumericalLessThanEqual( NumericalExpression *first, NumericalExpression *second );

            NumericalLessThanEqual* clone() const;

            void accept( ExpressionVisitor *visitor );
        };

        class NumericalGreaterThanEqual: public BinaryNumericalBooleanExpression {
        public:
            NumericalGreaterThanEqual( NumericalExpression *first, NumericalExpression *second );

            NumericalGreaterThanEqual* clone() const;
        
            void accept( ExpressionVisitor *visitor );
        };

        class StringLessThan: public BinaryStringBooleanExpression {
        public:
            StringLessThan( StringExpression *first, StringExpression *second );

            StringLessThan* clone() const;

            void accept( ExpressionVisitor *visitor );
        };

        class StringGreaterThan: public BinaryStringBooleanExpression {
        public:
            StringGreaterThan( StringExpression *first, StringExpression *second );

            StringGreaterThan* clone() const;
        
            void accept( ExpressionVisitor *visitor );
        };

        class StringLessThanEqual: public BinaryStringBooleanExpression {
        public:
            StringLessThanEqual( StringExpression *first, StringExpression *second );

            StringLessThanEqual* clone() const;

            void accept( ExpressionVisitor *visitor );
        };

        class StringGreaterThanEqual: public BinaryStringBooleanExpression {
        public:
            StringGreaterThanEqual( StringExpression *first, StringExpression *second );

            StringGreaterThanEqual* clone() const;

            void accept( ExpressionVisitor *visitor );
        };

        class DateTimeLessThan: public BinaryDateTimeBooleanExpression {
        public:
            DateTimeLessThan( const QDateTime& first, const QDateTime& second );

            DateTimeLessThan* clone() const;
        
            void accept( ExpressionVisitor *visitor );
        };

        class DateTimeGreaterThan: public BinaryDateTimeBooleanExpression {
        public:
            DateTimeGreaterThan( const QDateTime& first, const QDateTime& second );

            DateTimeGreaterThan* clone() const;

            void accept( ExpressionVisitor *visitor );
        };

        class DateTimeLessThanEqual: public BinaryDateTimeBooleanExpression {
        public:
            DateTimeLessThanEqual( const QDateTime& first, const QDateTime& second );

            DateTimeLessThanEqual* clone() const;

            void accept( ExpressionVisitor *visitor );
        };

        class DateTimeGreaterThanEqual: public BinaryDateTimeBooleanExpression {
        public:
            DateTimeGreaterThanEqual( const QDateTime& first, const QDateTime& second );

            DateTimeGreaterThanEqual* clone() const;

            void accept( ExpressionVisitor *visitor );
        };

        class NumericalMultiply: public BinaryNumericalExpression {
        public:
            NumericalMultiply( NumericalExpression *first, NumericalExpression *second );

            NumericalMultiply* clone() const;

            void accept( ExpressionVisitor *visitor ); 
        };

        class NumericalDivide: public BinaryNumericalExpression {
        public:
            NumericalDivide( NumericalExpression *first, NumericalExpression *second );

            NumericalDivide* clone() const;

            void accept( ExpressionVisitor *visitor );
        };

        class NumericalAdd: public BinaryNumericalExpression {
        public:
            NumericalAdd( NumericalExpression *first, NumericalExpression *second );

            NumericalAdd* clone() const;

            void accept( ExpressionVisitor *visitor );
        };

        class NumericalSubtract: public BinaryNumericalExpression {
        public:
            NumericalSubtract( NumericalExpression *first, NumericalExpression *second );

            NumericalSubtract* clone() const;

            void accept( ExpressionVisitor *visitor );
        };

        /*          
         * SPARQL Tests as defined in:         
         * - http://www.w3.org/TR/2005/WD-rdf-sparql-query-20051123/#SparqlOps       
         */

        class RTermEqual: public BinaryRTermBooleanExpression {
        public:
            RTermEqual( RTerm *first, RTerm *second );

            RTermEqual* clone() const;

            void accept( ExpressionVisitor *expression );
        };

        class RTermNotEqual: public BinaryRTermBooleanExpression {
        public:
            RTermNotEqual( RTerm *first, RTerm *second );

            RTermNotEqual* clone() const;

            void accept( ExpressionVisitor *expression );
        };

        class LangMatches: public BinaryStringBooleanExpression {
        public:
            LangMatches( StringExpression *first, StringExpression *second );

            LangMatches* clone() const;

            void accept( ExpressionVisitor *expression );
        }; 

        class Regexp: public BooleanExpression {
        public:
            Regexp( StringExpression *expression, const QString &pattern);
            Regexp( StringExpression *expression, const QString &pattern, const QString &flags );

            Regexp* clone() const;

            void setExpression( StringExpression *expression );
            const StringExpression *expression() const;

            void setPattern( const QString &pattern );
            QString pattern();

            void setFlags( const QString &flags );
            QString flags();

            void accept( ExpressionVisitor *expression );

        private:
            class Private;
            QSharedDataPointer<Private> d;
        }; 

        ////////////////////////////////////////////////////////////////////////
        // ExpressionVisitor                                                  //
        ////////////////////////////////////////////////////////////////////////

        class ExpressionVisitor {
        public:
	    virtual ~ExpressionVisitor() {}

            virtual void visit( Not *expression ) = 0;

            virtual void visit( Negate *expression ) = 0;
            
            virtual void visit( IsBound *expression ) = 0;
            
            virtual void visit( IsIRI *expression ) = 0;
            
            virtual void visit( IsBlank *expression ) = 0;
            
            virtual void visit( IsLiteral *expression ) = 0;
            
            virtual void visit( StringValue *expression ) = 0;
            
            virtual void visit( LangValue *expression ) = 0;
            
            virtual void visit( DataTypeValue *expression ) = 0;
            
            virtual void visit( LogicAnd *expression ) = 0;

            virtual void visit( LogicOr *expression ) = 0;
            
            virtual void visit( NumericalEqual *expression ) = 0;
            
            virtual void visit( NumericalNotEqual *expression ) = 0;
    
            virtual void visit( StringEqual *expression ) = 0;
    
            virtual void visit( StringNotEqual *expression ) = 0;
            
            virtual void visit( DateTimeEqual *expression ) = 0;
            
            virtual void visit( DateTimeNotEqual *expression ) = 0;

            virtual void visit( Numerical *expression ) = 0;

            virtual void visit( NumericalLessThan *expression ) = 0;

            virtual void visit( NumericalGreaterThan *expression ) = 0;
            
            virtual void visit( NumericalLessThanEqual *expression ) = 0;

            virtual void visit( NumericalGreaterThanEqual *expression ) = 0;

            virtual void visit( String *expression ) = 0;

            virtual void visit( StringLessThan *expression ) = 0;
        
            virtual void visit( StringGreaterThan *expression ) = 0;
            
            virtual void visit( StringLessThanEqual *expression ) = 0;
            
            virtual void visit( StringGreaterThanEqual *expression ) = 0;

            virtual void visit( DateTimeLessThan *expression ) = 0;

            virtual void visit( DateTimeGreaterThan *expression ) = 0;

            virtual void visit( DateTimeLessThanEqual *expression ) = 0;

            virtual void visit( DateTimeGreaterThanEqual *expression ) = 0;
            
            virtual void visit( NumericalMultiply *expression ) = 0;

            virtual void visit( NumericalDivide *expression ) = 0;
            
            virtual void visit( NumericalAdd *expression ) = 0;
            
            virtual void visit( NumericalSubtract *expression ) = 0;

            virtual void visit( RTermEqual *expression ) = 0;

            virtual void visit( RTermNotEqual *expression ) = 0;

            virtual void visit( LangMatches *expression ) = 0;

            virtual void visit( Regexp *expression ) = 0;
        };
    

        ////////////////////////////////////////////////////////////////////////
        // ParsedQuery                                                        //
        ////////////////////////////////////////////////////////////////////////

        class Prefix {
        public:
            Prefix();
            Prefix( const QString &prefix, const QUrl &uri );
            Prefix( const Prefix &other );

            Prefix& operator=( const Prefix& other );
        
            const QString prefix() const;
            const QUrl uri() const;

        private:
            class Private;
            QSharedDataPointer<Private> d;
        };

        class GraphPattern : public BooleanExpression {
        public:
            GraphPattern();

            /*
             * GraphPattern takes ownership of the RTerm instances.
             */
            GraphPattern( RTerm *subject, RTerm *predicate, RTerm *object, RTerm *context = 0, bool optional = false );
            GraphPattern( const GraphPattern &other );
            ~GraphPattern();
        
            GraphPattern& operator=( const GraphPattern &other );
        
            void setOptional(bool optional);
            bool optional() const;
        
            void setSubject( RTerm *subject );
            const RTerm *subject() const;
            
            void setPredicate( RTerm *predicate );
            const RTerm *predicate() const; 
                              
            void setObject( RTerm *object );
            const RTerm *object() const; 
            
            void setContext( RTerm *context );
            const RTerm *context() const;
                
        private:
            class Private;
            QSharedDataPointer<Private> d;
        };
        
        class QueryTerms {
        public:
            QueryTerms();
            QueryTerms( const QueryTerms& other );
            ~QueryTerms();
            
            QueryTerms& operator=( const QueryTerms& other);
    
            /*
             * QueryTerms takes ownership of the RTerm instances.
             */
            void addQueryTerm( RTerm *rterm );
        
            QList<const RTerm*> terms() const;
            
            bool selectAll() const;
            
        private:
            class Private;
            QSharedDataPointer<Private> d;
        };
        
        class Query {
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
            QueryTerms terms() const;
        
        private:
            class Private;
            QSharedDataPointer<Private> d;
        };
    };
};

#endif // SOPRANO_QUERY_API_H
