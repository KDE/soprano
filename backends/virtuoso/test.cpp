#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>

#include "soprano.h"

#include "iodbcmodel.h"

using namespace Soprano;

int main( int argc, char** argv )
{
    QCoreApplication app( argc, argv );
    IODBCModel model;
    if ( !model.connect( "host=localhost:1111;uid=dba;pwd=dba;driver=/usr/local/lib/virtodbc.so;" ) ) {
        qDebug() << "Failed to connect to db";
        return 1;
    }
    QueryResultIterator it = model.executeQuery( argv[1], Query::QueryLanguageSparql );
    qDebug() << "bindingCount:" << it.bindingCount();
    qDebug() << "bindingNames:" << it.bindingNames();
    while ( it.next() ) {
        qDebug() << *it;
    }
    return 0;
}
