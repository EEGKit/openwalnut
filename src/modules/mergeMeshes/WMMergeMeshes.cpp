//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2022 OpenWalnut Community and Hochschule Worms
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

#include "WMMergeMeshes.h"
#include "core/common/WPropertyHelper.h"
#include "core/common/math/WMath.h"
#include "core/dataHandler/WDataHandler.h"
#include "core/kernel/WKernel.h"

// This line is needed by the module loader to actually find your module. You need to add this to your module too. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMMergeMeshes )

WMMergeMeshes::WMMergeMeshes():
    WModule()
{
}

WMMergeMeshes::~WMMergeMeshes()
{
    // Cleanup!
}

std::shared_ptr< WModule > WMMergeMeshes::factory() const
{
    return std::shared_ptr< WModule >( new WMMergeMeshes() );
}

const char** WMMergeMeshes::getXPMIcon() const
{
    return NULL;
}

const std::string WMMergeMeshes::getName() const
{
    return "Merge Meshes";
}

const std::string WMMergeMeshes::getDescription() const
{
    return "This module merges several given mesh datasets into one new dataset";
}

void WMMergeMeshes::connectors()
{
    m_meshInput1 = WModuleInputData< WTriangleMesh >::createAndAdd( shared_from_this(), "in1", "The first mesh" );
    m_meshInput2 = WModuleInputData< WTriangleMesh >::createAndAdd( shared_from_this(), "in2", "The second mesh" );
    m_meshOutput = WModuleOutputData< WTriangleMesh >::createAndAdd( shared_from_this(), "out", "The merged mesh" );

    // call WModule's initialization
    WModule::connectors();
}

void WMMergeMeshes::properties()
{
    m_propCondition = std::shared_ptr< WCondition >( new WCondition() );

    // call WModule's initialization
    WModule::properties();
}

void WMMergeMeshes::moduleMain()
{
    // get notified about data changes
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_meshInput1->getDataChangedCondition() );
    m_moduleState.add( m_meshInput2->getDataChangedCondition() );
    // Remember the condition provided to some properties in properties()? The condition can now be used with this condition set.
    m_moduleState.add( m_propCondition );

    ready();

    // main loop
    while( !m_shutdownFlag() )
    {
        debugLog() << "Waiting ...";
        m_moduleState.wait();

        // woke up since the module is requested to finish?
        if( m_shutdownFlag() )
        {
            break;
        }

        // To query whether an input was updated, simply ask the input:
        bool dataUpdated = m_meshInput1->handledUpdate() || m_meshInput2->handledUpdate();
        std::shared_ptr< WTriangleMesh > dataSet1 = m_meshInput1->getData();
        std::shared_ptr< WTriangleMesh > dataSet2 = m_meshInput2->getData();
        bool dataValid = ( dataSet1 && dataSet2 );

        // reset everything if input was disconnected/invalid
        if( !dataValid )
        {
            debugLog() << "Resetting output.";
            m_meshOutput->reset();
            continue;
        }

        if( dataValid && dataUpdated )
        {
            WProgress::SPtr progress = WProgress::SPtr( new WProgress( "Merging meshes." ) );
            m_progress->addSubProgress( progress );

            std::shared_ptr< WTriangleMesh > merged = std::make_shared< WTriangleMesh >( 0, 0 );
            merged->addMesh( dataSet1 );
            merged->addMesh( dataSet2 );

            // // directly merge bounding boxes
            // WBoundingBox bb = dataSet1->getBoundingBox();
            // bb.expandBy( dataSet2->getBoundingBox() );

            m_meshOutput->updateData( merged );
            progress->finish();
            m_progress->removeSubProgress( progress );
        }
    }
}
