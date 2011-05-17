//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2009 OpenWalnut Community, BSV@Uni-Leipzig and CNCF@MPI-CBS
// For more information see http://www.openwalnut.org/copying
//
// This file is part of OpenWalnut.
//
// OpenWalnut is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// OpenWalnut is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with OpenWalnut. If not, see <http://www.gnu.org/licenses/>.
//
//---------------------------------------------------------------------------

#ifndef WDATASETVISITOR_H
#define WDATASETVISITOR_H

#include "WValueMapper.h"

/**
 * Visitor base class. Derive your own visitor from this class and specify your own class as this class' template parameter. It uses the CRTP
 * pattern to implement compile-time polymorphism. This class provides additional useful members with getters which might come in handy to the
 * developer. Your visitor-class needs to implement the operator().
 *
 * \note currently, this class is empty but provides some flexibility later on.
 *
 * \tparam VisitorT your visitor class. Do class MyVisitor: public WDataSetVisitor< SomeGridType, SomeStructuralType, MyVisitor >.
 * \tparam GridT the same grid type as the DataSethas.
 * \tparam StructuralT the same structural type as the DataSet has.
 */
template< typename GridT, typename StructuralT, typename VisitorT >
class WDataSetVisitor
{
public:

    /**
     * We need the value-mapper as friend to allow him to set several internal variables which are not allowed to be set by others, especially
     * VisitorT.
     */
    template< typename, typename > friend class WValueMapper;

    /**
     * Grid type.
     */
    typedef GridT GridType;

    /**
     * Structural type.
     */
    typedef StructuralT StructuralType;

    /**
     * The type of the visitor.
     */
    typedef VisitorT VisitorType;

    /**
     * My own type.
     */
    typedef WDataSetVisitor< GridType, StructuralType, VisitorType > DataSetVisitorType;

    /**
     * Operator called by the \ref WStructuralTypeResolution mechanism. Its template parameter is the real type in the value-set. This operator
     * creates pointers to the correct value-set and grid and initializes access-classes and forwards the call to the derived visitor , which
     * then can use them to interact with the data-set.
     *
     * \tparam ValueType the real type stored in the value-set.
     */
    template< typename ValueType >
    void operator()( ValueType /* sample */ )
    {
        // Construct the needed access objects here


        // call VisitorT::operator() with the proper parameters
        VisitorT* v = static_cast< VisitorT* >( this );
        v->operator()( ValueType() );
    }

    /**
     * Operator called by the \ref WStructuralTypeResolution mechanism. Its template parameter is the real type in the value-set. This operator
     * creates pointers to the correct value-set and grid and initializes access-classes and forwards the call to the derived visitor , which
     * then can use them to interact with the data-set.
     *
     * \note this is the const version. As we do not want the
     * \tparam ValueType the real type stored in the value-set.
     */
    template< typename ValueType >
    void operator()( ValueType /* sample */ ) const
    {
        // Construct the needed access objects here


        // call VisitorT::operator() with the proper parameters
        //VisitorT* v = static_cast< VisitorT* >( this );
        //v->operator()( ValueType() );
    }

private:

    typedef WValueMapper< GridType, StructuralType > ValueMapperType;

    ValueMapperType* m_valueMapper;

    void setValueMapper( const ValueMapperType* valuemapper )
    {
        m_valueMapper = valuemapper;
    }
};

#endif  // WDATASETVISITOR_H

