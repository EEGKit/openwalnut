//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2015 OpenWalnut Community, Nemtics, BSV@Uni-Leipzig
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

#include <memory>
#include <string>
#include <vector>

#include "WMResampleRegular.h"
#include "core/common/WProgress.h"
#include "core/dataHandler/WGridTransformOrtho.h"
#include "core/kernel/WKernel.h"

// This line is needed by the module loader to actually find your module. Do not remove. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMResampleRegular )

WMResampleRegular::WMResampleRegular():
    WModule()
{
}

WMResampleRegular::~WMResampleRegular()
{
    // Cleanup!
}

std::shared_ptr< WModule > WMResampleRegular::factory() const
{
    // See "src/modules/template/" for an extensively documented example.
    return std::shared_ptr< WModule >( new WMResampleRegular() );
}

const std::string WMResampleRegular::getName() const
{
    return "Resample Regular";
}

const std::string WMResampleRegular::getDescription() const
{
    return "A module to resample a regular dataset with a different regular grid.";
}

void WMResampleRegular::connectors()
{
    m_original = std::shared_ptr<WModuleInputData<WDataSetScalar> >(
        new WModuleInputData<WDataSetScalar> ( shared_from_this(), "Original",
                                               "The dataset to resample." ) );
    addConnector( m_original );

    m_target = std::shared_ptr<WModuleInputData<WDataSetScalar> >(
        new WModuleInputData<WDataSetScalar> ( shared_from_this(), "Target",
                                               "The original dataset is resamples to the grid of this dataset." ) );
    addConnector( m_target );

    m_resampled = std::shared_ptr<WModuleOutputData<WDataSetScalar> >(
        new WModuleOutputData<WDataSetScalar> ( shared_from_this(), "Resampled",
                                                "The resampled data set." ) );
    addConnector( m_resampled );

    WModule::connectors();
}

void WMResampleRegular::properties()
{
    m_propCondition = std::shared_ptr< WCondition >( new WCondition() );

    WModule::properties();
}

void WMResampleRegular::requirements()
{
    // Put the code for your requirements here. See "src/modules/template/" for an extensively documented example.
}

void WMResampleRegular::moduleMain()
{
    m_moduleState.add( m_original->getDataChangedCondition() );
    m_moduleState.add( m_target->getDataChangedCondition() );
    m_moduleState.add( m_propCondition );

    // signal ready state
    ready();

    // loop until the module container requests the module to quit
    while( !m_shutdownFlag() )
    {
        debugLog() << "Waiting ...";
        m_moduleState.wait();

        // woke up since the module is requested to finish
        if( m_shutdownFlag() )
        {
            break;
        }

        std::shared_ptr< WDataSetScalar > originalData = m_original->getData();
        std::shared_ptr< WDataSetScalar > targetData = m_target->getData();

        // If no data found go into waiting state again.
        if( !originalData || !targetData )
        {
            continue;
        }


        std::shared_ptr< WGridRegular3D > resampledGrid = std::dynamic_pointer_cast< WGridRegular3D >( targetData->getGrid() );

        std::shared_ptr<WValueSetBase> vals;
        vals = std::dynamic_pointer_cast<WValueSetBase >( originalData->getValueSet() );

        std::shared_ptr< std::vector< float > > theValues;
        theValues =  std::make_shared< std::vector< float > >();

        size_t nX = resampledGrid->getNbCoordsX();
        size_t nY = resampledGrid->getNbCoordsY();
        size_t nZ = resampledGrid->getNbCoordsZ();

        std::shared_ptr< WProgress > progress( new WProgress( "Resampling", nZ ) );
        m_progress->addSubProgress( progress );

        for( size_t idZ = 0; idZ < nZ; ++idZ )
        {
            ++*progress;
            for( size_t idY = 0; idY < nY; ++idY )
            {
                for( size_t idX = 0; idX < nX; ++idX )
                {
                    bool valid;
                    theValues->push_back( static_cast<float>( originalData->interpolate( resampledGrid->getPosition( idX, idY, idZ ), &valid ) ) );
                }
            }
        }

        std::shared_ptr< WValueSet< float > >  newValueSet;
        newValueSet = std::shared_ptr< WValueSet< float > >( new WValueSet<float>( vals->order(), vals->dimension(), theValues ) );

        m_resampled->updateData( std::shared_ptr<WDataSetScalar>( new WDataSetScalar( newValueSet, resampledGrid ) ) );
        progress->finish();
    }
}
