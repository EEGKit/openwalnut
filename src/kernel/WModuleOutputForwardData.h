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

#ifndef WMODULEOUTPUTFORWARDDATA_H
#define WMODULEOUTPUTFORWARDDATA_H

#include <iostream>
#include <string>

#include <boost/shared_ptr.hpp>

#include "../common/WLogger.h"

#include "WModuleInputData.h"
#include "WModuleOutputData.h"

/**
 * This is a simple class which forwards output data to output data connectors. It itself is a output data connector and can be used
 * as one, but also provides the possibility to forward data changes to other output data connectors.
 */
template< typename T >
class WModuleOutputForwardData: public WModuleOutputData< T >
{
public:

    /**
     * Constructor. This creates a new output data connector which is able to forward data changes <b>FROM</b> other output data connectors.
     *
     * \param module the module which is owner of this connector.
     * \param name The name of this connector.
     * \param description Short description of this connector.
     */
    WModuleOutputForwardData( boost::shared_ptr<WModule> module, std::string name="", std::string description="" )
        :WModuleOutputData< T >( module, name, description )
    {
        // initialize the output data connector
        m_in = boost::shared_ptr< WModuleInputData< T > >( new WModuleInputData< T >( module, "[FWD]" + name, description ) );

        // subscribe both signals
        m_in->subscribeSignal( CONNECTION_ESTABLISHED, boost::bind( &WModuleOutputForwardData::inputNotifyDataChange, this, _1, _2 ) );
        m_in->subscribeSignal( DATA_CHANGED,           boost::bind( &WModuleOutputForwardData::inputNotifyDataChange, this, _1, _2 ) );
    };

    /**
     * Destructor.
     */
    virtual ~WModuleOutputForwardData()
    {
    }

    /**
     * Forward the output to the specified output. The specified output must be compatible with the template parameter of this output.
     *
     * \param from the output connector whose data should be forwarded.
     */
    virtual void forward( boost::shared_ptr< WModuleConnector > from )
    {
        m_in->connect( from );
    }

    /**
     * Remove the specified connector from the forwarding list.
     *
     * \param from the output connector to be removed from forwarding list.
     */
    virtual void unforward( boost::shared_ptr< WModuleConnector > from )
    {
        m_in->disconnect( from );
    }

protected:

    /**
     * The output connector which collects data and distributes it to all connectors connected using the forwardTo() method.
     */
    boost::shared_ptr< WModuleInputData< T > > m_in;

    /**
     * Gets called whenever a connected output updates its data. In detail: it is a callback for m_in and waits simply forwards
     * new data to this output instance.
     *
     * \param input     the input connector receiving the change (m_in).
     * \param output    the output connector sending the change
     */
    virtual void inputNotifyDataChange( boost::shared_ptr<WModuleConnector> /*input*/, boost::shared_ptr<WModuleConnector> /*output*/ )
    {
        // if the input changes its data-> forward the change to this output instance
        updateData( m_in->getData() );
    }

private:
};

#endif  // WMODULEOUTPUTFORWARDDATA_H

