#include "query.h"

Soprano::Query::Integer::Integer( int value )
    :m_value(value)
{
}

int Soprano::Query::Integer::value() const
{
    return m_value;
}

Soprano::Query::Float::Float( float value )
    :m_value(value)
{
}

float Soprano::Query::Float::value() const
{
    return m_value;
}

Soprano::Query::Double::Double( int value )
    :m_value(value)
{
}

double Soprano::Query::Double::value() const
{
    return m_value;
}

