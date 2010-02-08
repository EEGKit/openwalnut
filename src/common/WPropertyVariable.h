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

#ifndef WPROPERTYVARIABLE_H
#define WPROPERTYVARIABLE_H

#include <stdint.h>

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <set>

#include <boost/filesystem.hpp>
#include <boost/thread.hpp>

#include "WFlag.h"
#include "WPropertyBase.h"
#include "WPropertyConstraintTypes.h"
#include "WPropertyConstraintMin.h"
#include "WPropertyConstraintMax.h"
#include "WCondition.h"

/**
 * A named property class with a concrete type.
 */
template< typename T >
class WPropertyVariable: public WFlag< T >,
                         public WPropertyBase
{
public:

    /**
     * Create an empty instance just containing a name.
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     */
    WPropertyVariable( std::string name, std::string description, const T& initial );

    /**
     * Create an empty instance just containing a name. This constructor allows an external condition to be used for notifiaction.
     * This is practical if one would like to share a condition among several properties.
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param condition use this external condition for notification.
     */
    WPropertyVariable( std::string name, std::string description, const T& initial, boost::shared_ptr< WCondition > condition );

    /**
     * Create an empty instance just containing a name. This constructor allows an external callback to be used for notification.
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     *
     * \note: instead of setting another notifier, you should consider using the callbacks the condition offers.
     * \note: the notifiers gets connected to the notification callback of the internal condition. So be careful when using the
     *        condition ( getCondition() ) for other properties, since they would also share the callbacks
     *
     */
    WPropertyVariable( std::string name, std::string description, const T& initial, WCondition::t_ConditionNotifierType notifier );

    /**
     * Create an empty instance just containing a name. This constructor allows an external callback and condition to be used for notification.
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param condition use this external condition for notification
     *
     * \note: instead of setting another notifier, you should consider using the callbacks the condition offers.
     * \note: the notifiers gets connected to the notification callback of the internal condition. So be careful when using the
     *        condition ( getCondition() ) for other properties, since they would also share the callbacks
     *
     */
    WPropertyVariable( std::string name, std::string description, const T& initial, boost::shared_ptr< WCondition > condition,
                       WCondition::t_ConditionNotifierType notifier );

    /**
     * Destructor.
     */
    virtual ~WPropertyVariable();

    /**
     * Determines whether the specified value is acceptable.
     *
     * \param newValue the new value.
     *
     * \return true if it is a valid/acceptable value.
     */
    virtual bool accept( T newValue );

    /**
     * Class building the base for user defined constraints on a property instance.
     */
    class PropertyConstraint
    {
    public:

        /**
         * Default constructor.
         */
        PropertyConstraint();

        /**
         * Destructor.
         */
        virtual ~PropertyConstraint();

        /**
         * This method decides whether the specified value is valid for a specific property.
         *
         * \param value the new value.
         * \param property the property to which the value should be set.
         *
         * \return true whenever the new value is acceptable for the property.
         */
        virtual bool accept( boost::shared_ptr< WPropertyVariable< T > > property, T value ) = 0;

        /**
         * Allows simple identification of the real constraint type.
         *
         * \return the type
         */
        virtual PROPERTYCONSTRAINT_TYPE getType();
    };

    /**
     * Alias for min constraints. It is an alias for convenience.
     */
    typedef boost::shared_ptr< WPropertyConstraintMin< T > > PropertyConstraintMin;

    /**
     * Alias for max constraints. It is an alias for convenience.
     */
    typedef boost::shared_ptr< WPropertyConstraintMax< T > > PropertyConstraintMax;

    /**
     * Add a new constraint. This is useful to disallow several (custom) values for this property.
     *
     * \param constraint the new constraint.
     *
     */
    void addConstraint( boost::shared_ptr< PropertyConstraint > constraint );

    /**
     * Creates a new WPropertyConstraintMin for this WPropertyVariable.
     *
     * \param min the minimum value.
     *
     * \return the new constraint.
     */
    static PropertyConstraintMin minConstraint( T min );

    /**
     * Creates a new WPropertyConstraintMax for this WPropertyVariable.
     *
     * \param max the maximum value of the property
     *
     * \return the new constraint.
     */
    static PropertyConstraintMax maxConstraint( T max );

    /**
     * Set a minimum constraint.
     *
     * \param min the minimum value allowed.
     *
     * \return the newly created constraint.
     */
    PropertyConstraintMin setMin( T min );

    /**
     * Set a maximum constraint.
     *
     * \param max the maximum value allowed.
     *
     * \return the newly created constraint.
     */
    PropertyConstraintMax setMax( T max );

    /**
     * Gets the current minimum constraint value.
     *
     * \return the minimum constraint, or NULL if none.
     */
    PropertyConstraintMin getMin();

    /**
     * Gets the current maximum constraint value.
     *
     * \return the maximum constraint, or NULL if none.
     */
    PropertyConstraintMax getMax();

protected:

    /**
     * The connection used for notification.
     */
    boost::signals2::connection m_notifierConnection;

    /**
     * Uses typeid() to set the proper type constant.
     */
    virtual void updateType();

    /**
     * A set of constraints applied on this property.
     */
    std::set< boost::shared_ptr< PropertyConstraint > > m_constraints;

    /**
     * The iterator used togo through the set
     */
    typedef typename std::set< boost::shared_ptr< PropertyConstraint > >::const_iterator constraintIterator;

    /**
     * boost mutex object for thread safety of updating of m_propertyConstraints.
     */
    boost::shared_mutex m_constraintsLock;

    /**
     * Cleans m_constraints from all existing constrains of the specified type.
     *
     * \param type the type to remove.
     *
     */
    void removeConstraints( PROPERTYCONSTRAINT_TYPE type );

    /**
     * Method searching the first appearance of a constrained with the specified type.
     *
     * \param type the type of the searched constraint
     *
     * \return the constraint, or NULL if none.
     */
    boost::shared_ptr< PropertyConstraint > getFirstConstraint( PROPERTYCONSTRAINT_TYPE type );

private:
};

