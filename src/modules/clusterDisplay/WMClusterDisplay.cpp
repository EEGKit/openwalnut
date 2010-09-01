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

#include <queue>
#include <string>
#include <vector>

#include <boost/regex.hpp>

#include <osgGA/TrackballManipulator>
#include <osgGA/StateSetManipulator>
#include <osgViewer/ViewerEventHandlers>
#include <osgWidget/Util> //NOLINT
#include <osgWidget/ViewerEventHandlers> //NOLINT
#include <osgWidget/WindowManager> //NOLINT

#include "../../common/WPathHelper.h"
#include "../../common/WPropertyHelper.h"

#include "../../graphicsEngine/WROISphere.h"
#include "../../graphicsEngine/WGEUtils.h"

#include "../../kernel/WKernel.h"
#include "clusterDisplay.xpm" // Please put a real icon here.

#include "WMClusterDisplay.h"

// This line is needed by the module loader to actually find your module. Do not remove. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMClusterDisplay )

WMClusterDisplay::WMClusterDisplay():
    WModule(),
    m_widgetDirty( false ),
    m_biggestClusterButtonsChanged( false ),
    m_dendrogramDirty( false ),
    m_labelMode( 0 )
{
}

WMClusterDisplay::~WMClusterDisplay()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMClusterDisplay::factory() const
{
    // See "src/modules/template/" for an extensively documented example.
    return boost::shared_ptr< WModule >( new WMClusterDisplay() );
}

const char** WMClusterDisplay::getXPMIcon() const
{
    return clusterDisplay_xpm; // Please put a real icon here.
}
const std::string WMClusterDisplay::getName() const
{
    // Specify your module name here. This name must be UNIQUE!
    return "ClusterDisplay";
}

const std::string WMClusterDisplay::getDescription() const
{
    // Specify your module description here. Be detailed. This text is read by the user.
    // See "src/modules/template/" for an extensively documented example.
    return "This module loads a text file containing the hierarchical tree representation of a fiber clustering"
            " and provides selection appropriate tools";
}

void WMClusterDisplay::connectors()
{
    using boost::shared_ptr;
    typedef WModuleInputData< const WDataSetFibers > FiberInputData;  // just an alias

    m_fiberInput = shared_ptr< FiberInputData >( new FiberInputData( shared_from_this(), "fiberInput", "A loaded fiber dataset." ) );

    addConnector( m_fiberInput );
    // call WModules initialization
    WModule::connectors();
}

std::vector< std::string > WMClusterDisplay::readFile( const std::string fileName )
{
    std::ifstream ifs( fileName.c_str(), std::ifstream::in );

    std::vector< std::string > lines;

    std::string line;

    if ( ifs.is_open() )
    {
        debugLog() << "trying to load " << fileName;
        debugLog() << "file exists";
    }
    else
    {
        debugLog() << "trying to load " << fileName;
        debugLog() << "file doesn\'t exist";
        ifs.close();
        return lines;
    }

    while ( !ifs.eof() )
    {
        getline( ifs, line );

        lines.push_back( std::string( line ) );
    }

    ifs.close();

    return lines;
}

