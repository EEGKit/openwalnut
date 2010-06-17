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

#ifndef WMODULECONTAINER_H
#define WMODULECONTAINER_H

#include <list>
#include <set>
#include <map>
#include <vector>
#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/signals2/signal.hpp>
#include <boost/function.hpp>

#include "../common/WSharedObject.h"

#include "WModuleCombinerTypes.h"
#include "WModuleSignals.h"

class WThreadedRunner;
class WBatchLoader;
class WModule;
class WMData;

/**
 * Class able to contain other modules. It manages several tasks like finding appropriate modules, managing data modules and
 * module initialization.
 *
 * \ingroup Kernel
 */
class WModuleContainer: public WModule
{
public:

    // the following typedefs are for convenience; to help accessing the container in a thread safe way.

    /**
     * For shortening: a type defining a shared vector of WModule pointers.
     */
    typedef std::set< boost::shared_ptr< WModule > > ModuleContainerType;

    /**
     * The alias for a shared container.
     */
    typedef WSharedObject< ModuleContainerType > ModuleSharedContainerType;

    /**
     * The access type
     */
    typedef ModuleSharedContainerType::WSharedAccess ModuleAccessType;

    /**
     * The const iterator type of the container.
     */
    typedef ModuleContainerType::const_iterator ModuleConstIterator;

    /**
     * The iterator type of the container.
     */
    typedef ModuleContainerType::iterator ModuleIterator;


    /**
     * Constructor. Initializes container.
     *
     * \param name  name of the container
     * \param description short description.
     */
    WModuleContainer( std::string name = "Unnamed Module Container",
                      std::string description = "Used as container for several modules." );

    /**
     * Destructor.
     */
    virtual ~WModuleContainer();

    /**
     * Add a module to this container and start it. Please note, that a module can be added only once. If it already is
     * associated with this container nothing happens.
     *
     * \param module the module to add.
     * \param run true when the module should be run automatically after adding it.
     * \throw WModuleUninitialized thrown whenever someone wants to add a module not yet initialized.
     */
    virtual void add( boost::shared_ptr< WModule > module, bool run = true );

    /**
     * Remove the given module from this container if it is associated with it. It only provides flat removal. It does not remove depending
     * modules. Please be aware that this method does NOT stop the module. It just removes it from the container. If you release the shared
     * pointer after removing from the container, the instance gets freed although it still might run. To also wait for the module to quit, use
     * module->wait( true ).
     *
     * \param module the module to remove.
     */
    virtual void remove( boost::shared_ptr< WModule > module );

    /**
     * As \ref remove, it removes the module from the container. In contrast to \ref remove, it also removes all the depending modules from the
     * container.
     *
     * \param module the module which should be removed including all depending modules
     */
    virtual void removeDeep( boost::shared_ptr< WModule > module );

    /**
     * Stops all modules inside this container. Note that this function could take some time, since it waits until the last module
     * has quit.
     */
    virtual void stop();

    /**
     * Gives back the name of this module.
     * \return the module's name.
     */
    virtual const std::string getName() const;

    /**
     * Gives back a description of this module.
     * \return description to module.
     */
    virtual const std::string getDescription() const;

    /**
     * Add a specified notifier to the list of default notifiers which get connected to each added module.
     *
     * \param signal    the signal the notifier should get connected to
     * \param notifier  the notifier function
     */
    virtual void addDefaultNotifier( MODULE_SIGNAL signal, t_ModuleErrorSignalHandlerType notifier );

    /**
     * Add a specified notifier to the list of default notifiers which get connected to each added module.
     *
     * \param signal    the signal the notifier should get connected to
     * \param notifier  the notifier function
     */
    virtual void addDefaultNotifier( MODULE_SIGNAL signal, t_ModuleGenericSignalHandlerType notifier );

    /**
     * Function combines two modules. This runs synchronously. It might take some time to finish since combination of modules is
     * allowed only with modules marked as "ready" which might take some time.
     *
     * \param applyOn the module which already has to be in the container and to apply the other one on.
     * \param what the prototype name of the module to apply on the other one specified.
     * \param tryOnly If set to false and the prototype "what" does not exist this will throw an exception. If set to true and the prototype does
     *                not exist, the nothing will happen.
     *
     * \return the newly created module connected with the one specified in applyOn. If the prototype could not be found and tryOnly was set to
     *         true it will return NULL.
     */
    virtual boost::shared_ptr< WModule > applyModule( boost::shared_ptr< WModule > applyOn, std::string what, bool tryOnly = false );

    /**
     * Function combines two modules. This runs synchronously. It might take some time to finish since combination of modules is
     * allowed only with modules marked as "ready" which might take some time.
     *
     * \param applyOn the module which already has to be in the container and to apply the other one on.
     * \param prototype the prototype of the module to apply on the other one specified.
     *
     * \return the newly created module connected with the one specified in applyOn.
     */
    virtual boost::shared_ptr< WModule > applyModule( boost::shared_ptr< WModule > applyOn, boost::shared_ptr< WModule > prototype );

    /**
     * Load specified datasets. It immediately returns and starts another thread, which actually loads the data.
     *
     * \param fileNames list of filenames to load. The registered notification handler for the root container will get notified on
     * error and success.
     */
    boost::shared_ptr< WBatchLoader > loadDataSets( std::vector< std::string > fileNames );

    /**
     * Loads the specified files synchronously.
     *
     * \param fileNames list of filenames to load. The registered notification handler for the root container will get notified on
     * error and success.
     */
    void loadDataSetsSynchronously( std::vector< std::string > fileNames );

