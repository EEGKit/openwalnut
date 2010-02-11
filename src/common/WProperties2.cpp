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

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "WLogger.h"
#include "exceptions/WPropertyUnknown.h"

#include "WProperties2.h"

WProperties2::WProperties2():
    m_propAccess( m_properties.getAccessObject() )
{
}

WProperties2::~WProperties2()
{
}

void WProperties2::addProperty( boost::shared_ptr< WPropertyBase > prop )
{
    m_propAccess->beginWrite();
    m_propAccess->get().push_back( prop );
    m_propAccess->endWrite();
}

boost::shared_ptr< WPropertyBase > WProperties2::findProperty( std::string name )
{
    boost::shared_ptr< WPropertyBase > result = boost::shared_ptr< WPropertyBase >();

    m_propAccess->beginRead();

    // iterate over the items
    for ( PropertyContainerType::const_iterator it = m_propAccess->get().begin(); it != m_propAccess->get().end(); ++it )
    {
        if ( ( *it )->getName() == name )
        {
            result = ( *it );
            break;
        }
    }
    m_propAccess->endRead();

    return result;
}

bool WProperties2::existsProperty( std::string name )
{
    return ( findProperty( name ) != boost::shared_ptr< WPropertyBase >() );
}

boost::shared_ptr< WPropertyBase > WProperties2::getProperty( std::string name )
{
    boost::shared_ptr< WPropertyBase > p = findProperty( name );
    if ( p == boost::shared_ptr< WPropertyBase >() )
    {
        throw WPropertyUnknown( "Property \"" + name + "\" can't be found." );
    }

    return p;
}

WProperties2::PropertySharedContainerType::WSharedAccess WProperties2::getAccessObject()
{
    return m_properties.getAccessObject();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// convenience methods for
// template< typename T>
// boost::shared_ptr< WPropertyVariable< T > > addProperty( std::string name, std::string description, const T& initial, bool hide = false );
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WPropBool WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_BOOL&   initial, bool hide )
{
    return addProperty< WPVBaseTypes::PV_BOOL >( name, description, initial, hide );
}

WPropInt WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_INT&    initial, bool hide )
{
    WPropInt p = addProperty< WPVBaseTypes::PV_INT >( name, description, initial, hide );
    p->setMin( 0 );
    p->setMax( 100 );
    return p;
}

WPropDouble WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_DOUBLE& initial, bool hide )
{
    WPropDouble p = addProperty< WPVBaseTypes::PV_DOUBLE >( name, description, initial, hide );
    p->setMin( 0.0 );
    p->setMax( 100.0 );
    return p;
}

WPropString WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_STRING& initial, bool hide )
{
    return addProperty< WPVBaseTypes::PV_STRING >( name, description, initial, hide );
}

WPropFilename WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_PATH&   initial, bool hide )
{
    WPropFilename p = addProperty< WPVBaseTypes::PV_PATH >( name, description, initial, hide );
    p->addConstraint( WPVFilename::PropertyConstraint::create( PC_NOTEMPTY ) );
    return p;
}

WPropList WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_LIST&   initial, bool hide )
{
    return addProperty< WPVBaseTypes::PV_LIST >( name, description, initial, hide );
}

WPropPosition WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_POSITION&   initial, bool hide )
{
    return addProperty< WPVBaseTypes::PV_POSITION >( name, description, initial, hide );
}

WPropColor WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_COLOR&   initial, bool hide )
{
    return addProperty< WPVBaseTypes::PV_COLOR >( name, description, initial, hide );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// convenience methods for
// template< typename T>
// boost::shared_ptr< WPropertyVariable< T > > addProperty( std::string name, std::string description, const T& initial,
//                                                          boost::shared_ptr< WCondition > condition, bool hide = false );
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WPropBool WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_BOOL&   initial,
                                     boost::shared_ptr< WCondition > condition, bool hide )
{
    return addProperty< WPVBaseTypes::PV_BOOL >( name, description, initial, condition, hide );
}

WPropInt WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_INT&    initial,
                                    boost::shared_ptr< WCondition > condition, bool hide )
{
    WPropInt p = addProperty< WPVBaseTypes::PV_INT >( name, description, initial, condition, hide );
    p->setMin( 0 );
    p->setMax( 100 );
    return p;
}

WPropDouble WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_DOUBLE& initial,
                                       boost::shared_ptr< WCondition > condition, bool hide )
{
    WPropDouble p = addProperty< WPVBaseTypes::PV_DOUBLE >( name, description, initial, condition, hide );
    p->setMin( 0.0 );
    p->setMax( 100.0 );
    return p;
}

WPropString WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_STRING& initial,
                                       boost::shared_ptr< WCondition > condition, bool hide )
{
    return addProperty< WPVBaseTypes::PV_STRING >( name, description, initial, condition, hide );
}

WPropFilename WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_PATH&   initial,
                                         boost::shared_ptr< WCondition > condition, bool hide )
{
    WPropFilename p = addProperty< WPVBaseTypes::PV_PATH >( name, description, initial, condition, hide );
    p->addConstraint( WPVFilename::PropertyConstraint::create( PC_NOTEMPTY ) );
    return p;
}

