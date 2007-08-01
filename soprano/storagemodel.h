/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007 Sebastian Trueg <trueg@kde.org>
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

#ifndef _SOPRANO_STORAGE_MODEL_H_
#define _SOPRANO_STORAGE_MODEL_H_

#include "model.h"
#include "soprano_export.h"

namespace Soprano {
    /**
     * Base class for all Models that store data (as compared to FilterModels).
     * The StorageModel simply implements some of the methods from Model which
     * may be equal for different backends.
     */
    class SOPRANO_EXPORT StorageModel : public Model
    {
	Q_OBJECT

    public:
	virtual ~StorageModel();

	/**
	 * Default implementation is based on Model::statementCount
	 */
	virtual bool isEmpty() const;
	
	/**
	 * Default implementation is based on Model::listStatements
	 */
	virtual bool containsStatements( const Statement &statement ) const;

    protected:
	StorageModel();

    private:
	class Private;
	Private* const d;
    };
}

#endif