bool WMClusterDisplay::loadTreeAscii( std::string fileName )
{
    std::vector<std::string> lines;

    debugLog() << "start parsing tree file...";

    lines = readFile( fileName );

    if ( lines.size() == 0 )
    {
        return false;
    }

    for ( size_t i = 0; i < lines.size() - 1; ++i )
    {
        std::string &ls = lines[i];

        boost::regex reg1( "\\(" );
        ls = boost::regex_replace( ls, reg1, "(," );

        boost::regex reg2( "\\)" );
        ls = boost::regex_replace( ls, reg2, ",)" );

        boost::regex reg3( "\\ " );
        ls = boost::regex_replace( ls, reg3, "" );
    }

    std::vector<std::string>svec;

    for ( size_t i = 0; i < lines.size()-1; ++i )
    {
        svec.clear();
        boost::regex reg( "," );
        boost::sregex_token_iterator it( lines[i].begin(), lines[i].end(), reg, -1 );
        boost::sregex_token_iterator end;
        while ( it != end )
        {
            svec.push_back( *it++ );
        }

        size_t level = boost::lexical_cast<size_t>( svec[1] );
        if ( level == 0 )
        {
            m_tree.addLeaf();
        }
        else
        {
            // start after ( level (
            size_t k = 3;
            std::vector<size_t>leafes;
            while ( svec[k] != ")" )
            {
                leafes.push_back( boost::lexical_cast<size_t>( svec[k] ) );
                ++k;
            }
            //skip ) (
            k += 2;

            size_t cluster1 = boost::lexical_cast<size_t>( svec[k++] );
            size_t cluster2 = boost::lexical_cast<size_t>( svec[k++] );
            ++k;
            float data = boost::lexical_cast<float>( svec[k++] );

            m_tree.addCluster( cluster1, cluster2, level, leafes, data );

            if ( svec[k] != ")" )
            {
                std::cout << "parse error" << std::endl;
                return false;
            }
        }
    }
    debugLog() << m_tree.getClusterCount() << " clusters created.";

    debugLog() << "finished parsing tree file...";

    if ( m_tree.getLeafCount() == m_fiberInput->getData()->size() )
    {
        return true;
    }
    return false;
}

void WMClusterDisplay::initWidgets()
{
    osg::ref_ptr<osgViewer::Viewer> viewer = WKernel::getRunningKernel()->getGraphicsEngine()->getViewer()->getView();

    int height = viewer->getCamera()->getViewport()->height();
    int width = viewer->getCamera()->getViewport()->width();

    m_oldViewHeight = height;
    m_oldViewWidth = width;

    m_rootNode = osg::ref_ptr< WGEManagedGroupNode >( new WGEManagedGroupNode( m_active ) );

    m_wm = new osgWidget::WindowManager( viewer, 0.0f, 0.0f, MASK_2D );

    float xorig = 100.f;
    float yorig = 300.f;

    WOSGButton* current = new WOSGButton( std::string( "current" ), osgWidget::Box::VERTICAL, true, false );
    WOSGButton* up1 =     new WOSGButton( std::string( "+1" ),      osgWidget::Box::VERTICAL, true, false );
    WOSGButton* down11 =  new WOSGButton( std::string( "-1.1" ),    osgWidget::Box::VERTICAL, true, false );
    WOSGButton* down12 =  new WOSGButton( std::string( "-1.2" ),    osgWidget::Box::VERTICAL, true, false );

    WOSGButton* showN =  new WOSGButton( std::string( " N " ),    osgWidget::Box::VERTICAL, true, false );
    WOSGButton* showS =  new WOSGButton( std::string( " S " ),    osgWidget::Box::VERTICAL, true, false );
    WOSGButton* showE =  new WOSGButton( std::string( " E " ),    osgWidget::Box::VERTICAL, true, false );

    up1->setPosition(     osg::Vec3( xorig + 85.f,  yorig + 300.f, 0 ) );
    current->setPosition( osg::Vec3( xorig + 85.f,  yorig + 200.f, 0 ) );
    down11->setPosition(  osg::Vec3( xorig + 30.f,  yorig + 100.f, 0 ) );
    down12->setPosition(  osg::Vec3( xorig + 140.f, yorig + 100.f, 0 ) );

    showN->setPosition( osg::Vec3( xorig + 85.f,  yorig + 220.f, 0 ) );
    showS->setPosition( osg::Vec3( xorig + 110.f,  yorig + 220.f, 0 ) );
    showE->setPosition( osg::Vec3( xorig + 135.f,  yorig + 220.f, 0 ) );

    up1->managed( m_wm );
    current->managed( m_wm );
    down11->managed( m_wm );
    down12->managed( m_wm );

    showN->managed( m_wm );
    showS->managed( m_wm );
    showE->managed( m_wm );

    m_wm->addChild( current );
    m_wm->addChild( up1 );
    m_wm->addChild( down11 );
    m_wm->addChild( down12 );

    m_wm->addChild( showN );
    m_wm->addChild( showS );
    m_wm->addChild( showE );

    m_treeButtonList.push_back( current );
    m_treeButtonList.push_back( up1 );
    m_treeButtonList.push_back( down11 );
    m_treeButtonList.push_back( down12 );

    m_treeButtonList.push_back( showN );
    m_treeButtonList.push_back( showS );
    m_treeButtonList.push_back( showE );

    m_camera = new osg::Camera();
    m_camera->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF );

    m_camera->setProjectionMatrix( osg::Matrix::ortho2D( 0.0, width, 0.0f, height ) );
    m_camera->setReferenceFrame( osg::Transform::ABSOLUTE_RF );
    m_camera->setViewMatrix( osg::Matrix::identity() );
    m_camera->setClearMask( GL_DEPTH_BUFFER_BIT );
    m_camera->setRenderOrder( osg::Camera::POST_RENDER );

    m_rootNode->addChild( m_camera );
    m_camera->addChild( m_wm );

    viewer->addEventHandler( new osgWidget::MouseHandler( m_wm ) );
    viewer->addEventHandler( new osgWidget::KeyboardHandler( m_wm ) );
    viewer->addEventHandler( new osgWidget::ResizeHandler( m_wm, m_camera ) );
    viewer->addEventHandler( new osgWidget::CameraSwitchHandler( m_wm, m_camera ) );
    viewer->addEventHandler( new osgViewer::StatsHandler() );
    viewer->addEventHandler( new osgViewer::WindowSizeHandler() );
    viewer->addEventHandler( new osgGA::StateSetManipulator( viewer->getCamera()->getOrCreateStateSet() ) );

    m_wm->resizeAllWindows();

    m_rootNode->addUpdateCallback( new SafeUpdateCallback( this ) );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_rootNode );
}


