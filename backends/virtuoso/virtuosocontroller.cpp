/*
 * This file is part of Soprano Project
 *
 * Copyright (C) 2009-2012 Sebastian Trueg <trueg@kde.org>
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

#include "virtuosocontroller.h"
#include "sopranodirs.h"
#include "lockfile.h"

#include <QtCore/QTemporaryFile>
#include <QtCore/QProcess>
#include <QtCore/QSettings>
#include <QtCore/QFileInfo>
#include <QtCore/QFile>
#include <QtCore/QEventLoop>
#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtCore/QCoreApplication>

#ifndef Q_OS_WIN
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

Q_DECLARE_METATYPE( QProcess::ExitStatus )

namespace {
    quint16 getFreePortNumber() {
//         QTcpServer server;
//         if ( server.listen() ) {
//             return server.serverPort();
//         }
//         else {
//             qDebug() << "Failed to determine free port. Falling back to default 1111.";
//             return 1111;
//         }
#ifdef Q_OS_WIN
        static QMutex portNumberMutex;
        static quint16 p = 1113;
        QMutexLocker l(&portNumberMutex);
        return p++;
#else
        int p = 1113;
        while ( QFile::exists( QString( "/tmp/virt_%1" ).arg( p ) ) ) {
            ++p;
        }
        return p;
#endif
    }
}

Soprano::VirtuosoController::VirtuosoController()
    : QObject( 0 ),
      m_port( 0 ),
      m_status( NotRunning ),
      m_lastExitStatus( NormalExit )
{
    connect( &m_virtuosoProcess, SIGNAL(finished(int,QProcess::ExitStatus)),
             this, SLOT(slotProcessFinished(int,QProcess::ExitStatus)) );

    // necessary in case we are started from a thread != the main thread
    qRegisterMetaType<QProcess::ExitStatus>();
}


Soprano::VirtuosoController::~VirtuosoController()
{
    if ( m_virtuosoProcess.state() == QProcess::Running )
        shutdown();
}


bool Soprano::VirtuosoController::start( const QString &virtuosoExe, const BackendSettings &settings, RunFlags flags )
{
    switch( m_status ) {
        case NotRunning:
            break; // Try to start server
        case StartingUp:
            setError( "Virtuoso is already starting up.");
            return false;
        case Running:
            setError( "Virtuoso is already running." );
            return false;
        case ShuttingDown:
        case Killing:
            setError( "Virtuoso is not stopped yet." );
            return false;
    }

    QTemporaryFile tmpFile( QDir::tempPath() + "/virtuoso_XXXXXX.ini" );
    tmpFile.setAutoRemove( false );
    tmpFile.open();
    m_configFilePath = tmpFile.fileName();
    tmpFile.close();
    writeConfigFile( m_configFilePath, settings );
    m_runFlags = flags;

    m_status = StartingUp;

    const QString storageDir = valueInSettings( settings, BackendOptionStorageDir ).toString();

    // aquire a lock for ourselves
    m_virtuosoLock.setFileName( storageDir + QLatin1String("/soprano-virtuoso.lock") );
    int pid = 0;
    if( !m_virtuosoLock.aquireLock( &pid ) ) {
        setError( QString::fromLatin1("Another instance of Soprano (%1) is already running on the data in '%2'.")
                    .arg(pid).arg(storageDir));
        return false;
    }

    // check if another instance of Virtuoso is running
    pid = pidOfRunningVirtuosoInstance( storageDir );
    if ( pid > 0 && valueInSettings( settings, "forcedstart", false ).toBool() ) {
#ifndef Q_OS_WIN
        qDebug( "Shutting down Virtuoso instance (%d) which is in our way.", pid );
        ::kill( pid_t( pid ), SIGINT );

        // wait for at max 30 seconds for the old instance to go down
        int maxWaitSecs = 30;
        while(pidOfRunningVirtuosoInstance( storageDir ) > 0 &&
              --maxWaitSecs > 0) {
            ::sleep(1);
        }
#endif
        pid = 0;
    }

    // remove old lock files in case Virtuoso crashed
    if ( !pid ) {
        QString lockFilePath = storageDir + QLatin1String( "/soprano-virtuoso.lck" );
        if ( QFile::exists( lockFilePath ) )
            QFile::remove( lockFilePath );
    }

    QStringList args;
    args << "+foreground"
            << "+configfile" << QDir::toNativeSeparators(m_configFilePath);
#ifndef Q_OS_WIN
    args << "+wait";
#endif
    qDebug() << "Starting Virtuoso server:" << virtuosoExe << args;

    // We need to set the working directory cause virtuoso creates a temp checkpoint_in_progress file
    // in the directory it was started.
    m_virtuosoProcess.setWorkingDirectory( storageDir );
    m_virtuosoProcess.start( virtuosoExe, args, QIODevice::ReadOnly );
    m_virtuosoProcess.setReadChannel( QProcess::StandardError );
    m_virtuosoProcess.closeReadChannel( QProcess::StandardOutput );
    if ( waitForVirtuosoToInitialize(virtuosoExe, args) ) {
        clearError();
        m_status = Running;
        qDebug() << "Virtuoso started:" << m_virtuosoProcess.pid();
        return true;
    }
    else {
        setError( "Failed to start Virtuoso" );
        return false;
    }
}


bool Soprano::VirtuosoController::waitForVirtuosoToInitialize(const QString& exe, const QStringList& args)
{
    // FIXME: timeout
    if ( m_virtuosoProcess.waitForStarted() ) {
        while( m_virtuosoProcess.waitForReadyRead(-1) ) {
            while( m_virtuosoProcess.canReadLine() ) {
                QString line = QString::fromLatin1( m_virtuosoProcess.readLine() );
                qDebug() << line;
                if ( line.contains( "Delete transaction log" ) ) {
                    // Close virtuoso and restart -
                    disconnect( &m_virtuosoProcess, SIGNAL(finished(int,QProcess::ExitStatus)),
                            this, SLOT(slotProcessFinished(int,QProcess::ExitStatus)) );
                    m_virtuosoProcess.close();
                    m_virtuosoProcess.waitForFinished();
                    QString storagePath = m_virtuosoProcess.workingDirectory();
                    QFile::remove( storagePath + "/soprano-virtuoso.trx" );

                    connect( &m_virtuosoProcess, SIGNAL(finished(int,QProcess::ExitStatus)),
                            this, SLOT(slotProcessFinished(int,QProcess::ExitStatus)) );
                    m_virtuosoProcess.setWorkingDirectory(storagePath);
                    m_virtuosoProcess.start(exe, args, QIODevice::ReadOnly);
                    m_virtuosoProcess.setReadChannel( QProcess::StandardError );
                    m_virtuosoProcess.closeReadChannel( QProcess::StandardOutput );
                    m_virtuosoProcess.waitForStarted();
                    m_virtuosoProcess.waitForReadyRead(-1);
                }
                else if ( line.contains( "Server online at" ) ) {
                    m_virtuosoProcess.closeReadChannel( QProcess::StandardError );
                    m_status = Running;
                    return true;
                }
            }
        }
        return( m_status == Running );
    }
    else {
        return false;
    }
}


int Soprano::VirtuosoController::usedPort() const
{
    return m_port;
}


bool Soprano::VirtuosoController::shutdown()
{
    if ( m_virtuosoProcess.state() == QProcess::Running ) {
        qDebug() << "Shutting down virtuoso instance" << m_virtuosoProcess.pid();
#ifndef Q_OS_WIN
        m_status = ShuttingDown;
        // terminate Virtuoso with the SIGINT signal to force it to make a clean shutdown
        ::kill( pid_t( m_virtuosoProcess.pid() ), SIGINT );
        if ( !m_virtuosoProcess.waitForFinished( 30*1000 ) ) {
            qDebug() << "Killing virtuoso instance" << m_virtuosoProcess.pid();
            setError( "Virtuoso did not shut down after 30 seconds. Process killed." );
            m_status = Killing;
            m_virtuosoProcess.kill();
            m_virtuosoProcess.waitForFinished();
            m_virtuosoLock.releaseLock();
            return false;
        }
        else {
            clearError();
            m_virtuosoLock.releaseLock();
            return true;
        }
#else
        m_status = Killing;
        m_virtuosoProcess.kill();
        m_virtuosoProcess.waitForFinished();
        clearError();
        m_virtuosoLock.releaseLock();
        return true;
#endif
    }
    else {
        setError( "Virtuoso not running. Cannot shutdown." );
        m_virtuosoLock.releaseLock();
        return false;
    }
}


void Soprano::VirtuosoController::slotProcessFinished( int, QProcess::ExitStatus exitStatus )
{
    // clean up config
    if ( !( m_runFlags & DebugMode ) &&
         QFile::exists( m_configFilePath ) ) {
        QFile::remove( m_configFilePath );
    }

    // release our lock
    m_virtuosoLock.releaseLock();

    m_lastExitStatus = NormalExit;
    if ( exitStatus == QProcess::CrashExit )
        m_lastExitStatus = CrashExit;
    else if ( m_status == Killing )
        m_lastExitStatus = ForcedExit;
    else if ( m_status != ShuttingDown )
        m_lastExitStatus = ThirdPartyExit;

    m_status = NotRunning;

    qDebug() << "Virtuoso server stopped:" << m_lastExitStatus;

    emit stopped( m_lastExitStatus );
}


// TODO: optimize the settings
void Soprano::VirtuosoController::writeConfigFile( const QString& path, const BackendSettings& settings )
{
    qDebug() << Q_FUNC_INFO << path;

    // storage dir
    QString dir = valueInSettings( settings, BackendOptionStorageDir ).toString();

    // backwards compatibility
    int numberOfBuffers = valueInSettings( settings, "buffers", 2000 ).toInt();
    int numberOfThreads = valueInSettings( settings, "threads", 100 ).toInt();

    numberOfBuffers = valueInSettings( settings, "NumberOfBuffers", numberOfBuffers ).toInt();
    numberOfThreads = valueInSettings( settings, "ServerThreads", numberOfThreads ).toInt();
    int maxDirtyBuffers = valueInSettings( settings, "MaxDirtyBuffers", numberOfBuffers*2/3 ).toInt();
    int checkpointInterval = valueInSettings( settings, "CheckpointInterval", -1 ).toInt();
    int minAutoCheckpointSize = valueInSettings( settings, "MinAutoCheckpointSize", -1 ).toInt();

    // although we do not actually use a port Virtuoso uses the port number to create
    // the unix socket name.
    m_port = getFreePortNumber();

    if ( !dir.endsWith( '/' ) )
        dir += '/';
    dir = QDir::toNativeSeparators( dir );

    QSettings cfs( path, QSettings::IniFormat );

    cfs.beginGroup( "Database" );
    cfs.setValue( "DatabaseFile", dir + "soprano-virtuoso.db" );
    cfs.setValue( "ErrorLogFile", dir + "soprano-virtuoso.log" );
    cfs.setValue( "TransactionFile", dir + "soprano-virtuoso.trx" );
    cfs.setValue( "xa_persistent_file", dir + "soprano-virtuoso.pxa" );
    cfs.endGroup();

    cfs.beginGroup( "TempDatabase" );
    cfs.setValue( "DatabaseFile", dir + "soprano-virtuoso-temp.db" );
    cfs.setValue( "TransactionFile", dir + "soprano-virtuoso-temp.trx" );
    cfs.setValue( "MaxCheckpointRemap", "1000");
    cfs.endGroup();

    cfs.beginGroup( "Parameters" );
    cfs.setValue( "LiteMode", "1" );
    cfs.setValue( "ServerPort", QString::number( m_port ) );
#ifdef Q_OS_WIN
    cfs.setValue( "DisableUnixSocket", "1" );
    cfs.setValue( "DisableTcpSocket", "0" );
#else
    cfs.setValue( "DisableTcpSocket", "1" );
#endif

    // TODO: make this configurable to allow the server to read/write from and to a set of folders
    //    cfs.setValue( "DirsAllowed", "." );

    // FIXME: what is this?
    cfs.setValue( "PrefixResultNames", "0" );

    // Number of thread used in the server (default: 10)
    // FIXME: we have a problem here: soprano server is now multithreaded. Thus, we run out of threads very quickly.
    cfs.setValue( "ServerThreads", numberOfThreads );

    // Memory used by Virtuoso, 8k buffers
    cfs.setValue( "NumberOfBuffers", numberOfBuffers );

    // down from 1200
    cfs.setValue( "MaxDirtyBuffers", maxDirtyBuffers );

    // down from 10
    cfs.setValue( "SchedulerInterval", "5" );

    // down from 10000000
    cfs.setValue( "FreeTextBatchSize", "1000" );

    // checkpoint interval in minutes (Virtuoso default: 60)
    if ( checkpointInterval >= 0 )
        cfs.setValue( "CheckpointInterval", checkpointInterval );
    if ( minAutoCheckpointSize >= 0 )
        cfs.setValue( "MinAutoCheckpointSize", minAutoCheckpointSize );

    cfs.endGroup();

    // normalize all free-text indexes and bif:contains terms (remove accents)
    // Only supported by Virtuoso >= 6.2; older versions will simply ignore it
    cfs.beginGroup( QLatin1String("I18N") );
    cfs.setValue( QLatin1String("XAnyNormalization"), "3" );
    cfs.endGroup();
}


// static
int Soprano::VirtuosoController::pidOfRunningVirtuosoInstance( const QString& storagePath )
{
    // try to aquire lock as Virtuoso itself does
    LockFile lock( storagePath + QLatin1String( "/soprano-virtuoso.db" ) );
    int pid = 0;
    if ( !lock.aquireLock( &pid ) ) {
        return pid;
    }
    else {
        return 0;
    }
}

#include "virtuosocontroller.moc"
