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

namespace Soprano
{
    class Node;

    namespace Query {

        class DateTime {
        };

        class Variable {
        public:
            Variable( const QString &name );

            const QString name() const;

        private:
            QString m_name;
        };

        class RTerm {
        public:
            RTerm( Soprano::Node *node );
            RTerm( Variable *binding );
 
            bool isBinding() const;

            Soprano::Node *node();
            Variable *binding();

        private:
            Soprano::Node *m_node;
            Variable *m_binding;
        };

        class Numerical {
        };
   
        class Decimal: public Numerical {
        };
 
        class Double : public Decimal {
        public:
            Double( double value );

            double value() const;

        private:
            double m_value;
        };

        class Float: public Decimal {
        public:
            Float( float value );

            float value() const;

        private:
            float m_value;
        };

        class Integer : public Numerical {
        public:
            Integer( int value );

            int value() const;

        private:
            int m_value;
        };

        ////////////////////////////////////////////////////////////////////////
        // Expressions                                                        //
        ////////////////////////////////////////////////////////////////////////

        class ExpressionVisitor;

        class Expression {
        public:
            virtual void accept( ExpressionVisitor *visitor ) = 0;
        };

        // An expression that return a xsd:boolean
        class BooleanExpression: public Expression {
        };

        // An expression that return a xsd:integer, xsd:decimal, xsd:float, and xsd:double
        class NumericalExpression: public Expression {
        };

        // An expression that return a xsd:string or rdfs:Datatype
        class StringExpression: public Expression {
        };

        ////////////////////////////////////////////////////////////////////////
        // Unary Expressions                                                  //
        ////////////////////////////////////////////////////////////////////////

        class UnaryBooleanExpression: public BooleanExpression {
        public:
            UnaryBooleanExpression( BooleanExpression *expression );

            void setExpression( BooleanExpression *expression );
            BooleanExpression *expression();

        private:
            BooleanExpression *m_expression;
        };

        class UnaryRTermBooleanExpression: public BooleanExpression {
        public:
            UnaryRTermBooleanExpression( RTerm *rterm );

            void setRTerm( RTerm *expression );
            RTerm *rterm();

        private:
            RTerm *m_rterm;
        };

        class UnaryRTermStringExpression: public StringExpression {
        public:
            UnaryRTermStringExpression( RTerm *rterm );

            void setRTerm( RTerm *expression );
            RTerm *rterm();

        private:
            RTerm *m_rterm;
        };

        class UnaryNumericalExpression: public NumericalExpression {
        public:
            UnaryNumericalExpression( NumericalExpression *expression );

            void setExpression( NumericalExpression *expression );
            NumericalExpression *expression();

        private:
            NumericalExpression *m_expression;
        };

        /* XQUERY Tests */

        class Not: public UnaryBooleanExpression {
        public:
            Not( BooleanExpression *expression );

            void accept( ExpressionVisitor *visitor );
        };

        class UnaryPlus: public UnaryNumericalExpression {
        public:
            UnaryPlus( NumericalExpression *expression );

            void accept( ExpressionVisitor *visitor );
        };

        class UnaryMinus: public UnaryNumericalExpression  {
        public:
            UnaryMinus( NumericalExpression *expression );

            void accept( ExpressionVisitor *visitor );
        };

        /* 
         * SPARQL Tests as defined in
         * - http://www.w3.org/TR/2005/WD-rdf-sparql-query-20051123/#SparqlOps
         */

        class IsBound: public BooleanExpression {
        public:
            IsBound( Variable *variable );

            void setVariable( Variable *variable );
            Variable *variable();

            void accept( ExpressionVisitor *visitor );

        private:
            Variable *m_variable;
        };


        class IsIRI: public UnaryRTermBooleanExpression {
        public:
            IsIRI( RTerm *rterm );

            void accept( ExpressionVisitor *visitor );
        };

        class IsBlank: public UnaryRTermBooleanExpression {
        public:
            IsBlank( RTerm *rterm );

            void accept( ExpressionVisitor *visitor );
        };

