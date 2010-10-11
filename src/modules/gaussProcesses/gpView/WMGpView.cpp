//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2009 OpenWalnut Community, BSV-Leipzig and CNCF-CBS
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

#include <string>

#include "../../../graphicsEngine/WGEGeodeUtils.h"
#include "../../../kernel/WKernel.h"
#include "../../emptyIcon.xpm" // Please put a real icon here.

#include "WMGpView.h"

WMGpView::WMGpView():
    WModule()
{
}

WMGpView::~WMGpView()
{
}

boost::shared_ptr< WModule > WMGpView::factory() const
{
    return boost::shared_ptr< WModule >( new WMGpView() );
}

const char** WMGpView::getXPMIcon() const
{
    return emptyIcon_xpm; // Please put a real icon here.
}
const std::string WMGpView::getName() const
{
    return "Gaussian Process Viewer";
}

const std::string WMGpView::getDescription() const
{
    return "Displays gaussian processes. It is intended to display espically GP representing deterministic tracts.";
}

void WMGpView::connectors()
{
    m_gpIC = WModuleInputData< WDataSetGP >::createAndAdd( shared_from_this(), "gpIn", "The Gaussian processes" );

    WModule::connectors();
}

void WMGpView::properties()
{
    m_normal = m_properties->addProperty( "#Plane normal", "The normal of the plane", wmath::WPosition( 1.0, 0.0, 0.0 ) );
    m_pos = m_properties->addProperty( "#Plane position", "The position of the plane", wmath::WPosition( 0.0, 0.0, 0.0 ) );
    m_scale = m_properties->addProperty( "Scaling factor", "How much the plane is streched", 0.0 );

    WModule::properties();
}

void WMGpView::moduleMain()
{
    m_moduleState.setResetable( true, true ); // remember actions when actually not waiting for actions
    m_moduleState.add( m_gpIC->getDataChangedCondition() );
    m_moduleState.add( m_scale->getCondition() );

    ready();

    m_rootNode = osg::ref_ptr< WGEManagedGroupNode > ( new WGEManagedGroupNode( m_active ) );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_rootNode );
    debugLog() << "Insert quad-plane";
    m_rootNode->clear();
    m_rootNode->insert( wge::genUnitSubdividedPlane( 100, 100 ) );

    while ( !m_shutdownFlag() ) // loop until the module container requests the module to quit
    {
        debugLog() << "Waiting..";
        m_moduleState.wait();
        if ( !m_gpIC->getData().get() ) // ok, the output has not yet sent data
        {
            continue;
        }

        boost::shared_ptr< WDataSetGP > dataSet = m_gpIC->getData();
        if( !dataSet )
        {
            debugLog() << "Invalid data--> continue";
            continue;
        }
        if( m_gpIC->handledUpdate() )
        {
            debugLog() << "Input has been updated...";
        }
        debugLog() << "Resetting the matrix.";
        m_rootNode->setMatrix( generateMatrix() );
    }
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootNode );
}

osg::Matrixd WMGpView::generateMatrix() const
{
    osg::Matrixd trans;
    trans.makeTranslate( m_pos->get() );

    osg::Matrixd scale;
    scale.makeScale( m_scale->get(), m_scale->get(), m_scale->get() );

    osg::Matrixd rot;
    rot.makeRotate( wmath::WVector3D( 0.0, 0.0, 1.0 ), m_normal->get() );

    return trans*scale*rot;
}