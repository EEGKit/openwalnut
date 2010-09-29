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

#include <vector>
#include <string>

#include "../../kernel/WKernel.h"
#include "../../common/WPropertyHelper.h"
#include "../../dataHandler/WDataHandler.h"
#include "../../dataHandler/WDataTexture3D.h"

#include "WMDerive.h"
#include "WMDerive.xpm"

// This line is needed by the module loader to actually find your module. You need to add this to your module too. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMDerive )

WMDerive::WMDerive():
    WModule()
{
}

WMDerive::~WMDerive()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMDerive::factory() const
{
    return boost::shared_ptr< WModule >( new WMDerive() );
}

const char** WMDerive::getXPMIcon() const
{
    return WMDerive_xpm;
}

const std::string WMDerive::getName() const
{
    return "Spatial Derivation";
}

const std::string WMDerive::getDescription() const
{
    return "This module a scalar field and derives it spatially.";
}

void WMDerive::connectors()
{
    // the dataset to process. Only accept scalar data.

   // vector input
    m_scalarIn = WModuleInputData< WDataSetScalar >::createAndAdd( shared_from_this(), "scalars", "The scalar dataset."
                                                                                                    "Needs to be in the same grid as the mesh." );

    m_vectorOut = WModuleOutputData< WDataSetVector >::createAndAdd( shared_from_this(), "derived", "The vector dataset representing spatial"
                                                                                                    " derivate of the input field." );

    // call WModule's initialization
    WModule::connectors();
}

void WMDerive::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    // normalizing?
    m_normalize = m_properties->addProperty( "Normalize", "If true, vectors get normalized.", true );

    // call WModule's initialization
    WModule::properties();
}

void WMDerive::moduleMain()
{
    // get notified about data changes
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_scalarIn->getDataChangedCondition() );
    // Remember the condition provided to some properties in properties()? The condition can now be used with this condition set.
    m_moduleState.add( m_propCondition );

    ready();

    // main loop
    while ( !m_shutdownFlag() )
    {
        debugLog() << "Waiting ...";
        m_moduleState.wait();

        // woke up since the module is requested to finish?
        if ( m_shutdownFlag() )
        {
            break;
        }

        // To query whether an input was updated, simply ask the input:
        bool dataUpdated = m_scalarIn->handledUpdate() || m_normalize->changed();
        boost::shared_ptr< WDataSetScalar > dataSet = m_scalarIn->getData();

        bool dataValid = ( dataSet );
        if ( !dataValid || ( dataValid && !dataUpdated ) )
        {
            continue;
        }

        // prepare progress indicators
        boost::shared_ptr< WProgress > progress = boost::shared_ptr< WProgress >( new WProgress( "Processing", 0 ) );
        m_progress->addSubProgress( progress );

        // loop through each voxel
        boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( dataSet->getGrid() );
        if ( !grid )
        {
            errorLog() << "Only regular 3D grids allowed.";
            continue;
        }

        switch( dataSet->getValueSet()->getDataType() )
        {
            case W_DT_UNSIGNED_CHAR:
            {
                boost::shared_ptr<WValueSet<unsigned char> > vals;
                vals = boost::shared_dynamic_cast<WValueSet<unsigned char> >( dataSet->getValueSet() );
                WAssert( vals, "Data type and data type indicator must fit." );
                derive( grid, vals );
                break;
            }
            case W_DT_INT16:
            {
                boost::shared_ptr<WValueSet<int16_t> > vals;
                vals = boost::shared_dynamic_cast<WValueSet<int16_t> >( dataSet->getValueSet() );
                WAssert( vals, "Data type and data type indicator must fit." );
                derive( grid, vals );
                break;
            }
            case W_DT_SIGNED_INT:
            {
                boost::shared_ptr<WValueSet<int32_t> > vals;
                vals = boost::shared_dynamic_cast<WValueSet<int32_t> >( dataSet->getValueSet() );
                WAssert( vals, "Data type and data type indicator must fit." );
                derive( grid, vals );
                break;
            }
            case W_DT_FLOAT:
            {
                boost::shared_ptr< WValueSet< float > > vals;
                vals = boost::shared_dynamic_cast< WValueSet< float > >( dataSet->getValueSet() );
                WAssert( vals, "Data type and data type indicator must fit." );
                derive( grid, vals );
                break;
            }
            case W_DT_DOUBLE:
            {
                boost::shared_ptr< WValueSet< double > > vals;
                vals = boost::shared_dynamic_cast< WValueSet< double > >( dataSet->getValueSet() );
                WAssert( vals, "Data type and data type indicator must fit." );
                derive( grid, vals );
                break;
            }
            default:
                WAssert( false, "Unknown data type in." );
        }

        // work done
        progress->finish();
        m_progress->removeSubProgress( progress );

        debugLog() << "Done";
    }
}