void WMClusterDisplay::properties()
{
    // Initialize the properties
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    m_propSelectedCluster = m_properties->addProperty( "Selected Cluster", "", 0, m_propCondition );
    m_propSelectedCluster->setMin( 0 );
    m_propSelectedCluster->setMax( 0 );

    m_propSelectedClusterOffset = m_properties->addProperty( "Offset", "", 0, m_propCondition );
    m_propSelectedClusterOffset->setMin( -10 );
    m_propSelectedClusterOffset->setMax( 10 );


    m_propSubClusters = m_properties->addProperty( "Subclusters", "How many sub clusters should be selected", 10, m_propCondition );
    m_propSubClusters->setMin( 1 );
    m_propSubClusters->setMax( 50 );

    m_propSubLevelsToColor = m_properties->addProperty( "Sublevels to Color", "Number of subclusters to color differently", 4, m_propCondition );
    m_propSubLevelsToColor->setMin( 0 );
    m_propSubLevelsToColor->setMax( 0 );

    m_propMinSizeToColor = m_properties->addProperty( "Min size to Color", "Specifies a minimum size for a cluster so that too small cluster wont get an own color", 4, m_propCondition ); // NOLINT
    m_propMinSizeToColor->setMin( 1 );
    m_propMinSizeToColor->setMax( 200 );

    m_propMaxSubClusters = m_properties->addProperty( "Max clusters in box ", "", 1, m_propCondition );
    m_propMaxSubClusters->setMin( 1 );
    m_propMaxSubClusters->setMax( 100 );

    m_propBoxClusterRatio = m_properties->addProperty( "Box-Cluster Ratio", "", 0.9, m_propCondition );
    m_propBoxClusterRatio->setMin( 0.0 );
    m_propBoxClusterRatio->setMax( 1.0 );

    m_propShowTree = m_properties->addProperty( "Show tree", "", true, m_propCondition );
    m_propShowDendrogram = m_properties->addProperty( "Show dendrogram", "", true, m_propCondition );

    m_propTreeFile = m_properties->addProperty( "Tree file", "", WPathHelper::getAppPath() );
    m_readTriggerProp = m_properties->addProperty( "Do read",  "Press!", WPVBaseTypes::PV_TRIGGER_READY, m_propCondition );
    WPropertyHelper::PC_PATHEXISTS::addTo( m_propTreeFile );
}

