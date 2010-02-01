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

#ifndef WMODULEINPUTFORWARDDATA_H
#define WMODULEINPUTFORWARDDATA_H

#include <boost/shared_ptr.hpp>

#include "../common/WLogger.h"

#include "WModuleInputData.h"
#include "WModuleOutputData.h"

/**
 * This is a simple class which forwards input data to input data connectors. It itself is a input data connector and can be used
 * as one, but also provides the possibility to forward data changes to other input data connectors, using a separate output data
 * connector (which is not visible to the outside world).
 */
template< typename T >
class WModuleInputForwardConnector: public WModuleInputData< T >
{
public:

    /**
     * Constructor. This creates a new input data connector which is able to forward data changes to other input data connectors.
     *
     * \param module the module which is owner of this connector.
     * \param name The name of this connector.
     * \param description Short description of this connector.
     */
    WModuleInputForwardConnector( boost::shared_ptr<WModule> module, std::string name="", std::string description="" )
        :WModuleInputData< T >( module, name, description )
    {
        // initialize the output data connector
        m_out = boost::shared_ptr< WModuleOutputData< T > >( new WModuleOutputData< T >( module, "[FWD]" + name, description ) );
    };

    /**
     * Destructor.
     */
    virtual ~WModuleInputForwardConnector()
    {
    }

    /**
     * Forward the input to the specified input. The specified input must be compatible with the template parameter of this input.
     *
     * \param to the input connector to forward data to.
     */
    virtual void forward( boost::shared_ptr< WModuleConnector > to )
    {
        m_out->connect( to );
    }

    /**
     * Remove the specified connector from the forwarding list.
     *
     * \param to the input connector to be removed from forwarding list.
     */
    virtual void unforward( boost::shared_ptr< WModuleConnector > to )
    {
        m_out->disconnect( to );
    }

protected:

    /**
     * The output connector which collects data and distributes it to all connectors connected using the forwardTo() method.
     */
    boost::shared_ptr< WModuleOutputData< T > > m_out;

    /**
     * Gets called whenever a connected output updates its data. This method uses this callback to update the m_out connector to
     * inform all inputs to which the data should be forwarded.
     *
     * \param input     the input connector receiving the change
     * \param output    the output connector sending the change
     */
    void notifyDataChange( boost::shared_ptr<WModuleConnector> input, boost::shared_ptr<WModuleConnector> output )
    {
        m_out->updateData( WModuleInputData< T >::getData() );

        // simply forward the call
        WModuleInputData< T >::notifyDataChange( input, output );
    }

    /**
     * Gets called whenever a connector gets connected to the specified input.
     *
     * \param here the connector of THIS module that got connected to "there"
     * \param there the connector that has been connected with the connector "here" of this module.
     */
    virtual void notifyConnectionEstablished( boost::shared_ptr<WModuleConnector> here, boost::shared_ptr<WModuleConnector> there )
    {
        // a connection -> grab the data and forward it to the output connector
        m_out->updateData( WModuleInputData< T >::getData() );

        // and forward
        WModuleInputData< T >::notifyDataChange( here, there );
    }

private:
};

#endif  // WMODULEINPUTFORWARDDATA_H

