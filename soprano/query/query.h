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

namespace Soprano
{
    class Node;

    namespace Query {

        class Variable {
        public:
            Variable();
            Variable( const QString &name );
            Variable( const Variable &other );

            QString name() const;

        private:
            class Private;
            QSharedDataPointer<Private> d;
        };

	// why not inherit Varianble from RTerm?
        class RTerm {
        public:
            RTerm( const Soprano::Node &node );
            RTerm( const Variable &binding );
            ~RTerm();
 
            bool isBinding() const;

            Soprano::Node node() const;
            Variable binding() const;

        private:
            class Private;
            Private *d;
        };

        class Numerical {
        };
   
        class Decimal: public Numerical {
        };
 
        class Double : public Decimal {
        public:
            Double( double value );
            Double( const Double &other );

            double value() const;

        private:
            double m_value;
        };

        class Float: public Decimal {
        public:
            Float( float value );
            Float( const Float &other );
            
            float value() const;

        private:
            float m_value;
        };

        class Integer : public Numerical {
        public:
            Integer( int value );
            Integer( const Integer &other );

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
	    // how about:
	    // bool evaluate() const;
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

	// why does a this expression which is derived from NumericalExpression have another
        // NumericalExpression as parameter? Shouldn't the Numerical classes also be expressions
	// of some kind so they can be nested here?
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

	// what does unary "+" do?
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
            BinaryDateTimeBooleanExpression( QDateTime *first, QDateTime *second );

            void setFirst( QDateTime *first );
            void setSecond( QDateTime *second );

            QDateTime *first();
            QDateTime *second();
  
        private:
            QDateTime *m_first;
            QDateTime *m_second;
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
            DateTimeEqual( QDateTime  *first, QDateTime *second );

            void accept( ExpressionVisitor *visitor );
        }; 

        class DateTimeNotEqual: public BinaryDateTimeBooleanExpression {
        public:
            DateTimeNotEqual( QDateTime  *first, QDateTime *second );

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
            DateTimeLessThan( QDateTime  *first, QDateTime *second );
        
            void accept( ExpressionVisitor *visitor );
        };

        class DateTimeGreaterThan: public BinaryDateTimeBooleanExpression {
        public:
            DateTimeGreaterThan( QDateTime  *first, QDateTime *second );

            void accept( ExpressionVisitor *visitor );
        };

        class DateTimeLessThanEqual: public BinaryDateTimeBooleanExpression {
        public:
            DateTimeLessThanEqual( QDateTime  *first, QDateTime *second );

            void accept( ExpressionVisitor *visitor );
        };

        class DateTimeGreaterThanEqual: public BinaryDateTimeBooleanExpression {
        public:
            DateTimeGreaterThanEqual( QDateTime  *first, QDateTime *second );

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
            Prefix();
            Prefix( const QString &prefix, const QUrl &uri );
            Prefix( const Prefix &other );

            const QString prefix() const;
            const QUrl uri() const;

        private:
            class Private;
            QSharedDataPointer<Private> d;
        };

        // TODO: Implement this
        class TriplePattern {
        public:
            TriplePattern( RTerm *subject, RTerm *predicate, RTerm *object ); 

            RTerm *subject();
            RTerm *predicate(); 
            RTerm *object(); 

        private:
            RTerm *m_subject;
            RTerm *m_predicate;
            RTerm *m_object;

            //FIXME: what does it mean?
            //Node *m_origin;
        };

        // TODO: Implement this
        class GraphPattern {

	    // why not implement this as expressions?
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
            
            void addConstraint( BooleanExpression *expression );
            QList<BooleanExpression *>constraints();

            void addSubGraphPattern( GraphPattern *subgraph );
            QList<GraphPattern *> childGraphs();

            GraphPattern *parentGraph();
    
        private:
            TriplePattern *m_triple;
            BooleanExpression *m_constraint;

            GraphPattern *m_parent;
            QList<GraphPattern *> m_childs;
        };

        // TODO: Implement this
        class QueryObject {
        public:
            QueryObject();

            void addPrefix( const Prefix &prefix );
            QList<Prefix> prefixes();

            //void addVariable( Variable *variable );
            //QList<Variable *> variables();

            void addGraphPattern( GraphPattern *graphPattern );
            QList<GraphPattern *> graphPatterns();

            void setQueryVerb( const QString &queryVerb );
            QString queryVerb();

            // SELECT *
            void setWildCard( bool wildCard );
            bool isWildCard();

        private:
            QList<Prefix> m_prefixes;

            QList<GraphPattern *> m_graphPatterns;
            QString m_queryVerb;
            bool m_wildCard;
        };
    };
};

#endif // SOPRANO_QUERY_API_H
