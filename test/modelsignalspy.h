/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2006 Sebastian Trueg <strueg@mandriva.com>
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

#ifndef _SOPRANOD_SIGNAL_SPY_H_
#define _SOPRANOD_SIGNAL_SPY_H_

#include <QtCore/QObject>

#include "../soprano/statement.h"


namespace Soprano {
    class Model;

    class ModelSignalSpy : public QObject
    {
	Q_OBJECT

    public:
	ModelSignalSpy( QObject* parent = 0 );
	~ModelSignalSpy();

	void setModel( Model* model );
	
    private Q_SLOTS:
	void slotStatementsAdded();
	void slotStatementsRemoved();
	void slotStatementAdded( const Soprano::Statement& );
	void slotStatementRemoved( const Soprano::Statement& );

    private:
	Model* m_model;
    };
}

#endif
