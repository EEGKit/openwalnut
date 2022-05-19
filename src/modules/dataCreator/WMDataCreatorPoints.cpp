//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2022 OpenWalnut Community, BSV@Uni-Leipzig and CNCF@MPI-CBS
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

#include <algorithm>
#include <memory>
#include <string>
#include <thread>
#include <chrono>

#include "WDataCreatorTorus.h"
#include "WMDataCreatorPoints.h"
#include "WDataCreatorPointsRandom.h"
#include "core/common/WAssert.h"
#include "core/common/WProgress.h"
#include "core/common/WStrategyHelper.h"
#include "core/dataHandler/WDataSetFibers.h"
#include "core/kernel/WKernel.h"

WMDataCreatorPoints::WMDataCreatorPoints():
    WModule(),
    m_strategy( "Dataset Creators", "Select one of the dataset creators and configure it to your needs.", NULL,
                "Creator", "A list of all known creators." )
{
    // add some strategies here
    m_strategy.addStrategy( WDataCreatorPointsRandom::SPtr( new WDataCreatorPointsRandom() ) );
    // m_strategy.addStrategy( WDataCreatorFiberRandom::SPtr( new WDataCreatorFiberRandom() ) );
    // m_strategy.addStrategy( WDataCreatorFiberSpiral::SPtr( new WDataCreatorFiberSpiral() ) );
    // m_strategy.addStrategy( WDataCreatorFiberStar::SPtr( new WDataCreatorFiberStar() ) );
    m_strategy.addStrategy( WDataCreatorPointsTorus::SPtr( new WDataCreatorPointsTorus() ) );
}

WMDataCreatorPoints::~WMDataCreatorPoints()
{
    // cleanup
    removeConnectors();
}

std::shared_ptr< WModule > WMDataCreatorPoints::factory() const
{
    return std::shared_ptr< WModule >( new WMDataCreatorPoints() );
}

const std::string WMDataCreatorPoints::getName() const
{
    return "Data Creator Points";
}

const std::string WMDataCreatorPoints::getDescription() const
{
    return ""; // See META file
}

void WMDataCreatorPoints::connectors()
{
    // initialize connectors
    m_output = WModuleOutputData< WDataSetPoints >::createAndAdd( shared_from_this(), "out", "The data that has been created" );

    // call WModule's initialization
    WModule::connectors();
}

void WMDataCreatorPoints::properties()
{
    m_propCondition = std::shared_ptr< WCondition >( new WCondition() );

    // grid transform information
    m_origin = m_properties->addProperty( "Origin", "Coordinate of the origin (voxel 0,0,0).", WPosition( 0.0, 0.0, 0.0 ), m_propCondition );
    m_size = m_properties->addProperty( "Size", "The size of the dataset along the X,Y, and Z axis in the OpenWalnut coordinate system.",
                                        WPosition( 128.0, 128.0, 128.0 ), m_propCondition );

    m_seed = m_properties->addProperty( "Seed", "The seed for the random numbers to create.", 0, m_propCondition );

    m_numPoints = m_properties->addProperty( "Num Points", "The number of points to create.", 500, m_propCondition );
    m_numPoints->setMin( 1 );
    m_numPoints->setMax( 1000000 );

    m_timeDependent = m_properties->addProperty( "Time dependent (experimental)",
                                                 "Vary data over time. This feature is <b>experimental</b>.",
                                                 false,
                                                 m_propCondition );

    m_pointColor = m_properties->addProperty( "Color", "Color for the points.", defaultColor::WHITE, m_propCondition );

    // now, setup the strategy helper.
    m_properties->addProperty( m_strategy.getProperties() );

    // call WModule's initialization
    WModule::properties();
}

void WMDataCreatorPoints::moduleMain()
{
    // let the main loop awake if the data changes or the properties changed.
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_propCondition );
    // we need to wake up if some strategy prop changed
    m_moduleState.add( m_strategy.getProperties()->getUpdateCondition() );

    // signal ready state
    ready();

    // loop until the module container requests the module to quit
    while( !m_shutdownFlag() )
    {
        debugLog() << "Creating dataset";

        WProgress::SPtr progress( new WProgress( "Creating Dataset", m_numPoints->get() ) );
        m_progress->addSubProgress( progress );

        // build structures for keeping the data
        size_t numPoints = m_numPoints->get();

        WDataSetPoints::VertexArray vertices = WDataSetPoints::VertexArray( new WDataSetPoints::VertexArray::element_type() );
        WDataSetPoints::ColorArray colors = WDataSetPoints::ColorArray( new WDataSetPoints::ColorArray::element_type() );
        vertices->reserve( numPoints * 3 );
        colors->reserve( numPoints * 3 );

        // get the current strategy
        m_strategy()->operator()( m_seed->get(),
                                            progress,
                                            m_pointColor->get(),
                                            numPoints,
                                            m_origin->get(),
                                            m_size->get(),
                                            vertices, colors );

        // build the dataset
        WDataSetPoints::SPtr ds = WDataSetPoints::SPtr( new WDataSetPoints( vertices, colors ) );

        // done. Notify user.
        debugLog() << "Created dataset with " << m_numPoints->get() << " points.";
        progress->finish();
        m_progress->removeSubProgress( progress );

        // done. update output
        m_output->updateData( ds );

        // Now, the moduleState variable comes into play. The module can wait for the condition, which gets fired whenever the input receives data
        // or an property changes. The main loop now waits until something happens.
        debugLog() << "Waiting ...";

        if( m_timeDependent->get() )
        {
            m_size->set( WVector3d( m_size->get()[0], m_size->get()[1], static_cast<int>( m_size->get()[2] + 10 ) % 1000 ) );
            std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
        }

        m_moduleState.wait();
    }
}