void WMClusterDisplay::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_propCondition );
    m_moduleState.add( m_active->getUpdateCondition() );
    m_moduleState.add( m_fiberInput->getDataChangedCondition() );

    ready();

    bool treeLoaded = false;

    // no text file was found, wait for the user to manually load on
    while ( !m_shutdownFlag() )
    {
        if ( m_shutdownFlag() )
        {
            break;
        }

        m_moduleState.wait();

        if ( m_dataSet != m_fiberInput->getData() )
        {
            m_dataSet = m_fiberInput->getData();
            std::string fn = m_dataSet->getFileName();
            std::string ext( ".fib" );
            std::string csvExt( "_hie.txt" );
            fn.replace( fn.find( ext ), ext.size(), csvExt );
            treeLoaded = loadTreeAscii( fn );
            if ( treeLoaded )
            {
                break;
            }
        }


        if ( m_readTriggerProp->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
        {
            std::string fileName = m_propTreeFile->get().file_string().c_str();
            treeLoaded = loadTreeAscii( fileName );
            m_readTriggerProp->set( WPVBaseTypes::PV_TRIGGER_READY, true );
            if ( treeLoaded )
            {
                break;
            }
        }
    }

    m_propTreeFile->setHidden( true );
    m_readTriggerProp->setHidden( true );

    m_propSelectedCluster->setMin( m_tree.getLeafCount() );
    m_propSelectedCluster->setMax( m_tree.getClusterCount() - 1 );
    m_propSelectedCluster->set( m_tree.getClusterCount() - 1 );
    m_propSubLevelsToColor->setMax( m_tree.getLevel( m_propSelectedCluster->get() ) );
    m_propMinSizeToColor->setMax( 100 );
    m_rootCluster = m_propSelectedCluster->get();

    m_propSelectedClusterOffset->setMax( m_tree.getMaxLevel() - m_tree.getLevel( m_propSelectedCluster->get() ) );
    m_propSelectedClusterOffset->setMin( 0 - m_tree.getLevel( m_propSelectedCluster->get() ) );

    initWidgets();
    WKernel::getRunningKernel()->getRoiManager()->setExternalBitfield( m_tree.getOutputBitfield( m_propSelectedCluster->get( true ) ) );
    WKernel::getRunningKernel()->getRoiManager()->setUseExternalBitfield( m_active->get( true ) );
    m_widgetDirty = true;
    updateWidgets();

    WKernel::getRunningKernel()->getRoiManager()->getProperties()->getProperty( "dirty" )->getUpdateCondition()->subscribeSignal(
            boost::bind( &WMClusterDisplay::handleRoiChanged, this ) );

    m_dendrogramGeode = new WDendrogram( &m_tree, m_rootCluster, 1000, 500 );
    m_camera->addChild( m_dendrogramGeode );


    m_propSelectedCluster->get( true );
    m_propSubClusters->get( true );
    m_propSubLevelsToColor->get( true );
    m_propMinSizeToColor->get( true );

    // main loop
    while ( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        if ( m_shutdownFlag() )
        {
           break;
        }

        if( m_propSelectedCluster->changed() )
        {
            handleSelectedClusterChanged();
        }

        if ( m_propSelectedClusterOffset->changed() )
        {
            handleOffsetChanged();
        }

        if( m_propSubClusters->changed() )
        {
            handleBiggestClustersChanged();
        }

        if ( m_propSubLevelsToColor->changed() )
        {
            handleColoringChanged();
        }

        if ( m_propMinSizeToColor->changed() )
        {
            handleMinSizeChanged();
        }

        if ( m_propBoxClusterRatio->changed() || m_propMaxSubClusters->changed() )
        {
            handleRoiChanged();
        }

        if ( m_propShowTree->changed() )
        {
            m_widgetDirty = true;
        }

        if ( m_propShowDendrogram->changed() )
        {
            m_dendrogramDirty = true;
        }

        if ( m_active->changed() )
        {
            WKernel::getRunningKernel()->getRoiManager()->setUseExternalBitfield( m_active->get( true ) );
        }
    }

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootNode );
}

