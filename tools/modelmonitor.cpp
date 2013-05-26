/*
 *
 * $Id: sourceheader 511311 2006-02-19 14:51:05Z trueg $
 *
 * This file is part of the Soprano project.
 * Copyright (C) 2009 Sebastian Trueg <trueg@kde.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * See the file "COPYING" for the exact licensing terms.
 */

#include "modelmonitor.h"

#include "statement.h"
#include "model.h"

#include <QtCore/QEventLoop>


class Soprano::ModelMonitor::Private
{
public:
    Model* model;
    Statement pattern;
    QEventLoop* loop;
    int addedCnt;
    int removedCnt;
};


Soprano::ModelMonitor::ModelMonitor( Soprano::Model* model )
    : d( new Private() )
{
    d->model = model;
    d->loop = 0;
}


Soprano::ModelMonitor::~ModelMonitor()
{
    delete d;
}


void Soprano::ModelMonitor::monitor( const Statement& pattern )
{
    QEventLoop loop;
    d->loop = &loop;
    d->pattern = pattern;
    d->addedCnt = 0;
    d->removedCnt = 0;
    connect( d->model, SIGNAL(statementAdded(Soprano::Statement)),
             this, SLOT(slotStatementAdded(Soprano::Statement)) );
    connect( d->model, SIGNAL(statementRemoved(Soprano::Statement)),
             this, SLOT(slotStatementRemoved(Soprano::Statement)) );
    loop.exec();
    d->model->disconnect( this );

    QTextStream outStream( stdout );
    outStream << d->addedCnt << " statement added and " << d->removedCnt << " statement removed events." << endl;
}


void Soprano::ModelMonitor::stopMonitoring()
{
    if ( d->loop )
        d->loop->exit();
}


void Soprano::ModelMonitor::slotStatementAdded( const Soprano::Statement& s )
{
    if ( s.matches( d->pattern ) ) {
        QTextStream outStream( stdout );
        outStream << QString( "%1 | " ).arg( d->removedCnt + d->addedCnt++, 4, 10, QChar('0') ) << "Added:   " << s << endl;
    }
}


void Soprano::ModelMonitor::slotStatementRemoved( const Soprano::Statement& s )
{
    // we do not use Statement::matches here since we want to match empty nodes on both sides.
    // not only in the pattern. This is because some backends do not emit the statementRemoved
    // signal on each single statement but on the pattern used.
    if ( ( d->pattern.subject().isEmpty() ||
           s.subject().isEmpty() ||
           d->pattern.subject() == s.subject() ) &&
         ( d->pattern.predicate().isEmpty() ||
           s.predicate().isEmpty() ||
           d->pattern.predicate() == s.predicate() ) &&
         ( d->pattern.object().isEmpty() ||
           s.object().isEmpty() ||
           d->pattern.object() == s.object() ) &&
         ( d->pattern.context().isEmpty() ||
           s.context().isEmpty() ||
           d->pattern.context() == s.context() ) ) {
        QTextStream outStream( stdout );
        outStream << QString( "%1 | " ).arg( d->addedCnt + d->removedCnt++, 4, 10, QChar('0') ) << "Removed: " << s << endl;
    }
}