template < typename T >
WPropertyVariable< T >::WPropertyVariable( std::string name, std::string description, const T& initial ):
        WFlag< T >( new WCondition(), initial ),
        WPropertyBase( name, description )
{
    updateType();
}

template < typename T >
WPropertyVariable< T >::WPropertyVariable( std::string name, std::string description, const T& initial, boost::shared_ptr< WCondition > condition ):
        WFlag< T >( condition, initial ),
        WPropertyBase( name, description )
{
    updateType();
}

template < typename T >
WPropertyVariable< T >::WPropertyVariable( std::string name, std::string description, const T& initial,
                                           WCondition::t_ConditionNotifierType notifier ):
        WFlag< T >( new WCondition(), initial ),
        WPropertyBase( name, description )
{
    updateType();

    // set custom notifier
    m_notifierConnection = WFlag< T >::getCondition()->subscribeSignal( notifier );
}

template < typename T >
WPropertyVariable< T >::WPropertyVariable( std::string name, std::string description, const T& initial, boost::shared_ptr< WCondition > condition,
                                           WCondition::t_ConditionNotifierType notifier ):
        WFlag< T >( condition, initial ),
        WPropertyBase( name, description )
{
    updateType();
    // set custom notifier
    m_notifierConnection = WFlag< T >::getCondition()->subscribeSignal( notifier );
}

template < typename T >
WPropertyVariable< T >::~WPropertyVariable()
{
    // clean up
    m_notifierConnection.disconnect();
    m_constraints.clear();
}

template < typename T >
bool WPropertyVariable< T >::accept( T newValue )
{
    boost::shared_lock< boost::shared_mutex > lock = boost::shared_lock< boost::shared_mutex >( m_constraintsLock );

    // iterate through the set
    bool acceptable = WFlag< T >::accept( newValue );
    for ( constraintIterator it = m_constraints.begin(); it != m_constraints.end(); ++it )
    {
        acceptable &= ( *it )->accept( boost::shared_static_cast< WPropertyVariable< T > >( shared_from_this() ), newValue );
    }
    lock.unlock();

    return acceptable;
}

template < typename T >
void WPropertyVariable< T >::addConstraint( boost::shared_ptr< PropertyConstraint > constraint )
{
    boost::unique_lock< boost::shared_mutex > lock = boost::unique_lock< boost::shared_mutex >( m_constraintsLock );
    m_constraints.insert( constraint );
    lock.unlock();
}

