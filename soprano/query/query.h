namespace Soprano
{
    namespace Query {

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
        // Unary ExpressionOperators                                          //
        ////////////////////////////////////////////////////////////////////////

        /*class ExpressionOperator {
        public:
            virtual void accept( Visitor *visitor ) = 0;
        };*/

        ////////////////////////////////////////////////////////////////////////
        // Unary ExpressionOperators                                          //
        ////////////////////////////////////////////////////////////////////////

        /* XQUERY Tests */

        /*class Not: public ExpressionOperator {
        public:
            void accept( Visitor *visitor );
        };

        class UnaryPlus: public ExpressionOperator  {
        public:
            UnaryPlus( Numerical *num );

            void accept( Visitor *visitor );
        };

        class UnaryMinus: public ExpressionOperator  {
        public:
            UnaryMinus( Numerical *num );

            void accept( Visitor *visitor );
        };*/

        /* 
         * SPARQL Tests as defined in:
         * - http://www.w3.org/TR/2005/WD-rdf-sparql-query-20051123/#func-isBound
         */

        /*class IsBound: public ExpressionOperator {
        public:
            IsBound( Variable *var );

            void accept( Visitor *visitor );
        };


        class IsIRI: public ExpressionOperator {
        public:
            IsIRI( Soprano::Node *node );

            void accept( Visitor *visitor );
        };

        class IsBlank: public ExpressionOperator {
        public:
            IsBlank( Soprano::Node *node );

            void accept( Visitor *visitor );
        };

        class IsLiteral: public ExpressionOperator {
        public:
            IsLiteral( Soprano::Node *node );

            void accept( Visitor *visitor );
        };*/

        /*
         * SPARQL Accessors
         */
        
        /*class StringValue: public ExpressionOperator {
        public:
            // Soprano::Node must be a literal
            StringValue( Soprano::Node *node );

            void accept( Visitor *visitor );
        };

        class LangValue: public ExpressionOperator {
        public:
            // Soprano::Node must be a literal
            LangValue( Soprano::Node *node );

            void accept( Visitor *visitor );
        };

        class DataTypeValue: public ExpressionOperator {
        public:
            // Soprano::Node must be a literal
            DataTypeValue( Soprano::Node *node );

            void accept( Visitor *visitor );
        };*/

        ////////////////////////////////////////////////////////////////////////
        // Binary ExpressionOperators                                         //
        ////////////////////////////////////////////////////////////////////////

        /*class LogicOr: public ExpressionOperator {
        public:
            LogicOr( BooleanExpression *first, BooleanExpression *second );

            void accept( Visitor *visitor );
        };

        class LogicAnd: public ExpressionOperator {
        public:
            LogicAnd( BooleanExpression *first, BooleanExpression *second );
            
            void accept( Visitor *visitor );
        };

        class NumericalEqual: public ExpressionOperator {
        public:
            NumericalEqual( Numerical *first, Numerical *second );

            void accept( Visitor *visitor );
        };

        class StringEqual: public ExpressionOperator {
        public:
            StringEqual( QString *first, QString *second );

            void accept( Visitor *visitor );
        };

        class DateTimeEqual: public ExpressionOperator {
        public:
            DateTimeEqual( DateTime *first, DateTime *second );

            void accept( Visitor *visitor );
        }; 

        class NumericalNotEqual: public ExpressionOperator {
        public:
            NumericalNotEqual( Numerical *first, Numerical *second );

            void accept( Visitor *visitor );
        };

        class StringNotEqual: public ExpressionOperator {
        public:
            StringNotEqual( QString *first, QString *second );

            void accept( Visitor *visitor );
        };

        class DateTimeNotEqual: public ExpressionOperator {
        public:
            DateTimeNotEqual( DateTime *first, DateTime *second );

            void accept( Visitor *visitor );
        };*/


        ////////////////////////////////////////////////////////////////////////
        // Query                                                              //
        ////////////////////////////////////////////////////////////////////////

        // FIXME: is this a Statement ???
        /*class TiplePattern {
        public:
            TriplePattern( Node *subject, Node *predicate, Node *object, ); 

            Node *subject();
            Node *predicate(); 
            Node *object(); 

        private:
            Node *m_subject;
            Node *m_predicate;
            Node *m_object;

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
            // FIXME: may a Node be also a Variable?for me yes
            GraphPattern( TriplePattern *triple, Operator op = BASIC );

            void addConstraint( ExpressionOperator *expression );

            void addSubGraphPattern( GraphPattern *subgraph, ExpressionOperator *expression );

            const Triple *triple();

        private:
            TriplePattern *m_triple;

            GraphPattern *m_parent;
            QList<GraphPattern *> m_subgraphs;
        };

        class Query {
        public:
            void addPrefix( Prefix *prefix );
            QList<Prefix *> prefixes();

            void addVariable( Variable *variable );
            QList<Variable *> variables();

            void addGraphPattern( GraphPattern *graphPattern );
            QList<GraphPattern *> graphPatterns();

            void setQueryVerb( const QString &queryVerb );
            QString queryVerb();

            // SELECT *
            bool isWildCard();
        };*/

                 
    };
};

