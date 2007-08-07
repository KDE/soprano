namespace Soprano
{

    namespace Query {

	class QueryNode {
            /**
             * Accept a Visitor, allowing it to change this condition.
             */
            virtual void accept( const Visitor *visitor ) = 0; // calls the non-const method on the visitor
            virtual void accept( const Visitor *visitor ) const = 0; // calls the const methos on the visitor	
	};

	class Condition : public QueryNode {
	};
	
        // Logic
        
        // TODO: Memory management
	class LogicalBooleanCondition : public Condition
	{
	public:
	LogicalBooleanCondition();
	LogicalBooleanCondition( Condition* left, Condition* right );
	
	// do we want set-methods here?
	void setLeft( Condition* );
	void setRight( Condition* );
	
	const Condition* right() const;
	const Condition* left() const;

	Condition* right();
	Condition* left();
	
	private:
	Condition* m_left;
	Condition* m_right;
	};

	/**
	 * LogicAnd surely is a Condition as it is either true or false.
	 * And it has without a doubt two subconditions.
	 */
	class LogicAnd: public LogicalBooleanCondition {
        public:
            LogicAnd(Condition *left, Condition *right);
            virtual ~LogicAnd();
            
            void accept( const Visitor *visitor );
            void accept( const Visitor *visitor ) const;
        };

	/**
	 * Same as with LoginAnd: a Condition for sure.
	 */
        class LogicOr: public LogicalBooleanCondition {
        public:
            LogicOr(Condition *first, Condition *second);
            virtual ~LogicOr();
            
            void accept( const Visitor *visitor );
            void accept( const Visitor *visitor ) const;
        };

	/**
	 * Not(A), without a doubt a Condition that wraps one
	 * subcondition.
	 */ 
        class LogicNot: public Condition {
        public:
            LogicNot(Condition *condition);
            virtual ~LogicNot();
            
            Condition* condition() const;
            
            void accept( const Visitor *visitor );
            void accept( const Visitor *visitor ) const;
        };

	class LiteralBooleanCondition : public Condition {
	public:
	LiteralBooleanCondition( Literal* left, Literal* right );
	
	void setLeft( Literal* left );
	void setRight( Literal *right );
	
	Literal* left() const;
	Literal* right() const;

	const Literal* left() const;
	const Literal* right() const;
	
	private:
	Literal* m_left;
	Literal* m_right;
	};
	
	
	/**
	 * This is a Condition, for sure, but is it a LogicalBooleanCondition? I dont think so. and can accept all the Contions classes?
	 * pretty much, we can check if two conditions evaluate to the same but we can also check if two variables evaluate to the same, right?
	 * or even a var and a resource, but not a var and a condition.... so should we have two difernt equals? One for conditions and one for
	 * literals and stuff?..i'm thinking
	 * a == b <=> ( a && b || !a && !b ) in the case of logic vars
	 */
        class Equal: public LiteralBooleanCondition {
        public:
            Equal(Literal *first, Literal *second);
            virtual ~Equal();
            
            void accept( const Visitor *visitor );
            void accept( const Visitor *visitor ) const;
        };

        /**
         * This would be XOR if it was logical, right?... but is it logic?...as equal i don't think are logic operator in that case we cannot derive from LogicalBooleanCondition..from aritmetic? dont know, maybe get the queryNode back abd use that for starters. Then clean up the rest and get back here?is ok for me
         */
        class NotEqual: public LiteralBooleanCondition {
        public:
            NotEqual(Literal *first, Literal *second);
            virtual ~NotEqual();
            
            void accept( const Visitor *visitor );
            void accept( const Visitor *visitor ) const;
        };

	/**
	 * Hmm, and if we use Literal as base for strings, numbers.... yes, that is probably it. These operators work on Literals.....yes "hello" != "hel" or "hello" >= "hel" 
	 * IN the end all thse return true and false again, so maybe they are Conditions after all....yes we can try with condition
	 */
        class LessThan: public LiteralBooleanCondition { // <------- isn't less the typical name?
        public:
            LessThan(Literal *first, Literal *second);
            virtual ~LessThan();
            
            void accept( const Visitor *visitor );
            void accept( const Visitor *visitor ) const;
        };

        class GreaterThan: public LiteralBooleanCondition {
        public:
            GreaterThan(Literal *first, Literal *second);
            virtual ~GreaterThan();
            
            void accept( const Visitor *visitor );
            void accept( const Visitor *visitor ) const;
        };

        class LessEqual: public LiteralBooleanCondition {
        public:
            LessEqual(Literal *first, Literal *second);
            virtual ~LessEqual();
            
            void accept( const Visitor *visitor );
            void accept( const Visitor *visitor ) const;
        };

        class GreaterEqual: public LiteralBooleanCondition {
        public:
            GreaterEqual(Literal *first, Literal *second);
            virtual ~GreaterEqual();
            
            void accept( const Visitor *visitor );
            void accept( const Visitor *visitor ) const;
        };

        // Binary

	/**
	* where is the diffence to LogicNot? could be a bit not ? ah, ok, do we have that in query langs? RASQAL_EXPR_TILDE, but for a Condition that does not make sense, maybe for a number, i.e. a Literal subclass?
	*/
        class BinaryNot: public Condition {
        public:
            BinaryNot(Condition *first);
            virtual BinaryNot();
            
            void accept( const Visitor *visitor );
            void accept( const Visitor *visitor ) const;
        };

        // Expression
        class Operator : public QueryNode {
        
        };

	class MathExpression : public QueryNode {
	
	};
	
	/**
	 * This is probably not a Condition becasue it evaluates to a number. Same for all the others.
	 */
        class SignedMinus: public MathExpression {
        public:
            SignedMinus(MathExpression *condition);
            virtual ~SignedMinus();
            
            void accept( const Visitor *visitor );
            void accept( const Visitor *visitor ) const;
        };

	
        class Plus: public Condition {
        public:
            Plus(Condition *condition);
            virtual ~ConditionPlus();
            
            void accept( const Visitor *visitor );
            void accept( const Visitor *visitor ) const;
        };

        class Minus: public Condition {
        public:
            Minus(Condition *condition);
            virtual ~Minus();
            
            void accept( const Visitor *visitor );
            void accept( const Visitor *visitor ) const;
        };

        class Multiplication: public Condition {
        public:
            Multiplication(Condition *first, Condition *second);
            virtual ~Multiplication();
            
            void accept( const Visitor *visitor );
            void accept( const Visitor *visitor ) const;
        };

        class Division: public Condition {
        public:
            Division(Condition *first, Condition *second);
            virtual ~Division();
            
            void accept( const Visitor *visitor );
            void accept( const Visitor *visitor ) const;
        };

        class DivisionWithRest: public Condition {
        public:
            DivisionWithRest(Condition *first, Condition *second);
            virtual ~DivisionWithRest();
            
            void accept( const Visitor *visitor );
            void accept( const Visitor *visitor ) const;
        };

        // String

        class StringEqual: public Condition {
        public:
            StringEqual(const QString &first, const QString &second);
            
            void accept( const Visitor* );
        };

        class StringNotEqual: public Condition {
        public:
            StringNotEqual(const QString &first, const QString &second);
            
            void accept( const Visitor *visitor );
            void accept( const Visitor *visitor ) const;
        };

        class StringNegativeMatch: public Condition {
        public:
            StringNegativeMatch(const QString &first, const QString &second);
            
            void accept( const Visitor *visitor );
            void accept( const Visitor *visitor ) const;
        };

	class Function : public Condition ? 
	
        class Bound: public Condition {
        public:
            Bound(Variable *variable);
            virtual ~Bound();
            
            void accept( const Visitor *visitor );
            void accept( const Visitor *visitor ) const;
        };
  
        class Variable : public Condition {
        public:
        QString name() const;

        private:
        QString m_name;
        }; 
        
        class Resource : public Condition {
        public:
            Soprano::Node resource() const;
        
            void accept( const Visitor *visitor );
            void accept( const Visitor *visitor ) const;
        
        private:
        Soprano::Node m_node;
        };
        
        class Literal: public Condition {
        public:
            Literal(LiteralValue value);
            
            void accept( const Visitor *visitor );
            void accept( const Visitor *visitor ) const;
        };
        
        class GraphPattern : public Condition {
        public:
        Condition* subject; // can be a Variable or a Resource
        Condition* predicate; // can be a Variable or a Resource
        Condition* object; // can be a Variable, Resource, or Literal
        Condition* context; // can be Variable, Resource, or Blank (ignore context)
        };
        
        //class Function: public Condition {
        //public:
        //    Function(const QString &name, const QList<Arguments>&va 
        //};

        class ConditionVisitor {
        public:
	    /**
	     * Visit a LogicAnd condition which can be changed.
	     * The default implementation simply calls the const version.
	     */
            void visit(LogicAnd* condition) const;
            void visit(const LogicAnd* condition) const = 0;
    
            void visit(LogicOr *condition);
            void visit(const LogicOr *condition) const = 0;

            void visit(LogicNot *condition);
            void visit(const LogicNot *condition) const = 0;

            void visit(Equal *condition);
            void visit(const Equal *condition) const = 0;
        }; 


        /*

         */

    
        class Numerical : public MathExpression {
        public:
        };
    
        class Double : public Numerical {
        public:
            double value() const;
        };
    
        class Integer : public Numerical {
        public:
            int value() const;
        };

        ////////////////////////////////////////////////////////////////////////
        // Unary Operators                                                    //
        ////////////////////////////////////////////////////////////////////////

        class Operator {
        public:
            virtual void accept( Visitor *visitor ) = 0;
        };

        ////////////////////////////////////////////////////////////////////////
        // Unary Operators                                                    //
        ////////////////////////////////////////////////////////////////////////

        /* XQUERY Tests */

        class Not: public Operator {
        public:
            void accept( Visitor *visitor );
        };

        class UnaryPlus: public Operator  {
        public:
            UnaryPlus( Numerical *num );

            void accept( Visitor *visitor );
        };

        class UnaryMinus: public Operator  {
        public:
            UnaryMinus( Numerical *num );

            void accept( Visitor *visitor );
        };

        /* 
         * SPARQL Tests as defined in:
         * - http://www.w3.org/TR/2005/WD-rdf-sparql-query-20051123/#func-isBound
         */

        class IsBound: public Operator {
        public:
            IsBound( Variable *var );
        };


        class IsIRI: public Operator {
        public:
            IsIRI( Soprano::Node *node );

            void accept( Visitor *visitor );
        };

        class IsBlank: public Operator {
        public:
            IsBlank( Soprano::Node *node );

            void accept( Visitor *visitor );
        };

        class IsLiteral: public Operator {
        public:
            IsLiteral( Soprano::Node *node );

            void accept( Visitor *visitor );
        };

        /*
         * SPARQL Accessors
         */
        
        class StringValue: public Operator {
        public:
            /* Soprano::Node must be a literal */
            StringValue( Soprano::Node *node );

            void accept( Visitor *visitor );
        };

        class LangValue: public Operator {
        public:
            /* Soprano::Node must be a literal */
            LangValue( Soprano::Node *node );

            void accept( Visitor *visitor );
        };

        class DataTypeValue: public Operator {
        public:
            /* Soprano::Node must be a literal */
            DataTypeValue( Soprano::Node *node );

            void accept( Visitor *visitor );
        }; 

        ////////////////////////////////////////////////////////////////////////
        // Binary Operators                                                   //
        ////////////////////////////////////////////////////////////////////////

        class LogicOr: public Operator {
        public:
            LogicOr( BooleanExpression *first, BooleanExpression *second );

            void accept( Visitor *visitor );
        };

        class LogicAnd: public Operator {
        public:
            LogicAnd( BooleanExpression *first, BooleanExpression *second );
            
            void accept( Visitor *visitor );
        };

        class NumericalEqual: public Operator {
        public:
            NumericalEqual( Numerical *first, Numerical *second );

            void accept( Visitor *visitor );
        };

        class StringEqual: public Operator {
        public:
            StringEqual( QString *first, QString *second );

            void accept( Visitor *visitor );
        };

        class DateTimeEqual: public Operator {
        public:
            DateTimeEqual( DateTime *first, DateTime *second );

            void accept( Visitor *visitor );
        }; 

        class NumericalNotEqual: public Operator {
        public:
            NumericalNotEqual( Numerical *first, Numerical *second );

            void accept( Visitor *visitor );
        };

        class StringNotEqual: public Operator {
        public:
            StringNotEqual( QString *first, QString *second );

            void accept( Visitor *visitor );
        };

        class DateTimeNotEqual: public Operator {
        public:
            DateTimeNotEqual( DateTime *first, DateTime *second );

            void accept( Visitor *visitor );
        };

         
    };
};