        class IsLiteral: public UnaryRTermBooleanExpression {
        public:
            IsLiteral( RTerm *rterm );

            void accept( ExpressionVisitor *visitor );
        };

        /*
         * SPARQL Accessors
         */
        
        class StringValue: public UnaryRTermStringExpression {
        public:
            // RTerm must be a literal or resource
            StringValue( RTerm *rterm );

            void accept( ExpressionVisitor *visitor );
        };

        class LangValue: public UnaryRTermStringExpression {
        public:
            // RTerm must be a literal
            LangValue( RTerm *rterm );

            void accept( ExpressionVisitor *visitor );
        };

        class DataTypeValue: public UnaryRTermStringExpression {
        public:
            // RTerm must be a literal
            DataTypeValue( RTerm *rterm );

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

            BooleanExpression *first();
            BooleanExpression *second();
    
        private:
            BooleanExpression *m_first;
            BooleanExpression *m_second;
        };

        class BinaryNumericalBooleanExpression: public BooleanExpression {
        public:
            BinaryNumericalBooleanExpression( NumericalExpression *first, NumericalExpression *second );

            void setFirst( NumericalExpression *first );
            void setSecond( NumericalExpression *second );

            NumericalExpression *first();
            NumericalExpression *second();
   
        private:
            NumericalExpression *m_first;
            NumericalExpression *m_second;
        };

        class BinaryStringBooleanExpression: public BooleanExpression {
        public:
            BinaryStringBooleanExpression( StringExpression *first, StringExpression *second );

            void setFirst( StringExpression *first );
            void setSecond( StringExpression *second );

            StringExpression *first();
            StringExpression *second();
  
        private:
            StringExpression *m_first;
            StringExpression *m_second;
        };

        class BinaryDateTimeBooleanExpression: public BooleanExpression {
        public:
            BinaryDateTimeBooleanExpression( DateTime *first, DateTime *second );

            void setFirst( DateTime *first );
            void setSecond( DateTime *second );

            DateTime *first();
            DateTime *second();
  
        private:
            DateTime *m_first;
            DateTime *m_second;
        };

        class BinaryRTermBooleanExpression: public BooleanExpression {
        public:
            BinaryRTermBooleanExpression( RTerm *first, RTerm *second );

            void setFirst( RTerm *first );
            void setSecond( RTerm *second );

            RTerm *first();
            RTerm *second();

        private:
            RTerm *m_first;
            RTerm *m_second;
        };

        class BinaryNumericalExpression: public NumericalExpression {
        public:
            BinaryNumericalExpression( NumericalExpression *first, NumericalExpression *second );

            void setFirst( NumericalExpression *first );
            void setSecond( NumericalExpression *second );

            NumericalExpression *first();
            NumericalExpression *second();
    
        private:
            NumericalExpression *m_first;
            NumericalExpression *m_second;
        };

        class LogicOr: public BinaryBooleanExpression {
        public:
            LogicOr( BooleanExpression *first, BooleanExpression *second );

            void accept( ExpressionVisitor *visitor );
        };

        class LogicAnd: public BinaryBooleanExpression {
        public:
            LogicAnd( BooleanExpression *first, BooleanExpression *second );
            
            void accept( ExpressionVisitor *visitor );
        };

        class NumericalEqual: public BinaryNumericalBooleanExpression {
        public:
            NumericalEqual( NumericalExpression *first, NumericalExpression *second );

            void accept( ExpressionVisitor *visitor );
        };

        class NumericalNotEqual: public BinaryNumericalBooleanExpression {
        public:
            NumericalNotEqual( NumericalExpression *first, NumericalExpression *second );

            void accept( ExpressionVisitor *visitor );
        };

        class StringEqual: public BinaryStringBooleanExpression {
        public:
            StringEqual( StringExpression *first, StringExpression *second );

            void accept( ExpressionVisitor *visitor );
        };

        class StringNotEqual: public BinaryStringBooleanExpression {
        public:
            StringNotEqual( StringExpression *first, StringExpression *second );

            void accept( ExpressionVisitor *visitor );
        };