size_t getId( size_t xDim, size_t yDim, size_t /*zDim*/, size_t x, size_t y, size_t z, size_t offset = 0, size_t elements = 1 )
{
    return offset + ( elements * ( z * xDim * yDim + y * xDim + x ) );
}

template< typename T >
void WMDerive::derive( boost::shared_ptr< WGridRegular3D > grid, boost::shared_ptr< WValueSet< T > > values )
{
    size_t nX = grid->getNbCoordsX();
    size_t nY = grid->getNbCoordsY();
    size_t nZ = grid->getNbCoordsZ();

    std::vector< double > vectors( 3 * nX * nY * nZ, 0.0 );

    // iterate field
    for( size_t z = 1; z < nZ - 1; z++ )
    {
        for( size_t y = 1; y < nY - 1; y++ )
        {
            for( size_t x = 1; x < nX - 1; x++ )
            {
                // TODO(ebaum): improve performance
                // this loop should be quite slow but it works for now. Sorry.
                float xp = values->getScalar( getId( nX, nY, nZ, x + 1, y, z ) );
                float xm = values->getScalar( getId( nX, nY, nZ, x - 1, y, z ) );
                float yp = values->getScalar( getId( nX, nY, nZ, x, y + 1, z ) );
                float ym = values->getScalar( getId( nX, nY, nZ, x, y - 1, z ) );
                float zp = values->getScalar( getId( nX, nY, nZ, x, y, z + 1 ) );
                float zm = values->getScalar( getId( nX, nY, nZ, x, y, z - 1 ) );

                float vx = xp - xm;
                float vy = yp - ym;
                float vz = zp - zm;

                float sqsum = vx * vx + vy * vy + vz * vz;
                float len = sqrt( sqsum );
                float scal = m_normalize->get( true ) ? 1.0 / len : 1.0;
                if ( len == 0.0 )
                    scal = 0.0;

                vectors[ getId( nX, nY, nZ, x, y, z, 0, 3 ) ] = scal * vx;
                vectors[ getId( nX, nY, nZ, x, y, z, 1, 3 ) ] = scal * vy;
                vectors[ getId( nX, nY, nZ, x, y, z, 2, 3 ) ] = scal * vz;
            }
        }
    }

    // de-register at datahandler
    if ( m_lastOutputDataSet )
    {
        WDataHandler::deregisterDataSet( m_lastOutputDataSet );
    }

    boost::shared_ptr< WValueSet< double > > valueset = boost::shared_ptr< WValueSet< double > >( new WValueSet< double >( 1, 3, vectors, W_DT_DOUBLE ) );
    m_lastOutputDataSet = boost::shared_ptr< WDataSetVector >( new WDataSetVector( valueset, grid ) );

    // register new
    WDataHandler::registerDataSet( m_lastOutputDataSet );
    m_vectorOut->updateData( m_lastOutputDataSet );
}

void WMDerive::activate()
{
    // deactivate the output if wanted
    if ( m_lastOutputDataSet )
    {
        m_lastOutputDataSet->getTexture()->setGloballyActive( m_active->get( true ) );
    }

    // Always call WModule's activate!
    WModule::activate();
}

