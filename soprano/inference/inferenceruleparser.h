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

#ifndef _INFERENCE_RULE_PARSER_H_
#define _INFERENCE_RULE_PARSER_H_

#include <QtCore/QString>

#include "soprano_export.h"

namespace Soprano {
    namespace Inference {

	class RuleSet;

	/**
	 * \class RuleParser inferenceruleparser.h Soprano/Inference/RuleParser
	 *
	 * \brief The RuleParser can be used to parse %Soprano rule files.
	 *
	 * \sa RuleSet::standardRuleSet()
	 *
	 * \author Sebastian Trueg <trueg@kde.org>
	 */
	class SOPRANO_EXPORT RuleParser
	{
	public:
	    RuleParser();
	    ~RuleParser();

	    bool parseFile( const QString& path );

	    RuleSet rules() const;

	private:
	    class Private;
	    Private* d;
	};
    }
}

#endif
