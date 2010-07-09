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

#ifndef _SOPRANO_MODEL_MONITOR_H_
#define _SOPRANO_MODEL_MONITOR_H_

#include <QtCore/QObject>

namespace Soprano {

    class Model;
    class Statement;

    class ModelMonitor : public QObject
    {
        Q_OBJECT

    public:
        ModelMonitor( Soprano::Model* model );
        ~ModelMonitor();

        void monitor( const Statement& pattern );

    public Q_SLOTS:
        void stopMonitoring();

    private Q_SLOTS:
        void slotStatementAdded( const Soprano::Statement& );
        void slotStatementRemoved( const Soprano::Statement& );

    private:
        class Private;
        Private* const d;
    };
}

#endif