        class DateTimeEqual: public BinaryDateTimeBooleanExpression {
        public:
            DateTimeEqual( DateTime *first, DateTime *second );

            void accept( ExpressionVisitor *visitor );
        }; 

        class DateTimeNotEqual: public BinaryDateTimeBooleanExpression {
        public:
            DateTimeNotEqual( DateTime *first, DateTime *second );

            void accept( ExpressionVisitor *visitor );
        };

        class NumericalLessThan: public BinaryNumericalBooleanExpression {
        public:
            NumericalLessThan( NumericalExpression *first, NumericalExpression *second );

            void accept( ExpressionVisitor *visitor );
        };

        class NumericalGreaterThan: public BinaryNumericalBooleanExpression {
        public:
            NumericalGreaterThan( NumericalExpression *first, NumericalExpression *second );

            void accept( ExpressionVisitor *visitor );
        };

        class NumericalLessThanEqual: public BinaryNumericalBooleanExpression {
        public:
            NumericalLessThanEqual( NumericalExpression *first, NumericalExpression *second );

            void accept( ExpressionVisitor *visitor );
        };

        class NumericalGreaterThanEqual: public BinaryNumericalBooleanExpression {
        public:
            NumericalGreaterThanEqual( NumericalExpression *first, NumericalExpression *second );
        
            void accept( ExpressionVisitor *visitor );
        };

        class StringLessThan: public BinaryStringBooleanExpression {
        public:
            StringLessThan( StringExpression *first, StringExpression *second );

            void accept( ExpressionVisitor *visitor );
        };

        class StringGreaterThan: public BinaryStringBooleanExpression {
        public:
            StringGreaterThan( StringExpression *first, StringExpression *second );
        
            void accept( ExpressionVisitor *visitor );
        };

        class StringLessThanEqual: public BinaryStringBooleanExpression {
        public:
            StringLessThanEqual( StringExpression *first, StringExpression *second );

            void accept( ExpressionVisitor *visitor );
        };

        class StringGreaterThanEqual: public BinaryStringBooleanExpression {
        public:
            StringGreaterThanEqual( StringExpression *first, StringExpression *second );

            void accept( ExpressionVisitor *visitor );
        };

        class DateTimeLessThan: public BinaryDateTimeBooleanExpression {
        public:
            DateTimeLessThan( DateTime *first, DateTime *second );
        
            void accept( ExpressionVisitor *visitor );
        };

        class DateTimeGreaterThan: public BinaryDateTimeBooleanExpression {
        public:
            DateTimeGreaterThan( DateTime *first, DateTime *second );

            void accept( ExpressionVisitor *visitor );
        };

        class DateTimeLessThanEqual: public BinaryDateTimeBooleanExpression {
        public:
            DateTimeLessThanEqual( DateTime *first, DateTime *second );

            void accept( ExpressionVisitor *visitor );
        };

        class DateTimeGreaterThanEqual: public BinaryDateTimeBooleanExpression {
        public:
            DateTimeGreaterThanEqual( DateTime *first, DateTime *second );

            void accept( ExpressionVisitor *visitor );
        };

        class NumericalMultiply: public BinaryNumericalExpression {
        public:
            NumericalMultiply( NumericalExpression *first, NumericalExpression *second );

            void accept( ExpressionVisitor *visitor ); 
        };

        class NumericalDivide: public BinaryNumericalExpression {
        public:
            NumericalDivide( NumericalExpression *first, NumericalExpression *second );

            void accept( ExpressionVisitor *visitor );
        };

        class NumericalAdd: public BinaryNumericalExpression {
        public:
            NumericalAdd( NumericalExpression *first, NumericalExpression *second );

            void accept( ExpressionVisitor *visitor );
        };

        class NumericalSubtract: public BinaryNumericalExpression {
        public:
            NumericalSubtract( NumericalExpression *first, NumericalExpression *second );

            void accept( ExpressionVisitor *visitor );
        };

        /*          
         * SPARQL Tests as defined in:         
         * - http://www.w3.org/TR/2005/WD-rdf-sparql-query-20051123/#SparqlOps       
         */

