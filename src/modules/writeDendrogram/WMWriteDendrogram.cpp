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

#include <memory>
#include <string>

#include "WMWriteDendrogram.h"
#include "WMWriteDendrogram.xpm"
#include "core/common/WPropertyHelper.h"
#include "core/dataHandler/io/WWriterDendrogram.h"
#include "core/kernel/WKernel.h"

W_LOADABLE_MODULE( WMWriteDendrogram )

WMWriteDendrogram::WMWriteDendrogram():
    WModule()
{
}

WMWriteDendrogram::~WMWriteDendrogram()
{
}

std::shared_ptr< WModule > WMWriteDendrogram::factory() const
{
    return std::shared_ptr< WModule >( new WMWriteDendrogram() );
}

const char** WMWriteDendrogram::getXPMIcon() const
{
    return WMWriteDendrogram_xpm;
}

const std::string WMWriteDendrogram::getName() const
{
    return "Write Dendrogram";
}

const std::string WMWriteDendrogram::getDescription() const
{
    return "Writes Dendrogram";
}

void WMWriteDendrogram::connectors()
{
    m_dendrogramIC = WModuleInputData< const WDendrogram >::createAndAdd( shared_from_this(), "dendrogramInput", "A the Dendrogram" );

    WModule::connectors();
}

void WMWriteDendrogram::properties()
{
    m_savePath = m_properties->addProperty( "Save Path", "Where to save the result", boost::filesystem::path( "/no/such/file" ) );
    WPropertyHelper::PC_NOTEMPTY::addTo( m_savePath );
    m_run      = m_properties->addProperty( "Save", "Start saving", WPVBaseTypes::PV_TRIGGER_READY );

    WModule::properties();
}

void WMWriteDendrogram::moduleMain()
{
    m_moduleState.add( m_dendrogramIC->getDataChangedCondition() );
    m_moduleState.add( m_run->getCondition() );

    ready();

    while( !m_shutdownFlag() )
    {
        infoLog() << "Waiting...";
        m_moduleState.wait();

        if( !m_dendrogramIC->getData() )
        {
            continue;
        }

        if( m_run->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
        {
            WWriterDendrogram w( m_savePath->get(), true );
            infoLog() << "Start saving to: " << m_savePath->get();
            w.write( m_dendrogramIC->getData() );
            infoLog() << "Saving done.";
            m_run->set( WPVBaseTypes::PV_TRIGGER_READY, false );
        }
    }
}
