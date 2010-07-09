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

#ifndef _SOPRANO_RDF_SCHEMA_MODEL_H_
#define _SOPRANO_RDF_SCHEMA_MODEL_H_

#include "filtermodel.h"
#include "node.h"
#include "soprano_export.h"

namespace Soprano {

    class Model;
    class StatementIterator;

    /**
     * \class RdfSchemaModel rdfschemamodel.h Soprano/RdfSchemaModel
     *
     * \brief Provides convenience methods to handle RDFS data.
     *
     * Interface based on Sesame's RdfSchemaSource. (Copyright (C) 2002-2006 Aduna BV, GNU LGPL License applies.)
     *
     * \warning Backward inferencing has not been implemented yet. Thus, methods like subClassOf() return the same
     * as directSubClassOf().
     *
     * \author Sebastian Trueg <trueg@kde.org>
     */
    class SOPRANO_EXPORT RdfSchemaModel : public FilterModel
    {
    public:
        /**
         * Create a new RDF Schema wrapper model that performs actions
         * on \p model
         */
        RdfSchemaModel( Model* parentModel = 0 );

        /**
         * Destructor.
         */
        ~RdfSchemaModel();

        /**
         * Get all defined RDF/S classes.
         * \return a StatementIterator that iterates over all statements in the model
         * defining an RDF/S class.
         */
        StatementIterator classes() const;

        /**
         * Gets all direct subClassOf relations with a specific sub- and/or superclass. 
         * A class A is a direct subclass of class B if there is no class C such that A is 
         * a subclass of C and C is a subclass of B.
         *
         * \param subClass The subclass of the relations that should be returned, 
         * or an empty node if relations with any subclass should be returned.
         * \param superClass The superclass of the relations that should be returned, 
         * or an empty node if relations with any superclass should be returned.
         *
         * \return A StatementIterator containing statements of the form
         * (subClass, rdfs:subClassOf, superClass).
         */
        StatementIterator directSubClassOf( const Node& subClass, const Node& superClass = Node() ) const;

        /**
         * Gets all direct subPropertyOf relations with a specific sub- and/or superproperty.
         * A property A is a direct subproperty of property B if there is no property C such
         * that A is a subproperty of C and C is a subproperty of B. 
         *
         * \param subProperty The subproperty of the relations that should be returned,
         * or an empty node if relations with any subproperty should be returned.
         * \param superProperty The superproperty of the relations that should be returned,
         * or an empty node if relations with any superproperty should be returned.
         *
         * \return A StatementIterator containing statements of the form
         * (subProperty, rdfs:subPropertyOf, superProperty).
         */
        StatementIterator directSubPropertyOf( const Node& subProperty, const Node& superProperty = Node() ) const;

        /**
         * Gets all direct type relations with a specific instance and/or class.
         *
         * \param someClass The instance of the relations that should be returned,
         * or an empty node if relations with any instance should be returned.
         * \param someType The class of the relations that should be returned,
         * or an empty node if relations with any class should be returned.
         *
         * \return A StatementIterator containing statements of the form
         * (someClass, rdf:type, someType).
         */
        StatementIterator directType( const Node& someClass, const Node& someType ) const;

        /**
         * Gets all domain relations with a specific property and/or domain class.
         *
         * \param prop The property of the relations that should be returned,
         * or an empty node if relations with any property should be returned.
         * \param domain The domain of the relations that should be returned,
         * or an empty node if relations with any domain should be returned.
         *
         * \return A StatementIterator containing statements of the form
         * (prop, rdfs:domain, domain).
         */
        StatementIterator domain( const Node& prop, const Node& domain ) const;

        /**
         * Gets all defined properties. 
         *
         * \return A StatementIterator containing statements of the form
         * (someProperty, rdf:type, rdf:Property).
         */
        StatementIterator properties() const;    

        /**
         * Gets all range relations with a specific property and/or range class.
         *
         * \param prop The property of the relations that should be returned,
         * or an empty node if relations with any property should be returned.
         * \param range The range of the relations that should be returned,
         * or an empty node if relations with any range should be returned.
         *
         * \return A StatementIterator containing statements of the form
         * (prop, rdfs:range, range).
         */
        StatementIterator range( const Node& prop, const Node& range ) const;