void WMClusterDisplay::handleSelectedClusterChanged()
{
    m_rootCluster = m_propSelectedCluster->get( true );
    m_propSelectedClusterOffset->setMax( m_tree.getMaxLevel() - m_tree.getLevel( m_propSelectedCluster->get() ) );
    m_propSelectedClusterOffset->setMin( 0 - m_tree.getLevel( m_propSelectedCluster->get() ) );
    m_propSelectedClusterOffset->set( 0 );
    m_propSelectedClusterOffset->get( true );

    WKernel::getRunningKernel()->getRoiManager()->setExternalBitfield( m_tree.getOutputBitfield( m_rootCluster ) );
    m_propSubLevelsToColor->setMax( m_tree.getLevel( m_rootCluster ) );
    colorClusters( m_propSelectedCluster->get( true ) );
    m_dendrogramDirty = true;
}

void WMClusterDisplay::handleOffsetChanged()
{
    if ( m_propSelectedClusterOffset->get( true ) == 0 )
    {
        m_rootCluster = m_propSelectedCluster->get( true );
    }
    else if ( m_propSelectedClusterOffset->get( true ) > 0 )
    {
        int plus = m_propSelectedClusterOffset->get( true );
        m_rootCluster = m_propSelectedCluster->get( true );
        while ( plus > 0 )
        {
            m_rootCluster = m_tree.getParent( m_rootCluster );
            --plus;
        }
    }
    else
    {
        int minusOff = m_propSelectedClusterOffset->get( true );
        m_rootCluster = m_propSelectedCluster->get( true );

        while ( minusOff < 0 )
        {
            if ( m_tree.getLevel( m_rootCluster ) > 0 )
            {
                size_t left =  m_tree.getChildren( m_rootCluster ).first;
                size_t right = m_tree.getChildren( m_rootCluster ).second;

                size_t leftSize =  m_tree.size( left );
                size_t rightSize = m_tree.size( right );

                if ( leftSize >= rightSize )
                {
                    m_rootCluster = left;
                }
                else
                {
                    m_rootCluster = right;
                }
            }
            ++minusOff;
        }
    }
    WKernel::getRunningKernel()->getRoiManager()->setExternalBitfield( m_tree.getOutputBitfield( m_rootCluster ) );
    m_propSubLevelsToColor->setMax( m_tree.getLevel( m_rootCluster ) );

    m_dendrogramDirty = true;
}

void WMClusterDisplay::handleBiggestClustersChanged()
{
    m_biggestClusters = m_tree.findXBiggestClusters( m_propSelectedCluster->get(), m_propSubClusters->get( true ) );

    WKernel::getRunningKernel()->getRoiManager()->setExternalBitfield( m_tree.getOutputBitfield( m_biggestClusters ) );

    m_tree.colorCluster( m_tree.getClusterCount() - 1, WColor( 0.3, 0.3, 0.3, 1.0 ) );

    for ( size_t k = 0; k < m_biggestClusters.size(); ++k )
    {
        size_t current = m_biggestClusters[k];
        setColor( m_tree.getLeafesForCluster( current ), wge::getNthHSVColor( k, m_biggestClusters.size() ) );
        m_tree.colorCluster( current, wge::getNthHSVColor( k, m_biggestClusters.size() ) );
    }

    m_dendrogramDirty = true;
    m_widgetDirty = true;
    m_biggestClusterButtonsChanged = true;
}

