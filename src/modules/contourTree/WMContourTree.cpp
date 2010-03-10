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

#include <string>

#include <osg/ShapeDrawable>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Material>
#include <osg/StateAttribute>

#include "../../common/WColor.h"
#include "../../dataHandler/WDataSetSingle.h"
#include "../../dataHandler/datastructures/WJoinContourTree.h"
#include "../../kernel/WKernel.h"
#include "WMContourTree.h"

WMContourTree::WMContourTree():
    WModule()
{
}

WMContourTree::~WMContourTree()
{
}

boost::shared_ptr< WModule > WMContourTree::factory() const
{
    return boost::shared_ptr< WModule >( new WMContourTree() );
}

const std::string WMContourTree::getName() const
{
    return "Contour Tree";
}

const std::string WMContourTree::getDescription() const
{
    return "Computes the Contour-Tree and its Join- and Split-Tree";
}

void WMContourTree::connectors()
{
    typedef WModuleInputData< WDataSetSingle > InputType;
    m_input = boost::shared_ptr< InputType >( new InputType( shared_from_this(), "scalarField", "DataSetSingle to compute contour tree for" ) );
    addConnector( m_input );

    typedef WModuleOutputData< WJoinContourTree > OutputType;
    m_output = boost::shared_ptr< OutputType >( new OutputType( shared_from_this(), "joinTree", "Join-Tree of the dataset" ) );
    addConnector( m_output );

    WModule::connectors();
}

void WMContourTree::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );

    ready();

    while( !m_shutdownFlag() )
    {
        m_moduleState.wait();
        if( m_shutdownFlag() )
        {
            break;
        }

        if( !m_input->getData() ) // incase of invalid data (yet) skip and continue
        {
            continue;
        }

        boost::shared_ptr< WJoinContourTree > joinTree = boost::shared_ptr< WJoinContourTree >( new WJoinContourTree( m_input->getData() ) );
        joinTree->buildJoinTree();
        m_output->updateData( joinTree );
    }
}
