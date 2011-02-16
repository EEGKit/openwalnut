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

#ifndef WPROPERTYOBJECT_H
#define WPROPERTYOBJECT_H

#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>

#include "WPropertyBase.h"
#include "WPropertyTypes.h"

#include "WExportCommon.h"

/**
 * This class implements the idea of WPropertyVariable but with a tremendous difference. In WPropertyVariable, the decision whether the value has
 * changed or not is up to the WPropertyVariable. So setting a new value to the variable means a change. But what if the embedded variable is an
 * object, which can change its state internally? This object does not have any possibility to notify the WPropertyVariable that a change has
 * occured. WPropertyObject allows embedding an whole Class type and allowing it to determine when a change occured.
 *
 * To fulfill the requirements of WPropertyObject, your class needs to provide the following concepts
 * <ul>
 *  <li> Serializable:
 *  <ul>
 *      <li> bool setAsString( std::string value ) which uses a string to set the object's internal state. Usually, this will be/use your operator<<
 *      <li> std::string getAsString() which serializes the object state to a string. Usually, this will be/use your operator>>
 *  </ul>
 *  <li> Copyable:
 *  <ul>
 *      <li> bool set( const ObjectType& value ) which sets the object state according to the given one. Usually, this will be/use your operator=
 *      <li> a copy constructor
 *      <li> a default constructor
 *  </ul>
 *  <li> boost::shared_ptr< WCondition > getUpdateCondition() const which returns a condition that gets fired whenever your object changes its
 *      state. The condition must not change during object life-time!
 * </ul>
 *
 * \tparam ObjectType the object to handle
 */
template< class ObjectType >
class OWCOMMON_EXPORT WPropertyObject: public WPropertyBase
{
public:

    /**
     * Convenience typedef for a boost::shared_ptr< WPropertyObject >.
     */
    typedef boost::shared_ptr< WPropertyObject > SPtr;

    /**
     * Convenience typedef for a boost::shared_ptr< const WPropertyObject >.
     */
    typedef boost::shared_ptr< const WPropertyObject > ConstSPtr;

    /**
     * Create an empty named property.
     *
     * \param name  the name of the property
     * \param description the description of the property
     */
    WPropertyObject( std::string name, std::string description );

    /**
     * Copy constructor. Creates a deep copy of this property. As boost::signals2 and condition variables are non-copyable, new instances get
     * created. The subscriptions to a signal are LOST as well as all listeners to a condition.
     *
     * \param from the instance to copy.
     */
    explicit WPropertyObject( const WPropertyObject& from );

    /**
     * Destructor.
     */
    virtual ~WPropertyObject();

    /**
     * This method clones a property and returns the clone. It does a deep copy and, in contrast to a copy constructor, creates property with the
     * correct type without explicitly requiring the user to specify it. It creates a NEW change condition and change signal. This means, alls
     * subscribed signal handlers are NOT copied.
     *
     * \note this simply ensures the copy constructor of the runtime type is issued.
     *
     * \return the deep clone of this property.
     */
    virtual boost::shared_ptr< WPropertyBase > clone();

    /**
     * This methods allows properties to be set by a string value. This is especially useful when a property is only available as string and the
     * real type of the property is unknown. This is a shortcut for casting the property and then setting the lexically casted value.
     *
     * \param value the new value to set.
     *
     * \return true if value could be set.
     */
    virtual bool setAsString( std::string value );

    /**
     * Returns the current value as a string. This is useful for debugging or project files. It is not implemented as << operator, since the <<
     * should also print min/max constraints and so on. This simply is the value.
     *
     * \return the value as a string.
     */
    virtual std::string getAsString();

    /**
     * Sets the value from the specified property to this one. This is especially useful to copy a value without explicitly casting/knowing the
     * dynamic type of the property.
     *
     * \param value the new value.
     *
     * \return true if the value has been accepted.
     */
    virtual bool set( boost::shared_ptr< WPropertyBase > value );

    /**
     * Returns the contained object.
     *
     * \note the returned ref is not const. You can modify it (use its non-const members) and this property will fire automatically.
     *
     * \return the object
     */
    virtual ObjectType& get() const;

protected:

private:

    /**
     * The object handled in this property.
     */
    ObjectType& m_object;
};

template < class ObjectType >
WPropertyObject< ObjectType >::WPropertyObject( std::string name, std::string description ):
    WPropertyBase( name, description ),
    m_object( ObjectType() )
{
    // initialize
    // we need to add the update condition of the new object.
    m_updateCondition->add( m_object.getUpdateCondition() );
}

template < class ObjectType >
WPropertyObject< ObjectType >::WPropertyObject( const WPropertyObject& from ):
    WPropertyBase( from ),
    m_object( ObjectType( from.m_object ) )
{
    // copy
    // we need to add the update condition of the new object.
    m_updateCondition->add( m_object.getUpdateCondition() );
}

template < class ObjectType >
WPropertyObject< ObjectType >::~WPropertyObject()
{
    // clean up
    m_updateCondition->remove( m_object.getUpdateCondition() );
}

template < class ObjectType >
boost::shared_ptr< WPropertyBase > WPropertyObject< ObjectType >::clone()
{
    // this simply uses the copy constructor
    return boost::shared_ptr< WPropertyBase >( new WPropertyObject< ObjectType >( *this ) );
}

template < class ObjectType >
bool WPropertyObject< ObjectType >::setAsString( std::string value )
{
    return m_object.setAsString( value );
}

template < class ObjectType >
std::string WPropertyObject< ObjectType >::getAsString()
{
    return m_object.getAsString();
}

template < class ObjectType >
bool WPropertyObject< ObjectType >::set( boost::shared_ptr< WPropertyBase > value )
{
    boost::shared_ptr< WPropertyObject< ObjectType > > p = boost::shared_dynamic_cast< WPropertyObject< ObjectType > >( value );
    if ( !p )
    {
        return false;
    }

    return m_object.set( p->get() );
}

template < class ObjectType >
ObjectType& WPropertyObject< ObjectType >::get() const
{
    return m_object;
}

#endif  // WPROPERTYOBJECT_H