        class RTermEqual: public BinaryRTermBooleanExpression {
        public:
            RTermEqual( RTerm *first, RTerm *second );

            void accept( ExpressionVisitor *expression );
        };

        class RTermNotEqual: public BinaryRTermBooleanExpression {
        public:
            RTermNotEqual( RTerm *first, RTerm *second );

            void accept( ExpressionVisitor *expression );
        };

        class LangMatches: public BinaryStringBooleanExpression {
        public:
            LangMatches( StringExpression *first, StringExpression *second );

            void accept( ExpressionVisitor *expression );
        }; 

        class Regexp: public BooleanExpression {
        public:
            Regexp( StringExpression *expression, const QString &pattern);
            Regexp( StringExpression *expression, const QString &pattern, const QString &flags );

            void setExpression( StringExpression *expression );
            StringExpression *expression();

            void setPattern( const QString &pattern );
            QString pattern();

            void setFlags( const QString &flags );
            QString flags();

            void accept( ExpressionVisitor *expression );

        private:
            StringExpression *m_expression;
            QString m_pattern;  
            QString m_flags;
        }; 

        ////////////////////////////////////////////////////////////////////////
        // ExpressionVisitor                                                  //
        ////////////////////////////////////////////////////////////////////////

        class ExpressionVisitor {
        public:
            virtual void visit( Not *expression ) = 0;

            virtual void visit( UnaryPlus *expression ) = 0;

            virtual void visit( UnaryMinus *expression ) = 0;
            
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

            virtual void visit( NumericalLessThan *expression ) = 0;

            virtual void visit( NumericalGreaterThan *expression ) = 0;
            
            virtual void visit( NumericalLessThanEqual *expression ) = 0;

            virtual void visit( NumericalGreaterThanEqual *expression ) = 0;

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
            Prefix( const QString &prefix, const QUrl &uri );

            const QString prefix() const;
            const QUrl uri() const;

        private:
            QString m_prefix;
            QUrl m_uri;
        };

        class QueryLiteral {
        public:
            QueryLiteral( Expression *expression );
            QueryLiteral( RTerm *rtem );

            bool isRTerm();

            RTerm *rtem();

            Expression *expression();

        private:
            Expression *m_expression;
            RTerm *m_rterm;
        };

        class TriplePattern {
        public:
            TriplePattern( QueryLiteral *subject, QueryLiteral *predicate, QueryLiteral *object ); 

            QueryLiteral *subject();
            QueryLiteral *predicate(); 
            QueryLiteral *object(); 

        private:
            QueryLiteral *m_subject;
            QueryLiteral *m_predicate;
            QueryLiteral *m_object;

            //FIXME: what does it mean?
            //Node *m_origin;
        };


        class GraphPattern {
            
            enum Operator {
                BASIC = 0, 
                OPTIONAL,
                UNION,
                GROUP,
                GRAPH
            };

        public:
            GraphPattern( TriplePattern *triple, Operator op = BASIC, GraphPattern *parent = NULL );

            TriplePattern *triple();
            
            // SPARQL FILTER
            void setConstraint( Expression *expression );
            Expression *constraint();

            void addSubGraphPattern( GraphPattern *subgraph );
            QList<GraphPattern *> childGraphs();

            GraphPattern *parentGraph();
    
        private:
            TriplePattern *m_triple;
            Expression *m_constraint;

            GraphPattern *m_parent;
            QList<GraphPattern *> m_childs;
        };

        class QueryObject {
        public:
            void addPrefix( Prefix *prefix );
            QList<Prefix *> prefixes();

            //void addVariable( Variable *variable );
            //QList<Variable *> variables();

            //void addGraphPattern( GraphPattern *graphPattern );
            //QList<GraphPattern *> graphPatterns();

            void setQueryVerb( const QString &queryVerb );
            QString queryVerb();

            // SELECT *
            void setWildCard( bool wildCard );
            bool isWildCard();

        private:
            QString m_queryVerb;
            bool m_wildCard;
        };
    };
};

#endif // SOPRANO_QUERY_API_H