void WMClusterDisplay::handleColoringChanged()
{
    WKernel::getRunningKernel()->getRoiManager()->setExternalBitfield( m_tree.getOutputBitfield( m_propSelectedCluster->get( true ) ) );
    m_propSubLevelsToColor->get( true );
    m_propMinSizeToColor->get( true );
    colorClusters( m_propSelectedCluster->get( true ) );
    m_dendrogramDirty = true;
}

void WMClusterDisplay::handleMinSizeChanged()
{
    m_dendrogramDirty = true;
}

void WMClusterDisplay::handleRoiChanged()
{
    if ( m_active->get() )
    {
        m_biggestClusters = m_tree.getBestClustersFittingRoi( m_propBoxClusterRatio->get( true ), m_propMaxSubClusters->get( true ) );

        WKernel::getRunningKernel()->getRoiManager()->setExternalBitfield( m_tree.getOutputBitfield( m_biggestClusters ) );

        for ( size_t k = 0; k < m_biggestClusters.size(); ++k )
        {
            size_t current = m_biggestClusters[k];
            setColor( m_tree.getLeafesForCluster( current ), wge::getNthHSVColor( k, m_biggestClusters.size() ) );
        }

        m_widgetDirty = true;
        m_biggestClusterButtonsChanged = true;
    }
}


void WMClusterDisplay::updateWidgets()
{
    osg::ref_ptr<osgViewer::Viewer> viewer = WKernel::getRunningKernel()->getGraphicsEngine()->getViewer()->getView();

    int height = viewer->getCamera()->getViewport()->height();
    int width = viewer->getCamera()->getViewport()->width();

    if ( ( height != m_oldViewHeight ) || width != m_oldViewWidth )
    {
        m_oldViewHeight = height;
        m_oldViewWidth = width;

        m_dendrogramDirty = true;
    }


    if ( !widgetClicked() && !m_widgetDirty && !m_dendrogramDirty )
    {
        return;
    }
    // store current cluster id for ease of access and better readability
    size_t current = m_propSelectedCluster->get();
    size_t up1, down11, down12; // up2, down21, down22, down23, down24;

    m_treeButtonList[0]->setId( current );
    m_treeButtonList[0]->setLabel( createLabel( current ) );

    for ( size_t i = 0; i < m_treeButtonList.size(); ++i )
    {
        m_treeButtonList[i]->hide();
    }

    if ( m_propShowTree->get( true ) )
    {
        m_treeButtonList[0]->show();

        // are we on top?
        if ( m_tree.getLevel( current ) < m_tree.getMaxLevel() )
        {
            // not yet
            up1 = m_tree.getParent( current );
            m_treeButtonList[1]->setId( up1 );
            m_treeButtonList[1]->setLabel( createLabel( up1 ) );
            m_treeButtonList[1]->show();
        }
        // are we all the way down
        if ( m_tree.getLevel( current ) > 0 )
        {
            down11 = m_tree.getChildren( current ).first;
            down12 = m_tree.getChildren( current ).second;

            m_treeButtonList[2]->setId( down11 );
            m_treeButtonList[2]->setLabel( createLabel( down11 ) );
            m_treeButtonList[2]->show();

            m_treeButtonList[3]->setId( down12 );
            m_treeButtonList[3]->setLabel( createLabel( down12 ) );
            m_treeButtonList[3]->show();
        }

        m_treeButtonList[4]->show();
        m_treeButtonList[5]->show();
        m_treeButtonList[6]->show();
    }

    if ( m_biggestClusterButtonsChanged )
    {
        for ( size_t i = 0; i < m_biggestClustersButtonList.size(); ++i )
        {
            m_wm->removeChild( m_biggestClustersButtonList[i] );
        }

        m_biggestClustersButtonList.clear();
        for ( size_t i = 0; i < m_biggestClusters.size(); ++i )
        {
            osg::ref_ptr<WOSGButton> newButton1 = osg::ref_ptr<WOSGButton>( new WOSGButton( std::string( "" ),
                    osgWidget::Box::VERTICAL, true, false ) );
            newButton1->setPosition( osg::Vec3( 10.f,  i * 20.f, 0 ) );
            newButton1->setId( 10000000 + m_biggestClusters[i] );
            newButton1->setLabel( std::string( " S " ) );
            newButton1->managed( m_wm );
            m_wm->addChild( newButton1 );
            m_biggestClustersButtonList.push_back( newButton1 );
            newButton1->setBackgroundColor( wge::getNthHSVColor( i, m_biggestClusters.size() ) );

            osg::ref_ptr<WOSGButton> newButton = osg::ref_ptr<WOSGButton>( new WOSGButton( std::string( "" ),
                    osgWidget::Box::VERTICAL, true, true ) );
            newButton->setPosition( osg::Vec3( 35.f,  i * 20.f, 0 ) );
            newButton->setId( m_biggestClusters[i] );
            newButton->setLabel( createLabel( m_biggestClusters[i] ) );
            newButton->managed( m_wm );
            m_wm->addChild( newButton );
            m_biggestClustersButtonList.push_back( newButton );
            newButton->setBackgroundColor( wge::getNthHSVColor( i, m_biggestClusters.size() ) );
        }
        m_biggestClusterButtonsChanged = false;
    }
    m_wm->resizeAllWindows();

    if ( m_dendrogramDirty )
    {
        //m_camera->removeChild( m_dendrogramGeode );
        m_camera->removeChild( 1, 1 );

        if ( m_propShowDendrogram->get( true ) )
        {
            m_dendrogramGeode = new WDendrogram( &m_tree, m_rootCluster, m_propMinSizeToColor->get(), width - 120, height / 2 , 100 );
            m_camera->addChild( m_dendrogramGeode );
        }
        m_dendrogramDirty = false;
    }
}

