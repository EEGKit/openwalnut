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

template< typename > class WValueSet;
template< typename, typename > class WDataSetAccessor;
template< typename, typename > class WDataSetAccessorConst;

template< typename ValueMapperT, typename VisitorT >
class WDataSetVisitor
{
public:
    /**
     * Grid type.
     */
    typedef typename ValueMapperT::GridType GridType;

    /**
     * Structural type.
     */
    typedef typename ValueMapperT::StructuralType StructuralType;

    /**
     * The type of the visitor.
     */
    typedef VisitorT VisitorType;

    /**
     * The value-mapper to which this class can be applied.
     */
    typedef ValueMapperT ValueMapperType;

    /**
     * My own type.
     */
    typedef WDataSetVisitor< ValueMapperType, VisitorType > DataSetVisitorType;

    /**
     * Construct new visitor, dispatching the visit call to the specified visitor. The WDataSetVisitor does not take ownership of the specified
     * pointers and does not free them upon destruction.
     *
     * \param visitor the visitor to call
     * \param valuemapper the valuemapper to which this dispatcher is applied.
     */
    WDataSetVisitor( VisitorType* visitor, ValueMapperType* valuemapper ):
        m_visitor( visitor ),
        m_valueMapper( valuemapper )
    {
        // initialize
    }

    /**
     * Operator called by the \ref WStructuralTypeResolution mechanism. Its template parameter is the real type in the value-set. This operator
     * creates pointers to the correct value-set and grid and initializes access-classes and forwards the call to the visitor, which
     * then can use them to interact with the data-set.
     *
     * \tparam ValueType the real type stored in the value-set.
     */
    template< typename ValueType >
    void operator()( ValueType /* sample */ )
    {
        // Construct the needed access objects here
        // TODO(all): use some access object? some super duper iterator=
        typedef WDataSetAccessor< GridType, ValueType > AccessorType;

        // call VisitorT::operator() with the proper parameters
        m_visitor->operator()( m_valueMapper->template createAccessor< AccessorType >() );
    }

    /**
     * Operator called by the \ref WStructuralTypeResolution mechanism. Its template parameter is the real type in the value-set. This operator
     * creates pointers to the correct value-set and grid and initializes access-classes and forwards the call to the visitor, which
     * then can use them to interact with the data-set.
     *
     * \note this is the const version. As we do not want the
     * \tparam ValueType the real type stored in the value-set.
     */
    template< typename ValueType >
    void operator()( ValueType /* sample */ ) const
    {
        // Construct the needed access objects here
        // TODO(all): use some access object? some super duper iterator=

        // NOTE: we never call the operator() const of VisitorT here. VisitorT is non const -> m_visitor is always non const (only the pointer is
        // const in this operator). This allows the programmer to write its own nice visitor which has always the rights to write its own members
        // even if called for a const dataset. We transport the const information by providing only a const reference to the dataset (access
        // object) to the operator.

        // to aware constness, use a special access object here
        typedef WDataSetAccessorConst< GridType, ValueType > AccessorType;
        // call VisitorT::operator() with the proper parameters
        m_visitor->operator()( m_valueMapper->template createAccessor< AccessorType >() );
    }

private:

    /**
     * The visitor instance.
     */
    VisitorType* m_visitor;

    /**
     * The instance of the actual value mapper.
     */
    ValueMapperType* m_valueMapper;
};

#endif  // WDATASETVISITOR_H

