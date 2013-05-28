#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QTime>

#include "soprano.h"

#include "virtuosobackend.h"
#include "virtuosomodel.h"

using namespace Soprano;

int main( int argc, char** argv )
{
    QCoreApplication app( argc, argv );

    Soprano::Virtuoso::BackendPlugin bp;
    bp.isAvailable();

    Soprano::BackendSettings settings;
    settings << Soprano::BackendSetting( Soprano::BackendOptionHost, "localhost" );
    settings << Soprano::BackendSetting( Soprano::BackendOptionPort, "1113" );
    settings << Soprano::BackendSetting( Soprano::BackendOptionUsername, "dba" );
    settings << Soprano::BackendSetting( Soprano::BackendOptionPassword, "dba" );
    settings << Soprano::BackendSetting( "noStatementSignals", true );
    settings << Soprano::BackendSetting( "fakeBooleans", false );
    settings << Soprano::BackendSetting( "emptyGraphs", false );

    StorageModel* model = bp.createModel(settings);
    QueryResultIterator it = model->executeQuery( argv[1], Query::QueryLanguageSparql );
    //qDebug() << "bindingCount:" << it.bindingCount();
    //qDebug() << "bindingNames:" << it.bindingNames();
    QTime timer;
    timer.start();
    while ( it.next() ) {
        *it;
    }
    qDebug() << timer.elapsed()/1000.0;

    return 0;
}