        /**
         * Gets all subClassOf relations with a specific sub- and/or superclass.
         * Note that the subClassOf relation is reflexive: a class is implicitly
         * always a subclass of itself.
         *
         * Do not use this method if the Soprano backend supports inferencing.
         * Use directSubClassOf instead which is much faster.
         *
         * \param subClass The subclass of the relations that should be returned,
         * or an empty node if relations with any subclass should be returned.
         * \param superClass The superclass of the relations that should be returned,
         * or an emtpy node if relations with any superclass should be returned.
         *
         * \return A StatementIterator containing all valid statements of the form
         * (subClass, rdfs:subClassOf, superClass) including those calculated through
         * direct inferencing.
         */
        StatementIterator subClassOf( const Node& subClass, const Node& superClass = Node() ) const;

        /**
         * Gets all subPropertyOf relations with a specific sub- and/or superproperty.
         * Note that the subPropertyOf relation is reflexive: a property is implicitly
         * always a subproperty of itself.
         *
         * Do not use this method if the Soprano backend supports inferencing.
         * Use directSubPropertyOf instead which is much faster.
         *
         * \param subProperty The subproperty of the relations that should be returned,
         * or an empty node if relations with any subproperty should be returned.
         * \param superProperty The superproperty of the relations that should be returned,
         * or an empty node if relations with any superproperty should be returned.
         *
         * \return A StatementIterator containing all valid statements of the form
         * (subProperty, rdfs:subPropertyOf, superProperty) including those calculated through
         * direct inferencing.
         */
        StatementIterator subPropertyOf( const Node& subProperty, const Node& superProperty = Node() ) const;
    
        /**
         * Gets all type relations with a specific instance and/or class.
         *
         * Do not use this method if the Soprano backend supports inferencing.
         * Use directType instead which is much faster.
         *
         * \param someClass The instance of the relations that should be returned,
         * or an empty node if relations with any instance should be returned.
         * \param someType The class of the relations that should be returned,
         * or an empty node if relations with any class should be returned.
         *
         * \return A StatementIterator containing all valid statements of the form
         * (someClass, rdf:type, someType) including those calculated through
         * direct inferencing.
         */
        StatementIterator type( const Node& someClass, const Node& someType ) const;

        /**
         * Checks whether the supplied resource represents a class.
         *
         * \param resource The resource to check.
         *
         * \return true if resource is a class, false otherwise.
         */
        bool isClass( const Node& resource ) const;

        /**
         * Checks whether the supplied resource represents a property.
         *
         * \param resource The resource to check.
         *
         * \return true if resource is a property, false otherwise.
         */
        bool isProperty( const Node& resource ) const;

        /**
         * Checks whether one resource is a direct subclass of another.
         *
         * \param subClass A class.
         * \param superClass A class.
         *
         * \return true if subClass is a direct subclass of superClass, false otherwise.
         */
        bool isDirectSubClassOf( const Node& subClass, const Node& superClass ) const;

        /**
         * Checks whether one resource is a direct subproperty of another.
         *
         * \param subProperty A property.
         * \param superProperty A property.
         *
         * \return true if subProperty is a direct subproperty of superProperty, false otherwise.
         */
        bool isDirectSubPropertyOf( const Node& subProperty, const Node& superProperty ) const;

        /**
         * Checks whether one resource is a direct instance of another.
         *
         * \param someClass An instance.
         * \param someType A class.
         *
         * \return true if someClass is a direct instance of someType, false otherwise.
         */
        bool isDirectType( const Node& someClass, const Node& someType ) const;

        /**
         * Checks whether one resource is a subclass of another.
         *
         * Do not use this method if the Soprano backend supports inferencing.
         * Use isDirectSubClassOf instead which is much faster.
         *
         * \param subClass A class.
         * \param superClass A class.
         *
         * \return true if subClass is a subclass of superClass, false otherwise.
         */
        bool isSubClassOf( const Node& subClass, const Node& superClass ) const;

        /**
         * Checks whether one resource is a subproperty of another.
         *
         * Do not use this method if the Soprano backend supports inferencing.
         * Use isDirectSubPropertyOf instead which is much faster.
         *
         * \param subProperty A property.
         * \param superProperty A property.
         *
         * \return true if subProperty is a subproperty of superProperty, false otherwise.
         */
        bool isSubPropertyOf( const Node& subProperty, const Node& superProperty ) const;

        /**
         * Checks whether one resource is a instance of another.
         *
         * Do not use this method if the Soprano backend supports inferencing.
         * Use isDirectType instead which is much faster.
         *
         * \param someClass An instance.
         * \param someType A class.
         *
         * \return true if someClass is an instance of someType, false otherwise.
         */
        bool isType( const Node& someClass, const Node& someType ) const;

    private:
        class Private;
        Private* const d;
    };
}

#endif