template < typename T >
void WPropertyVariable< T >::updateType()
{
    PROPERTY_TYPE_HELPER::WTypeIdentifier< T > tid;
    m_type = tid.getType();
}

template < typename T >
boost::shared_ptr< WPropertyConstraintMin< T > > WPropertyVariable< T >::minConstraint( T min )
{
    return boost::shared_ptr< WPropertyConstraintMin< T > >( new WPropertyConstraintMin< T >( min ) );
}

template < typename T >
boost::shared_ptr< WPropertyConstraintMax< T > > WPropertyVariable< T >::maxConstraint( T max )
{
    return boost::shared_ptr< WPropertyConstraintMax< T > >( new WPropertyConstraintMax< T >( max ) );
}

template < typename T >
boost::shared_ptr< WPropertyConstraintMin< T > > WPropertyVariable< T >::setMin( T min )
{
    removeConstraints( PC_MIN );
    boost::shared_ptr< WPropertyConstraintMin< T > > c = minConstraint( min );
    boost::unique_lock< boost::shared_mutex > lock = boost::unique_lock< boost::shared_mutex >( m_constraintsLock );
    m_constraints.insert( c );
    lock.unlock();
    return c;
}

template < typename T >
boost::shared_ptr< WPropertyConstraintMax< T > > WPropertyVariable< T >::setMax( T max )
{
    removeConstraints( PC_MAX );
    boost::shared_ptr< WPropertyConstraintMax< T > > c = maxConstraint( max );
    boost::unique_lock< boost::shared_mutex > lock = boost::unique_lock< boost::shared_mutex >( m_constraintsLock );
    m_constraints.insert( c );
    lock.unlock();
    return c;
}

template < typename T >
boost::shared_ptr< typename WPropertyVariable< T >::PropertyConstraint >
WPropertyVariable< T >::getFirstConstraint( PROPERTYCONSTRAINT_TYPE type )
{
    // lock
    boost::shared_lock< boost::shared_mutex > lock = boost::shared_lock< boost::shared_mutex >( m_constraintsLock );

    // search first appearance of a constraint of the specified type
    for ( constraintIterator it = m_constraints.begin(); it != m_constraints.end(); ++it )
    {
        if ( ( *it )->getType() == type )
        {
            return ( *it );
        }
    }
    lock.unlock();

    return boost::shared_ptr< PropertyConstraint >();
}

template < typename T >
boost::shared_ptr< WPropertyConstraintMin< T > > WPropertyVariable< T >::getMin()
{
    // get min
    boost::shared_ptr< PropertyConstraint > c = getFirstConstraint( PC_MIN );
    if ( !c.get() )
    {
        // return NULL if not found
        return boost::shared_ptr< WPropertyConstraintMin< T > >();
    }

    // cast to proper type
    return boost::shared_static_cast< WPropertyConstraintMin< T > >( c );
}

template < typename T >
boost::shared_ptr< WPropertyConstraintMax< T > > WPropertyVariable< T >::getMax()
{
    // get min
    boost::shared_ptr< PropertyConstraint > c = getFirstConstraint( PC_MAX );
    if ( !c.get() )
    {
        // return NULL if not found
        return boost::shared_ptr< WPropertyConstraintMax< T > >();
    }

    // cast to proper type
    return boost::shared_static_cast< WPropertyConstraintMax< T > >( c );
}

template < typename T >
void WPropertyVariable< T >::removeConstraints( PROPERTYCONSTRAINT_TYPE type )
{
    boost::unique_lock< boost::shared_mutex > lock = boost::unique_lock< boost::shared_mutex >( m_constraintsLock );
    for ( constraintIterator it = m_constraints.begin(); it != m_constraints.end(); ++it )
    {
        if ( ( *it )->getType() == type )
        {
            m_constraints.erase( it );
        }
    }
    lock.unlock();
}

template < typename T >
WPropertyVariable< T >::PropertyConstraint::PropertyConstraint()
{
}

template < typename T >
WPropertyVariable< T >::PropertyConstraint::~PropertyConstraint()
{
}

template < typename T >
PROPERTYCONSTRAINT_TYPE WPropertyVariable< T >::PropertyConstraint::getType()
{
    return PC_UNKNOWN;
}

#endif  // WPROPERTYVARIABLE_H