WPropList WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_LIST&   initial,
                                     boost::shared_ptr< WCondition > condition, bool hide )
{
    return addProperty< WPVBaseTypes::PV_LIST >( name, description, initial, condition, hide );
}

WPropPosition WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_POSITION&   initial,
                                         boost::shared_ptr< WCondition > condition, bool hide )
{
    return addProperty< WPVBaseTypes::PV_POSITION >( name, description, initial, condition, hide );
}

WPropColor WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_COLOR&   initial,
                                     boost::shared_ptr< WCondition > condition, bool hide )
{
    return addProperty< WPVBaseTypes::PV_COLOR >( name, description, initial, condition, hide );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// convenience methods for
// template< typename T>
// boost::shared_ptr< WPropertyVariable< T > > addProperty( std::string name, std::string description, const T& initial,
//                                                          WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WPropBool WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_BOOL&   initial,
                                     WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    return addProperty< WPVBaseTypes::PV_BOOL >( name, description, initial, notifier, hide );
}

WPropInt WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_INT&    initial,
                                    WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    WPropInt p = addProperty< WPVBaseTypes::PV_INT >( name, description, initial, notifier, hide );
    p->setMin( 0 );
    p->setMax( 100 );
    return p;
}

WPropDouble WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_DOUBLE& initial,
                                       WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    WPropDouble p = addProperty< WPVBaseTypes::PV_DOUBLE >( name, description, initial, notifier, hide );
    p->setMin( 0.0 );
    p->setMax( 100.0 );
    return p;
}

WPropString WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_STRING& initial,
                                       WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    return addProperty< WPVBaseTypes::PV_STRING >( name, description, initial, notifier, hide );
}

WPropFilename WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_PATH&   initial,
                                         WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    WPropFilename p = addProperty< WPVBaseTypes::PV_PATH >( name, description, initial, notifier, hide );
    p->addConstraint( WPVFilename::PropertyConstraint::create( PC_NOTEMPTY ) );
    return p;
}

WPropList WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_LIST&   initial,
                                     WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    return addProperty< WPVBaseTypes::PV_LIST >( name, description, initial, notifier, hide );
}

WPropPosition WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_POSITION&   initial,
                                         WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    return addProperty< WPVBaseTypes::PV_POSITION >( name, description, initial, notifier, hide );
}

WPropColor WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_COLOR&   initial,
                                     WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    return addProperty< WPVBaseTypes::PV_COLOR >( name, description, initial, notifier, hide );
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// convenience methods for
// template< typename T>
// boost::shared_ptr< WPropertyVariable< T > > addProperty( std::string name, std::string description, const T& initial,
//                                                          boost::shared_ptr< WCondition > condition,
//                                                          WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WPropBool WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_BOOL&   initial,
                                     boost::shared_ptr< WCondition > condition,
                                     WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    return addProperty< WPVBaseTypes::PV_BOOL >( name, description, initial, condition, notifier, hide );
}

WPropInt WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_INT&    initial,
                                     boost::shared_ptr< WCondition > condition,
                                     WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    WPropInt p = addProperty< WPVBaseTypes::PV_INT >( name, description, initial, condition, notifier, hide );
    p->setMin( 0 );
    p->setMax( 100 );
    return p;
}

WPropDouble WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_DOUBLE& initial,
                                     boost::shared_ptr< WCondition > condition,
                                     WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    WPropDouble p = addProperty< WPVBaseTypes::PV_DOUBLE >( name, description, initial, condition, notifier, hide );
    p->setMin( 0.0 );
    p->setMax( 100.0 );
    return p;
}

WPropString WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_STRING& initial,
                                     boost::shared_ptr< WCondition > condition,
                                     WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    return addProperty< WPVBaseTypes::PV_STRING >( name, description, initial, condition, notifier, hide );
}

WPropFilename WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_PATH&   initial,
                                     boost::shared_ptr< WCondition > condition,
                                     WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    WPropFilename p = addProperty< WPVBaseTypes::PV_PATH >( name, description, initial, condition, notifier, hide );
    p->addConstraint( WPVFilename::PropertyConstraint::create( PC_NOTEMPTY ) );
    return p;
}

WPropList WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_LIST&   initial,
                                     boost::shared_ptr< WCondition > condition,
                                     WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    return addProperty< WPVBaseTypes::PV_LIST >( name, description, initial, condition, notifier, hide );
}

WPropPosition WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_POSITION&   initial,
                                         boost::shared_ptr< WCondition > condition,
                                         WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    return addProperty< WPVBaseTypes::PV_POSITION >( name, description, initial, condition, notifier, hide );
}

WPropColor WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_COLOR&   initial,
                                     boost::shared_ptr< WCondition > condition,
                                     WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    return addProperty< WPVBaseTypes::PV_COLOR >( name, description, initial, condition, notifier, hide );
}

