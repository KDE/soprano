namespace Soprano
{
    namespace Query {

        class Condition {
            void accept( Visitor &visitor );
        };

        // Logic

        class LogicAnd: public Condition {
        public:
            LogicAnd(Condition *first, Condition *second);
            virtual ~LogicAnd();
        };

        class LogicOr: public Condition {
        public:
            LogicOr(Condition *first, Condition *second);
            virtual ~LogicOr();
        };

        class LogicNot: public Condition {
        public:
            LogicNot(Condition *first);
            virtual ~LogicNot();
        };

        class Equal: public Condition {
        public:
            Equal(Condition *first, Condition *second);
            virtual ~Equal();
        };
    
        class NotEqual: public Condition {
        public:
            NotEqual(Condition *first, Condition *second);
            virtual ~NotEqual();
        };

        class LowerThen: public Condition {
        public:
            LowerThen(Condition *first, Condition *second);
            virtual ~LowerThen();
        };

        class GreaterThen: public Condition {
        public:
            GreaterThen(Condition *first, Condition *second);
            virtual ~GreaterThen();
        };

        class LowerEqual: public Condition {
        public:
            LowerEqual(Condition *first, Condition *second);
            virtual ~LowerEqual();
        };

        class GreaterEqual: public Condition {
        public:
            GreaterEqual(Condition *first, Condition *second);
            virtual ~GreaterEqual();
        };

        // Binary

        class BinaryNot: public Condition {
        public:
            BinaryNot(Condition *first);
            virtual BinaryNot();
        };

        // Expression

        class SignedMinus: public Condition {
        public:
            SignedMinus(Condition *condition);
            virtual ~SignedMinus();
        };

        class Plus: public Condition {
        public:
            Plus(Condition *condition);
            virtual ~ConditionPlus();
        };

        class Minus: public Condition {
        public:
            Minus(Condition *condition);
            virtual ~Minus();
        };

        class Multiplication: public Condition {
        public:
            Multiplication(Condition *first, Condition *second);
            virtual ~Multiplication();
        };

        class Division: public Condition {
        public:
            Division(Condition *first, Condition *second);
            virtual ~Division();
        };

        class DivisionWithRest: public Condition {
        public:
            DivisionWithRest(Condition *first, Condition *second);
            virtual ~DivisionWithRest();
        };

        // String

        class StringEqual: public Condition {
        public:
            StringEqual(const QString &first, const QString &second);
        };

        class StringNotEqual: public Condition {
        public:
            StringNotEqual(const QString &first, const QString &second);
        };

        class StringNegativeMatch: public Condition {
        public:
            StringNegativeMatch(const QString &first, const QString &second);
        };

        class Literal: public Condition {
        public:
            Literal(LiteralValue value);
        };

        class IsBound: public Condition {
        public:
            IsBound(Condition *condition)
            virtual ~IsBound();
        };

        

        //class Function: public Condition {
        //public:
        //    Function(const QString &name, const QList<Arguments>&va 
        //};

        class ConditionVisitor {
        public:
            void visitLogicAnd(LogicAnd &condition);
    
            void visitLogicOr(LogicOr &condition);

            void visitLogicNot(LogicNot &condition);

            void visitEqual(Equal &condition);
        }; 

    };
};
