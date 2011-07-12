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

#ifndef WPROPERTIES_H
#define WPROPERTIES_H

#include <map>
#include <string>
#include <vector>

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread.hpp>

#include "WConditionSet.h"
#include "WSharedSequenceContainer.h"
#include "WPropertyBase.h"
#include "WPropertyTypes.h"
#include "WPropertyVariable.h"
#include "WExportCommon.h"

/**
 * Class to manage properties of an object and to provide convenience methods for easy access and manipulation. It also allows
 * thread safe iteration on its elements. The main purpose of this class is to group properties together and to allow searching properties by a
 * given name. The name of each property in a group has to be unique and is constructed using the group names containing them: hello/you/property
 * is the property with the name "property" in the group "you" which against is in the group "hello".
 * \note The root group of each module does not have a name.
 */
class OWCOMMON_EXPORT WProperties: public WPropertyBase
{
friend class WPropertiesTest;
public:

    // the following typedefs are for convenience.
    typedef boost::shared_ptr< WProperties > SPtr; //!< shared pointer to object of this type
    typedef boost::shared_ptr< const WProperties > ConstSPtr; //!< const shared pointer to object of this type
    typedef WProperties* Ptr; //!< pointer to object of this type
    typedef const WProperties* ConstPtr; //!< const pointer to object of this type
    typedef WProperties& Ref; //!< ref to object of this type
    typedef const WProperties& ConstRef; //!< const ref to object of this type

    /**
     * For shortening: a type defining a shared vector of WSubject pointers.
     */
    typedef std::vector< boost::shared_ptr< WPropertyBase > > PropertyContainerType;

    /**
     * The alias for a shared container.
     */
    typedef WSharedSequenceContainer< PropertyContainerType > PropertySharedContainerType;

    /**
     * The const iterator type of the container.
     */
    typedef PropertyContainerType::const_iterator PropertyConstIterator;

    /**
     * The iterator type of the container.
     */
    typedef PropertyContainerType::iterator PropertyIterator;

    /**
     * Constructor. Creates an empty list of properties.
     *
     * \note WModule::getProperties always returns an unnamed instance.
     *
     * \param name the name of the property group. The GUI is using this name for naming the tabs/group boxes
     * \param description the description of the group.
     */
    WProperties( std::string name = "unnamed group", std::string description = "an unnamed group of properties" );

    /**
     * Copy constructor. Creates a deep copy of this property. As boost::signals2 and condition variables are non-copyable, new instances get
     * created. The subscriptions to a signal are LOST as well as all listeners to a condition.
     * The conditions you can grab using getValueChangeConditon and getCondition are not the same as in the original! This is because
     * the class corresponds to the observer/observable pattern. You won't expect a clone to fire a condition if a original flag is changed
     * (which after cloning is completely decoupled from the clone).
     *
     * \note the properties inside this list are also copied deep
     *
     * \param from the instance to copy.
     */
    explicit WProperties( const WProperties& from );

    /**
     * destructor
     */
    virtual ~WProperties();

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
     * Simply insert the specified property to the list.
     *
     * \param prop the property to add
     */
    void addProperty( boost::shared_ptr< WPropertyBase > prop );

    /**
     * Simply remove the specified property from the list. If the given property is not in the list, nothing happens.
     *
     * \param prop the property to remove.
     */
    void removeProperty( boost::shared_ptr< WPropertyBase > prop );

    /**
     * Helper function that finds a property by its name. Use this method to find out whether the property exists or not, since
     * findProperty throws an exception.
     *
     * \param name name of searched property.
     */
    bool existsProperty( std::string name );

    /**
     * Function searches the property. If it does not exists, it throws an exception.
     *
     * \param name the name of the property
     *
     * \return a WProperty object
     */
    boost::shared_ptr< WPropertyBase > getProperty( std::string name );

    /**
     * Returns a read ticket for read-access to the list of properties.
     *
     * \return the read ticket.
     */
    PropertySharedContainerType::ReadTicket getProperties() const;

    /**
     * Returns an read ticket for the properties. This, and only this, has to be used for external iteration of properties.
     *
     * \see WSharedObjectTicketRead
     * \return the read ticket.
     */
    PropertySharedContainerType::ReadTicket getReadTicket() const;