    /**
     * Add the specified thread to the list of pending jobs. Only this ensures, that ALL pending threads get stopped before the
     * container gets stopped.
     *
     * \note use this to register threads whenever you start threads belonging to this container. This avoids shutting down the
     * container while other threads depend upon it.
     *
     * \param thread the thread to add
     */
    void addPendingThread( boost::shared_ptr< WThreadedRunner > thread );

    /**
     * The specified thread has finished and does not longer depend upon this container instance.
     *
     * \param thread the thread.
     */
    void finishedPendingThread( boost::shared_ptr< WThreadedRunner > thread );

    /**
     * Sets a flag denoting whether the container (which also is a module) should be marked as "crashed" if a nested module crashes.
     *
     * \param crashIfCrashed true if it also should crash.
     */
    void setCrashIfModuleCrashes( bool crashIfCrashed = true );

    /**
     * Due to the prototype design pattern used to build modules, this method returns a new instance of this method. NOTE: it
     * should never be initialized or modified in some other way. A simple new instance is required.
     *
     * \return the prototype used to create every module in OpenWalnut.
     */
    virtual boost::shared_ptr< WModule > factory() const;

    /**
     * Simple type for WMData pointer lists.
     */
    typedef std::set< boost::shared_ptr< WMData > > DataModuleListType;

    /**
     * Returns a vector of pointers to the loaded data modules in the container.
     *
     * \return the list of data modules.
     */
    DataModuleListType getDataModules();

    /**
     * Method returns a read ticket allowing read-access to the list of modules.
     * \note If done, ensure the ticket gets destroyed.
     *
     * \return the read ticket.
     */
    ModuleSharedContainerType::ReadTicket getModules() const;

    /**
     * This method creates a list of combiner instances, for each possible connection that can be made between the specified module and the
     * module currently inside the container. It might be possible that a module which is contained in the returned list is not associated
     * anymore if the combiner gets applied.
     *
     * \param module the module to which the possible connections should be returned
     *
     * \return the possible combinations of connectors.
     */
     WCompatiblesList getPossibleConnections( boost::shared_ptr< WModule > module );

protected:

    /**
     * Entry point after loading the module. Runs in separate thread. The module container does not use this method. It simply
     * returns.
     */
    virtual void moduleMain();

    /**
     * The modules associated with this container.
     */
    ModuleSharedContainerType m_modules;

    /**
     * Name of the module.
     */
    std::string m_name;

    /**
     * Description of the module.
     */
    std::string m_description;

    /**
     * Lock for error notifiers set.
     */
    boost::shared_mutex m_errorNotifiersLock;

    /**
     * The error notifiers connected to added modules by default.
     */
    std::list< t_ModuleErrorSignalHandlerType > m_errorNotifiers;

    /**
     * Lock for ready notifiers set.
     */
    boost::shared_mutex m_readyNotifiersLock;

    /**
     * The ready notifiers connected to added modules by default.
     */
    std::list< t_ModuleGenericSignalHandlerType > m_readyNotifiers;

    /**
     * Lock for associated notifiers set.
     */
    boost::shared_mutex m_associatedNotifiersLock;

    /**
     * The notifiers connected to added modules by default and fired whenever the module got associated.
     */
    std::list< t_ModuleGenericSignalHandlerType > m_associatedNotifiers;

    /**
     * Lock for remove-notifiers set.
     */
    boost::shared_mutex m_removedNotifiersLock;

    /**
     * The notifiers connected to added modules by default and fired whenever the module got removed again.
     */
    std::list< t_ModuleGenericSignalHandlerType > m_removedNotifiers;

    /**
     * Set of all threads that currently depend upon this container.
     */
    std::set< boost::shared_ptr< WThreadedRunner > > m_pendingThreads;

    /**
     * Lock for m_pendingThreads.
     */
    boost::shared_mutex m_pendingThreadsLock;

    /**
     * This method is called whenever a module inside the container crashes. By default, this method does nothing but forwarding the using
     * WModule's signals.
     *
     * \param module the module that has crashed.
     * \param exception the exception.
     */
    virtual void moduleError( boost::shared_ptr< WModule > module, const WException& exception );

    /**
     * This flag denotes whether the whole container should be marked as crashed if one of the contained modules crashes. By default, this is
     * true. The root container (the container not nested in any other container) sets this to false explicitly. Modules using the container to
     * encapsulate a whole bunch of modules can decide, but by default they crash too.
     */
    bool m_crashIfModuleCrashes;

private:

    // the following typedefs are for convenience; to help accessing the container in a thread safe way.

    /**
     * A type for mapping a module to all its subscriptions
     */
    typedef std::pair< boost::shared_ptr< WModule >, boost::signals2::connection > ModuleSubscription;

    /**
     * For shortening: a type defining a shared vector of subscriptions a module made to a notifier during add().
     */
    typedef std::multimap< boost::shared_ptr< WModule >, boost::signals2::connection > ModuleSubscriptionsType;

    /**
     * The alias for a shared container.
     */
    typedef WSharedObject< ModuleSubscriptionsType > ModuleSubscriptionsSharedType;

    /**
     * The const iterator type of the container.
     */
    typedef ModuleSubscriptionsType::const_iterator ModuleSubscriptionsConstIterator;

    /**
     * The iterator type of the container.
     */
    typedef ModuleSubscriptionsType::iterator ModuleSubscriptionsIterator;

    /**
     * The module's signal subscriptions.
     */
    ModuleSubscriptionsSharedType m_moduleSubscriptions;
};

#endif  // WMODULECONTAINER_H