std::string WMClusterDisplay::createLabel( size_t id )
{
    switch ( m_labelMode )
    {
        case 1:
            return boost::lexical_cast<std::string>( m_tree.size( id ) );
            break;
        case 2:
            return boost::lexical_cast<std::string>( m_tree.getCustomData( id ) );
            break;
        default:
            return boost::lexical_cast<std::string>( id );
    }
}

bool WMClusterDisplay::widgetClicked()
{
    bool clicked = false;

    if ( m_treeButtonList[0]->clicked() )
    {
        clicked = true;
        m_propSelectedCluster->set( m_rootCluster );
        m_propSelectedClusterOffset->set( 0 );
        m_propSelectedClusterOffset->setMax( m_tree.getMaxLevel() - m_tree.getLevel( m_propSelectedCluster->get() ) );
        m_propSelectedClusterOffset->setMin( 0 - m_tree.getLevel( m_propSelectedCluster->get() ) );
    }

    for ( size_t i = 1; i < m_treeButtonList.size() - 3; ++i )
    {
        if ( m_treeButtonList[i]->clicked() )
        {
            clicked = true;
            m_propSelectedCluster->set( m_treeButtonList[i]->getId() );
        }
    }

    for ( size_t i = 0; i < 3; ++i )
    {
        if ( m_treeButtonList[4 + i]->clicked() )
        {
            clicked = true;
            m_labelMode = i;
        }
    }

    bool biggestClusterSelectionChanged = false;
    for ( size_t i = 0; i < m_biggestClustersButtonList.size(); ++i )
    {
        if ( m_biggestClustersButtonList[i]->clicked() )
        {
            if ( m_biggestClustersButtonList[i]->getId() < 10000000 )
            {
                biggestClusterSelectionChanged = true;
                clicked = true;
            }
            else
            {
                clicked = true;
                m_propSelectedCluster->set( m_biggestClustersButtonList[i]->getId() - 10000000 );
            }
        }
    }

    if ( biggestClusterSelectionChanged )
    {
        std::vector<size_t>activeClusters;
        for ( size_t i = 0; i < m_biggestClustersButtonList.size(); ++i )
        {
            if ( m_biggestClustersButtonList[i]->pushed() )
            {
                activeClusters.push_back( m_biggestClustersButtonList[i]->getId() );
            }
        }

        WKernel::getRunningKernel()->getRoiManager()->setExternalBitfield( m_tree.getOutputBitfield( activeClusters ) );
    }
    return clicked;
}