    /**
     * Searches the property with a given name. It does not throw any exception. It simply returns NULL if it can't be found.
     *
     * \param name the name of the property to search
     *
     * \return the property or NULL if not found.
     */
    boost::shared_ptr< WPropertyBase > findProperty( std::string name ) const;

    /**
     * Removes all properties from the list.
     */
    virtual void clear();

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Convenience methods to create and add properties
    ///////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * Create and add a new property group. Use these groups to structure your properties.
     *
     * \param name the name of the group.
     * \param description the description of the group.
     * \param hide true if group should be completely hidden.
     */
    WPropGroup addPropertyGroup( std::string name, std::string description, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    template< typename T>
    boost::shared_ptr< WPropertyVariable< T > > addProperty( std::string name, std::string description, const T& initial, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param condition use this external condition for notification.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    template< typename T>
    boost::shared_ptr< WPropertyVariable< T > > addProperty( std::string name, std::string description, const T& initial,
                                                             boost::shared_ptr< WCondition > condition, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    template< typename T>
    boost::shared_ptr< WPropertyVariable< T > > addProperty( std::string name, std::string description, const T& initial,
                                                             WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param condition use this external condition for notification
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    template< typename T>
    boost::shared_ptr< WPropertyVariable< T > > addProperty( std::string name, std::string description, const T& initial,
                                                             boost::shared_ptr< WCondition > condition,
                                                             WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );

    /**
     * Gets the real type of this instance. In this case, PV_GROUP.
     *
     * \return the real type.
     */
    virtual PROPERTY_TYPE getType() const;

    /**
     * This methods allows properties to be set by a string value. This method does nothing here, as groups can not be set in any kind.
     *
     * \param value the new value to set. IGNORED.
     *
     * \return always true
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
     * \note for WProperties, this actually does nothing an.
     *
     * \param value the new value.
     *
     * \return true, always.
     */
    virtual bool set( boost::shared_ptr< WPropertyBase > value );

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Convenience methods to create and add properties
    // NOTE: these methods use the type of the initial parameter to automatically use the proper type.
    // This works, since the compiler always calls the function with the best matching parameter types.
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // convenience methods for
    // template< typename T>
    // boost::shared_ptr< WPropertyVariable< T > > addProperty( std::string name, std::string description, const T& initial, bool hide = false );
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropBool      addProperty( std::string name, std::string description, const WPVBaseTypes::PV_BOOL&   initial, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     * It also sets the min and max constraint to [0,100].
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropInt       addProperty( std::string name, std::string description, const WPVBaseTypes::PV_INT&    initial, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     * It also sets the min and max constraint to [0,100].
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropDouble    addProperty( std::string name, std::string description, const WPVBaseTypes::PV_DOUBLE& initial, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropString    addProperty( std::string name, std::string description, const WPVBaseTypes::PV_STRING& initial, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropFilename  addProperty( std::string name, std::string description, const WPVBaseTypes::PV_PATH&   initial, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropSelection      addProperty( std::string name, std::string description, const WPVBaseTypes::PV_SELECTION&   initial, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropPosition      addProperty( std::string name, std::string description, const WPVBaseTypes::PV_POSITION&   initial, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropColor         addProperty( std::string name, std::string description, const WPVBaseTypes::PV_COLOR&  initial, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropTrigger       addProperty( std::string name, std::string description, const WPVBaseTypes::PV_TRIGGER&  initial, bool hide = false );

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // convenience methods for
    // template< typename T>
    // boost::shared_ptr< WPropertyVariable< T > > addProperty( std::string name, std::string description, const T& initial,
    //                                                          boost::shared_ptr< WCondition > condition, bool hide = false );
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param condition use this external condition for notification.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropBool      addProperty( std::string name, std::string description, const WPVBaseTypes::PV_BOOL&   initial,
                                boost::shared_ptr< WCondition > condition, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     * It also sets the min and max constraint to [0,100].
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param condition use this external condition for notification.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropInt       addProperty( std::string name, std::string description, const WPVBaseTypes::PV_INT&    initial,
                                boost::shared_ptr< WCondition > condition, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     * It also sets the min and max constraint to [0,100].
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param condition use this external condition for notification.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropDouble    addProperty( std::string name, std::string description, const WPVBaseTypes::PV_DOUBLE& initial,
                                boost::shared_ptr< WCondition > condition, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param condition use this external condition for notification.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropString    addProperty( std::string name, std::string description, const WPVBaseTypes::PV_STRING& initial,
                                boost::shared_ptr< WCondition > condition, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param condition use this external condition for notification.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropFilename  addProperty( std::string name, std::string description, const WPVBaseTypes::PV_PATH&   initial,
                                boost::shared_ptr< WCondition > condition, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param condition use this external condition for notification.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropSelection addProperty( std::string name, std::string description, const WPVBaseTypes::PV_SELECTION&   initial,
                                boost::shared_ptr< WCondition > condition, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param condition use this external condition for notification.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropPosition  addProperty( std::string name, std::string description, const WPVBaseTypes::PV_POSITION&   initial,
                                boost::shared_ptr< WCondition > condition, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param condition use this external condition for notification.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropColor     addProperty( std::string name, std::string description, const WPVBaseTypes::PV_COLOR&   initial,
                                boost::shared_ptr< WCondition > condition, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param condition use this external condition for notification.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropTrigger   addProperty( std::string name, std::string description, const WPVBaseTypes::PV_TRIGGER&   initial,
                                boost::shared_ptr< WCondition > condition, bool hide = false );

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // convenience methods for
    // template< typename T>
    // boost::shared_ptr< WPropertyVariable< T > > addProperty( std::string name, std::string description, const T& initial,
    //                                                          WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropBool      addProperty( std::string name, std::string description, const WPVBaseTypes::PV_BOOL&   initial,
                                WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     * It also sets the min and max constraint to [0,100].
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropInt       addProperty( std::string name, std::string description, const WPVBaseTypes::PV_INT&    initial,
                                WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     * It also sets the min and max constraint to [0,100].
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropDouble    addProperty( std::string name, std::string description, const WPVBaseTypes::PV_DOUBLE& initial,
                                WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropString    addProperty( std::string name, std::string description, const WPVBaseTypes::PV_STRING& initial,
                                WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropFilename  addProperty( std::string name, std::string description, const WPVBaseTypes::PV_PATH&   initial,
                                WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropSelection addProperty( std::string name, std::string description, const WPVBaseTypes::PV_SELECTION&   initial,
                                WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropPosition  addProperty( std::string name, std::string description, const WPVBaseTypes::PV_POSITION& initial,
                                WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropColor     addProperty( std::string name, std::string description, const WPVBaseTypes::PV_COLOR&  initial,
                                WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropTrigger   addProperty( std::string name, std::string description, const WPVBaseTypes::PV_TRIGGER&  initial,
                                WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // convenience methods for
    // template< typename T>
    // boost::shared_ptr< WPropertyVariable< T > > addProperty( std::string name, std::string description, const T& initial,
    //                                                          boost::shared_ptr< WCondition > condition,
    //                                                          WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param condition use this external condition for notification
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropBool      addProperty( std::string name, std::string description, const WPVBaseTypes::PV_BOOL&   initial,
                                boost::shared_ptr< WCondition > condition,
                                WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     * It also sets the min and max constraint to [0,100].
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param condition use this external condition for notification
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropInt       addProperty( std::string name, std::string description, const WPVBaseTypes::PV_INT&    initial,
                                boost::shared_ptr< WCondition > condition,
                                WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     * It also sets the min and max constraint to [0,100].
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param condition use this external condition for notification
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropDouble    addProperty( std::string name, std::string description, const WPVBaseTypes::PV_DOUBLE& initial,
                                boost::shared_ptr< WCondition > condition,
                                WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );


    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param condition use this external condition for notification
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropString    addProperty( std::string name, std::string description, const WPVBaseTypes::PV_STRING& initial,
                                boost::shared_ptr< WCondition > condition,
                                WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param condition use this external condition for notification
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropFilename  addProperty( std::string name, std::string description, const WPVBaseTypes::PV_PATH&   initial,
                                boost::shared_ptr< WCondition > condition,
                                WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param condition use this external condition for notification
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropSelection addProperty( std::string name, std::string description, const WPVBaseTypes::PV_SELECTION&   initial,
                                boost::shared_ptr< WCondition > condition,
                                WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param condition use this external condition for notification
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropPosition  addProperty( std::string name, std::string description, const WPVBaseTypes::PV_POSITION&   initial,
                                boost::shared_ptr< WCondition > condition,
                                WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param condition use this external condition for notification
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropColor     addProperty( std::string name, std::string description, const WPVBaseTypes::PV_COLOR&   initial,
                                boost::shared_ptr< WCondition > condition,
                                WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param condition use this external condition for notification
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropTrigger   addProperty( std::string name, std::string description, const WPVBaseTypes::PV_TRIGGER&   initial,
                                boost::shared_ptr< WCondition > condition,
                                WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );

    /**
     * This returns the condition fired whenever one children fires its update condition. Useful to get notified about all changes that happen.
     *
     * \return the condition fired if a child fires its update condition.
     */
    virtual boost::shared_ptr< WCondition > getChildUpdateCondition() const;

protected:

   /**
    * Helping function to find a property inside a specific group. It does not recursively find properties nested inside other property groups.
    *
    * \param props the group to search in. This is not a shared pointer since it is not needed. It simply can't happen that it is freed during
    * findProperty as it is contained in this or a nested properties instance.
    * \param name the name of the property inside THIS group.
    *
    * \return the property if found, else NULL.
    */
    boost::shared_ptr< WPropertyBase > findProperty( const WProperties* const props, std::string name ) const;

private:

    /**
     * The set of proerties. This uses the operators ==,<,> WProperty to determine equalness.
     */
    PropertySharedContainerType m_properties;

    /**
     * Condition notified whenever a property inside this group fires its WPropertyBase::m_updateCondition. This is especially useful to get a
     * notification if something updates without further knowledge what changed. Useful if you want to listen for updates in modules for example.
     *
     * \see getChildUpdateCondition
     */
    boost::shared_ptr< WConditionSet > m_childUpdateCondition;

    /**
     * Compares the names of two properties and returns true if they are equal.
     *
     * \param prop1 the first prop.
     * \param prop2 the second prop.
     */
    bool propNamePredicate( boost::shared_ptr< WPropertyBase > prop1, boost::shared_ptr< WPropertyBase > prop2 ) const;
};

template< typename T>
boost::shared_ptr< WPropertyVariable< T > > WProperties::addProperty( std::string name, std::string description, const T& initial, bool hide )
{
    boost::shared_ptr< WPropertyVariable< T > > p = boost::shared_ptr< WPropertyVariable< T > >(
            new WPropertyVariable< T >( name, description, initial )
    );
    p->setHidden( hide );
    addProperty( p );
    return p;
}

template< typename T>
boost::shared_ptr< WPropertyVariable< T > > WProperties::addProperty( std::string name, std::string description, const T& initial,
                                                                       boost::shared_ptr< WCondition > condition, bool hide )
{
    boost::shared_ptr< WPropertyVariable< T > > p = boost::shared_ptr< WPropertyVariable< T > >(
            new WPropertyVariable< T >( name, description, initial, condition )
    );
    p->setHidden( hide );
    addProperty( p );
    return p;
}

template< typename T>
boost::shared_ptr< WPropertyVariable< T > > WProperties::addProperty( std::string name, std::string description, const T& initial,
                                                                       WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    boost::shared_ptr< WPropertyVariable< T > > p = boost::shared_ptr< WPropertyVariable< T > >(
            new WPropertyVariable< T >( name, description, initial, notifier )
    );
    p->setHidden( hide );
    addProperty( p );
    return p;
}

template< typename T>
boost::shared_ptr< WPropertyVariable< T > > WProperties::addProperty( std::string name, std::string description, const T& initial,
                                                                       boost::shared_ptr< WCondition > condition,
                                                                       WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    boost::shared_ptr< WPropertyVariable< T > > p = boost::shared_ptr< WPropertyVariable< T > >(
            new WPropertyVariable< T >( name, description, initial, condition, notifier )
    );
    p->setHidden( hide );
    addProperty( p );
    return p;
}

#endif  // WPROPERTIES_H