/*
 * This file is part of Soprano Project
 *
 * Copyright (C) 2009 Sebastian Trueg <trueg@kde.org>
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

#ifndef _SOPRANO_VIRTUOSO_CONTROLLER_H_
#define _SOPRANO_VIRTUOSO_CONTROLLER_H_

#include <QtCore/QObject>
#include <QtCore/QProcess>

#include "error.h"
#include "backend.h"

class QEventLoop;

namespace Soprano {
    class VirtuosoController : public QObject,  public Soprano::Error::ErrorCache
    {
        Q_OBJECT

    public:
        VirtuosoController();
        ~VirtuosoController();

        enum ExitStatus {
            NormalExit,
            ForcedExit,
            CrashExit,
            ThirdPartyExit
        };

        enum Status {
            NotRunning,
            StartingUp,
            Running,
            ShuttingDown,
            Killing
        };

        enum RunFlag {
            NoFlags = 0,
            DebugMode /**< Debug mode will not delete the config file after finishing. */
        };
        Q_DECLARE_FLAGS( RunFlags, RunFlag )

        bool start( const BackendSettings& settings, RunFlags = NoFlags );
        Q_INVOKABLE bool shutdown();

        int usedPort() const;

        bool isRunning() const;
        Status status() const { return m_status; }
        ExitStatus lastExitStatus() const { return m_lastExitStatus; }

        static QString locateVirtuosoBinary();

    Q_SIGNALS:
        void started();
        void stopped( ExitStatus status );

    private Q_SLOTS:
        void slotProcessFinished( int exitCode, QProcess::ExitStatus exitStatus );
        void slotProcessReadyRead();

    private:
        void writeConfigFile( const QString& path, const BackendSettings& settings );
        bool waitForVirtuosoToInitialize();

        QProcess m_virtuosoProcess;
        QString m_configFilePath;

        RunFlags m_runFlags;
        quint16 m_port;
        Status m_status;
        ExitStatus m_lastExitStatus;

        QEventLoop* m_initializationLoop;
    };
}

Q_DECLARE_OPERATORS_FOR_FLAGS( Soprano::VirtuosoController::RunFlags )

#endif