void WMClusterDisplay::colorClusters( size_t current )
{
    size_t num = m_propSubLevelsToColor->get();
    size_t size = m_propMinSizeToColor->get();

    m_tree.colorCluster( m_tree.getClusterCount() - 1, WColor( 0.3, 0.3, 0.3, 1.0 ) );

    std::vector<size_t>finalList;
    std::queue<size_t>currentLevelList;

    currentLevelList.push( current );

    std::queue<size_t>nextLevelList;
    while ( num > 0 )
    {
        while ( !currentLevelList.empty() )
        {
            size_t cluster = currentLevelList.front();
            currentLevelList.pop();

            if ( m_tree.getLevel( cluster ) > 0 )
            {
                size_t left = m_tree.getChildren( cluster ).first;
                size_t right = m_tree.getChildren( cluster ).second;

                if (  m_tree.size( left ) >= size )
                {
                    nextLevelList.push( left );
                }
                else
                {
                    //finalList.push( left );
                }
                if ( m_tree.size( right ) >= size )
                {
                    nextLevelList.push( right );
                }
                else
                {
                    //finalList.push( right );
                }
            }
            else
            {
                finalList.push_back( cluster );
            }
        }

        while ( !nextLevelList.empty() )
        {
            size_t cluster = nextLevelList.front();
            nextLevelList.pop();
            currentLevelList.push( cluster );
        }
        --num;
    }


    while ( !currentLevelList.empty() )
    {
        size_t cluster = currentLevelList.front();
        currentLevelList.pop();
        finalList.push_back( cluster );
    }

    int n = 0;
    int parts = finalList.size();


    for ( size_t i = 0; i < finalList.size(); ++i )
    {
        size_t cluster = finalList[i];
        m_tree.colorCluster( cluster, wge::getNthHSVColor( n, parts ) );
        setColor( m_tree.getLeafesForCluster( cluster ), wge::getNthHSVColor( n++, parts ) );
    }

    WKernel::getRunningKernel()->getRoiManager()->setExternalBitfield( m_tree.getOutputBitfield( finalList ) );

    m_biggestClusters.clear();
    m_biggestClusterButtonsChanged = true;
}

void WMClusterDisplay::setColor( std::vector<size_t> clusters, WColor color )
{
    boost::shared_ptr< std::vector< float > >colorField = WKernel::getRunningKernel()->getRoiManager()->getCustomColors();
    boost::shared_ptr< std::vector< size_t > > starts   = WKernel::getRunningKernel()->getRoiManager()->getStarts();
    boost::shared_ptr< std::vector< size_t > > lengths  = WKernel::getRunningKernel()->getRoiManager()->getLengths();

    for ( size_t i = 0; i < clusters.size(); ++i )
    {
        size_t current = clusters[i];

        for ( size_t k = (*starts)[current]; k < (*starts)[current] + (*lengths)[current]; ++k)
        {
            (*colorField)[k*3] = color.getRed();
            (*colorField)[k*3+1] = color.getGreen();
            (*colorField)[k*3+2] = color.getBlue();
        }
    }
}
