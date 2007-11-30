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

#ifndef _SOPRANO_NRL_MODEL_H_
#define _SOPRANO_NRL_MODEL_H_

#include "filtermodel.h"
#include "soprano_export.h"

namespace Soprano {
    /**
     * \class NRLModel nrlmodel.h Soprano/NRLModel
     *
     * \brief Model filter that enforces NRL cardinality rules.
     *
     * The NRLModel enforces NRL cardinality rules. That means predicates with a cardinality
     * of maximum 1 are always udpated while statements that define predicates with a maximum
     * cardinality bigger than 1 are rejected once the maximum is reached (future versions 
     * might remove an earlier defined statement based on the time the old statements were
     * added).
     *
     * Thus, at the moment NRLModel is mostly usable for handling properties with a maximum
     * cardinality of 1.
     *
     * \author Sebastian Trueg <trueg@kde.org>
     *
     * \sa Vocabulary::NRL
     */
    class SOPRANO_EXPORT NRLModel : public FilterModel
    {
    public:
    NRLModel();
    NRLModel( Model* parent );
    ~NRLModel();

    /**
     * When enforcing the NRL cardinality rules NRLModel can either
     * ignore the context of statements or treat different contexts
     * as separate sets, each resetting the cardinality.
     *
     * \param b If \p true (the default) NRLModel does ignore the context
     * when enforcing rules. If \p false the NRL rules can be violated
     * across contexts.
     *
     * \sa ignoreContext()
     */
    void setIgnoreContext( bool b );

    /**
     * \return \p true if contexts should be ignored when enforcing NRL
     * rules.
     *
     * \sa setIgnoreContext()
     */
    bool ignoreContext() const;

       /**
    * Add a statement.
    *
    * \param s The statement containing the property to be set.
    * If the predicate has NRL cardinality restrictions existing
    * statements will be updated. Otherwise this method has the
    * same effect as Model::addStatement().
    *
    * Adding a statement that defines a predicate with a maximum
    * cardinality bigger than 1 which has already been reached 
    * fails with an error.
    *
    * \return Error::ErrorNone on success.
    */
    Error::ErrorCode addStatement( const Statement& s );

    private:
    class Private;
    Private* const d;
    };
}

#endif
