//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2015 A. Betz, D- Gerlicher, OpenWalnut Community, Nemtics, BSV@Uni-Leipzig
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
#include <cmath>
#include <string>
#include <vector>

#include <osgViewer/View>
#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/CullFace>

#include "core/dataHandler/WDataSetScalar.h"
#include "core/graphicsEngine/callbacks/WGELinearTranslationCallback.h"
#include "core/graphicsEngine/shaders/WGEPropertyUniform.h"
#include "core/graphicsEngine/shaders/WGEShader.h"
#include "core/graphicsEngine/WGEGeodeUtils.h"
#include "core/graphicsEngine/WGEColormapping.h"
#include "core/graphicsEngine/WGEManagedGroupNode.h"
#include "core/graphicsEngine/WGraphicsEngine.h"
#include "core/graphicsEngine/WPickHandler.h"
#include "core/graphicsEngine/WPickInfo.h"
#include "core/kernel/WKernel.h"
#include "core/kernel/WModuleInputData.h"
#include "core/kernel/WModuleOutputData.h"

#include "core/graphicsEngine/geodes/WGEGridNode.h"

#include "WMPickingDVR.h"
#include "WMPickingDVRHelper.h"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMPickingDVR )

WMPickingDVR::WMPickingDVR():
WModule(),
    m_propCondition( new WCondition() )
{
    this->m_bIntersected = false;
    this->m_posStart  = osg::Vec3f( 0.0, 0.0, 0.0 );
    this->m_posEnd   = osg::Vec3f( 0.0, 0.0, 0.0 );
}

WMPickingDVR::~WMPickingDVR()
{
}

boost::shared_ptr< WModule > WMPickingDVR::factory() const
{
    return boost::shared_ptr< WModule >( new WMPickingDVR() );
}

const std::string WMPickingDVR::getName() const
{
    return "Picking in DVR";
}

const std::string WMPickingDVR::getDescription() const
{
    return "Picks a 3D Position from the DVR rendered dataset.";
}

void WMPickingDVR::connectors()
{
    // The transfer function for our DVR
    m_transferFunction = WModuleInputData< WDataSetSingle >::createAndAdd( shared_from_this(), "transfer function", "The 1D transfer function." );

    //Scalar field
    m_scalarIC = WModuleInputData< WDataSetScalar >::createAndAdd( shared_from_this(), "scalar data", "Scalar data." );

    WModule::connectors();
}

void WMPickingDVR::properties()
{
    //Color Property
    m_color = m_properties->addProperty( "Crosshair color", "Crosshair Color", WColor( 0.5f, 0.5f, 0.5f, 1.0f ), m_propCondition );

    // ---> Put the code for your properties here. See "src/modules/template/" for an extensively documented example.
    m_sampleSteps = m_properties->addProperty( "Samples - steps",
                      "Number of samples. Choose this appropriately for the settings used for the DVR itself.",
                       256,
                       m_propCondition );
    m_sampleSteps->setMin( 0 );
    m_sampleSteps->setMax( 10000 );

    m_opacityCorrectionEnabled = m_properties->addProperty( "Opacity correction",
            "If enabled, opacities are assumed to be relative to the "
            "sample count. Choose this appropriately for the settings used for the DVR itself.",
            true,
            m_propCondition );

    m_crossThickness = m_properties->addProperty( "Crosshair thickness", "Crosshair thickness", 0.5, m_propCondition );
    m_crossThickness->setMin( 0.001 );
    m_crossThickness->setMax( 1.0 );

    m_crossSize = m_properties->addProperty( "Crosshair size", "Crosshair size", 100.0, m_propCondition );
    m_crossSize->setMin( 0.001 );
    m_crossSize->setMax( 1000.0 );

    m_pickingCriteriaList = boost::shared_ptr< WItemSelection >( new WItemSelection() );
    m_pickingCriteriaList->addItem( WMPICKINGDVR_FIRST_HIT, WMPICKINGDVR_FIRST_HIT );
    m_pickingCriteriaList->addItem( WMPICKINGDVR_THRESHOLD, WMPICKINGDVR_THRESHOLD );
    m_pickingCriteriaList->addItem( WMPICKINGDVR_MOST_CONTRIBUTING, WMPICKINGDVR_MOST_CONTRIBUTING );
    m_pickingCriteriaList->addItem( WMPICKINGDVR_WYSIWYP, WMPICKINGDVR_WYSIWYP );
    m_pickingCriteriaList->addItem( WMPICKINGDVR_MAX_INT, WMPICKINGDVR_MAX_INT );

    m_pickingCriteriaCur = m_properties->addProperty( "Picking method",
                                                      "Select a picking method",
                                                      m_pickingCriteriaList->getSelectorFirst(),
                                                      m_propCondition );

    WPropertyHelper::PC_SELECTONLYONE::addTo( m_pickingCriteriaCur );
    WPropertyHelper::PC_NOTEMPTY::addTo( m_pickingCriteriaCur );

    m_alphaThreshold = m_properties->addProperty( "Alpha threshold %",
                                                  "Alpha value threshold for threshold picking mode.",
                                                  0.5,
                                                  m_propCondition );
    m_alphaThreshold->setMin( 0.0 );
    m_alphaThreshold->setMax( 1.0 );

    WModule::properties();
}

void WMPickingDVR::requirements()
{
}

void WMPickingDVR::moduleMain()
{
    // get notified about data changes
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_scalarIC->getDataChangedCondition() );
    m_moduleState.add( m_transferFunction->getDataChangedCondition() );
    m_moduleState.add( m_propCondition );

    ready();

    // graphics setup
    m_rootNode = osg::ref_ptr< WGEManagedGroupNode >( new WGEManagedGroupNode( m_active ) );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_rootNode );

    //Shader
    m_shader = new WGEShader( "WGELighting", m_localPath );
    m_shader->setDefine( "USE_MATERIAL_DIFFUSE" );

    //Get Camera and Register the callback
    boost::shared_ptr< WGraphicsEngine > graphicsEngine = WGraphicsEngine::getGraphicsEngine();
    boost::shared_ptr< WGEViewer > mainView = graphicsEngine->getViewerByName( "Main View" );

    //Register Pickhandler
    mainView->getPickHandler()->getPickSignal()->connect( boost::bind( &WMPickingDVR::pickHandler, this, _1 ) );

    // main loop
    while( !m_shutdownFlag() )
    {
        debugLog() << "Waiting ...";
        m_moduleState.wait();

        //Get Picking Mode
        WItemSelector selector  = m_pickingCriteriaCur->get( true );
        std::string  strRenderMode = selector.at( 0 )->getName();
        debugLog() << strRenderMode;

        updateModuleGUI( strRenderMode );

        // woke up since the module is requested to finish?
        if( m_shutdownFlag() )
        {
            break;
        }

        //Valid Positions
        if(this->m_bIntersected)
        {
            //Position Variables
            osg::Vec3f posStart  = this->m_posStart;
            osg::Vec3f posEnd  = this->m_posEnd;
            osg::Vec3f posSample = posStart;
            osg::Vec3f posPicking = posStart;
            osg::Vec3f vecDir  = posEnd - posStart;

            //Picking Variable
            double dMax   = 0.0;
            double dMin   = 0.0;
            double dAccAlpha  = 0.0;
            double dPickedAlpha = 0.0;
            double dMaxValue  = 0.0;
            bool bPickedPos  = false;

            //Calculate Step
            if(this->m_sampleSteps->get() > 0)
            {
                vecDir = vecDir / this->m_sampleSteps->get();
            }

            //Get Scalar Field
            boost::shared_ptr< WDataSetScalar > scalarData = m_scalarIC->getData();
            if(!scalarData)
            {
                errorLog()<< "[Invalid scalar field]";
                continue;
            }

            //Get Transferfunction Data
            boost::shared_ptr< WDataSetSingle > transferFunctionData = m_transferFunction->getData();
            if(!transferFunctionData)
            {
                errorLog()<< "[Invalid transferfunction data]";
                continue;
            }

            //Get Transferfunction Values
            boost::shared_ptr< WValueSetBase > transferFunctionValues = transferFunctionData->getValueSet();

            dMax  = scalarData->getMax();
            dMin  = scalarData->getMin();
            dMaxValue = dMin;

            std::vector<double> vecAlphaAcc;

            //Sampling
            for(int i = 0; i < this->m_sampleSteps->get(); i++)
            {
                //Scalarfield Values
                bool bSuccess = false;
                double dValue  = scalarData->interpolate( posSample, &bSuccess );

                //Add Step
                posSample = posSample + vecDir;

                if(!bSuccess)
                {
                    continue;
                }

                //Classification Variables
                double dNominator = dValue - dMin;
                double dDenominator = dMax - dMin;

                if( dDenominator == 0.0 )
                {
                    dDenominator = 0.0001;
                }

                //Classification: Convert Scalar to Color
                double dScalarPercentage = dNominator / dDenominator;
                int  iColorIdx   = dScalarPercentage * transferFunctionValues->size();

                //color
                WMPickingColor<double> color;

                //Get Color from transferfunction
                color.setRed( transferFunctionData->getSingleRawValue( iColorIdx * 4 + 0 ) );
                color.setGreen( transferFunctionData->getSingleRawValue( iColorIdx * 4 + 1 ) );
                color.setBlue( transferFunctionData->getSingleRawValue( iColorIdx * 4 + 2 ) );
                color.setAlpha( transferFunctionData->getSingleRawValue( iColorIdx * 4 + 3 ) );
                color.normalize();

                //gamma = alpha + beta - alpha * beta == currentAlpha + accedAlpha - currentAlpa * accedAlpha
                //alpha = currentAlpha
                //beta = accedAlpha
                double dCurrentAlpha = color.getAlpha();
                double dCurrentAlphaCorrected;

                if( m_opacityCorrectionEnabled->get( true ) )
                {
                    const double defaultNumberOfSteps = 128.0;
                    float relativeSampleDistance = defaultNumberOfSteps / m_sampleSteps->get();
                    dCurrentAlphaCorrected =  1.0 - pow( 1.0 - dCurrentAlpha, relativeSampleDistance );
                }
                else
                {
                    dCurrentAlphaCorrected = dCurrentAlpha;
                }

                dAccAlpha  = dCurrentAlphaCorrected + ( dAccAlpha - dCurrentAlphaCorrected * dAccAlpha );



                if( strRenderMode == WMPICKINGDVR_MAX_INT )
                {
                    //Maximum Intensity: maximal scalar value
                    if( dValue > dMaxValue )
                    {
                        dMaxValue = dValue;
                        posPicking = posSample;
                        bPickedPos = true;
                    }
                }
                else if( strRenderMode == WMPICKINGDVR_FIRST_HIT )
                {
                    //First Hit: first alpha value > 0.0
                    if( dCurrentAlpha > 0.0 )
                    {
                        dPickedAlpha = dCurrentAlpha;
                        posPicking  =  posSample;
                        bPickedPos  = true;
                        break;
                    }
                }
                else if( strRenderMode == WMPICKINGDVR_THRESHOLD )
                {
                    //Threshold: accumulated alpha value > threshold
                    if( dAccAlpha > this->m_alphaThreshold->get() )
                    {
                        dPickedAlpha = dCurrentAlpha;
                        posPicking  =  posSample;
                        bPickedPos  = true;
                        break;
                    }
                }
                else if( strRenderMode == WMPICKINGDVR_MOST_CONTRIBUTING )
                {
                    //Most Contributing: maximal alpha value
                    if( dCurrentAlpha > dPickedAlpha )
                    {
                        dPickedAlpha = dCurrentAlpha;
                        posPicking  =  posSample;
                        bPickedPos  = true;
                    }
                }
                else if( strRenderMode == WMPICKINGDVR_WYSIWYP )
                {
                    //WYSIWYP: Save all the accumulated alpha values
                    vecAlphaAcc.push_back( dAccAlpha );
                }
            }

            //WYSIWYP: Calculate the largest interval
            if( strRenderMode == WMPICKINGDVR_WYSIWYP )
            {
                //Derivative Variables
                std::vector<double> vecFirstDerivative;
                std::vector<double> vecSecondDerivative;

                calculateDerivativesWYSIWYP( vecAlphaAcc, vecFirstDerivative, vecSecondDerivative );

                //Create Intervals
                std::vector<int> vecIndicesLowerBounds;
                std::vector<int> vecIndicesUpperBounds;

                //Calculate Interval Boundaries
                double dOldDerivative = vecSecondDerivative[0];
                for( unsigned int j = 1; j < vecSecondDerivative.size(); j++ )
                {
                    if( dOldDerivative < 0.0 && vecSecondDerivative[j] >= 0.0
                        && ( vecIndicesLowerBounds.size() > 0 ) ) // need to have a lower bound already
                    {
                        vecIndicesUpperBounds.push_back( j );
                    }

                    if( dOldDerivative <= 0.0 && vecSecondDerivative[j] > 0.0 )
                    {
                        vecIndicesLowerBounds.push_back( j );
                    }


                    dOldDerivative = vecSecondDerivative[j];
                }

                //Calculate Max Diff
                double dDiff  = 0.0;
                double dMaxDiff = 0.0;
                int iSample  = -1;

                for( unsigned int j = 0; j < std::min( vecIndicesLowerBounds.size(), vecIndicesUpperBounds.size() ); j++ )
                {
                    //Calculate Diff
                    dDiff = vecAlphaAcc[vecIndicesUpperBounds[j]] - vecAlphaAcc[vecIndicesLowerBounds[j]];
                    debugLog() << "Interval [" <<  vecIndicesLowerBounds[j] << "," << vecIndicesUpperBounds[j] << "] = " << dDiff;

                    //Is Max Diff
                    if( dDiff > dMaxDiff )
                    {
                        dMaxDiff = dDiff;
                        iSample = vecIndicesLowerBounds[j];
                    }
                }
                debugLog() << "Sample of largest interval " << iSample;

                //Calculate Position
                if( iSample >= 0 )
                {
                    //Calculate pick position
                    posPicking = posStart + vecDir * iSample;
                    bPickedPos = true;
                }
            }

            //Rendering: update only if picked
            if( bPickedPos )
            {
                debugLog() << "[dPickedAlpha = " << dPickedAlpha << "]"
                           <<"[posPicking][X = " << posPicking.x() << " ]"
                           <<"[Y = " << posPicking.y() << " ][Z = " << posPicking.z() << "]";

                osg::ref_ptr< osg::Geometry > geometry;
                osg::ref_ptr< osg::Geode >  geode( new osg::Geode );

                double dSize  = m_crossSize->get();
                double dThickness = m_crossThickness->get();

                osg::ShapeDrawable* pBoxX = new osg::ShapeDrawable( new osg::Box( posPicking, dSize, dThickness, dThickness ) );
                osg::ShapeDrawable* pBoxY = new osg::ShapeDrawable( new osg::Box( posPicking, dThickness, dSize, dThickness ) );
                osg::ShapeDrawable* pBoxZ = new osg::ShapeDrawable( new osg::Box( posPicking, dThickness, dThickness, dSize ) );

                pBoxX->setColor( m_color->get() );
                pBoxY->setColor( m_color->get() );
                pBoxZ->setColor( m_color->get() );

                geode->addDrawable( pBoxX );
                geode->addDrawable( pBoxY );
                geode->addDrawable( pBoxZ );

                m_rootNode->remove( m_geode );
                m_geode = geode;
                m_shader->apply( m_geode );

                m_rootNode->clear();
                m_rootNode->insert( geode );
            }
        }
    }

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootNode );
}

void WMPickingDVR::pickHandler( WPickInfo pickInfo )
{
    //Not Right Mouse
    if( pickInfo.getMouseButton() != 2 )
    {
        return;
    }

    //Intersect with 3D
    boost::shared_ptr< WGraphicsEngine > graphicsEngine = WGraphicsEngine::getGraphicsEngine();
    boost::shared_ptr< WGEViewer > mainView = graphicsEngine->getViewerByName( "Main View" );

    osg::ref_ptr<osgViewer::Viewer> view = mainView->getView();
    osgUtil::LineSegmentIntersector::Intersections intersections;

    float fPosX = pickInfo.getPickPixel().x();
    float fPosY = pickInfo.getPickPixel().y();

    //Intersect
    bool bIntersected = view->computeIntersections( fPosX, fPosY, intersections, 0xFFFFFFFF );
    if( bIntersected )
    {
        osgUtil::LineSegmentIntersector::Intersection start= *intersections.begin();
        osgUtil::LineSegmentIntersector::Intersection end = *intersections.rbegin();

        this->m_posStart = start.getWorldIntersectPoint();
        this->m_posEnd  = end.getWorldIntersectPoint();

        this->m_bIntersected = true;

        //Notify Main
        this->m_propCondition->notify();
    }
}

void WMPickingDVR::updateModuleGUI( std::string strRenderMode )
{
    if( strRenderMode == WMPICKINGDVR_THRESHOLD )
    {
        m_alphaThreshold->setHidden( false );
    }
    else
    {
        m_alphaThreshold->setHidden( true );
    }
}

void WMPickingDVR::calculateDerivativesWYSIWYP( const std::vector<double>& values,
                                               std::vector<double>& vecFirstDerivative,
                                               std::vector<double>& vecSecondDerivative )
{
    //Fourth Order Finite Differencing by Daniel Gerlicher
    unsigned int n  = values.size();
    double dDeriv = 0.0;
    double dCoeff = 1.0 / 12.0;

    //Calculate first derivative
    for( unsigned int j = 0; j < n; j++ )
    {
        //Forward Diff
        if( j == 0 )
        {
            dDeriv = dCoeff * ( -25 * values[j]
                                + 48 * values[j+1]
                                - 36 * values[j+2]
                                + 16 * values[j+3]
                                - 3 * values[j+4] );
        }
        else if( j == 1 )
        {
            dDeriv = dCoeff * ( -3 * values[j-1]
                                - 10 * values[j]
                                + 18 * values[j+1]
                                - 6 * values[j+2]
                                + 1 * values[j+3] );
        }

        //Backward Diff
        else if( j == n - 1 )
        {
            dDeriv = dCoeff * ( 25 * values[j]
                                - 48 * values[j-1]
                                + 36 * values[j-2]
                                - 16 * values[j-3]
                                + 3 * values[j-4] );
        }
        else if( j == n - 2 )
        {
            dDeriv = dCoeff * ( +3 * values[j+1]
                                + 10 * values[j]
                                - 18 * values[j-1]
                                + 6 * values[j-2]
                                - 1 * values[j-3] );
        }

        //Center
        else
        {
            dDeriv = dCoeff * ( -1 * values[j+2]
                                + 8 * values[j+1]
                                - 8 * values[j-1]
                                + 1 * values[j-2] );
        }

        vecFirstDerivative.push_back( dDeriv );
    }

    //Calculate Second derivative, by applying the first derivative
    for( unsigned int j = 0; j < n; j++ )
    {
        //Forward Diff
        if( j == 0 )
        {
            dDeriv = dCoeff * ( -25 * vecFirstDerivative[j]
                                + 48 * vecFirstDerivative[j+1]
                                - 36 * vecFirstDerivative[j+2]
                                + 16 * vecFirstDerivative[j+3]
                                - 3 * vecFirstDerivative[j+4] );
        }
        else if( j == 1 )
        {
            dDeriv = dCoeff * ( -3 * vecFirstDerivative[j-1]
                                - 10 * vecFirstDerivative[j]
                                + 18 * vecFirstDerivative[j+1]
                                - 6 * vecFirstDerivative[j+2]
                                + 1 * vecFirstDerivative[j+3] );
        }

        //Backward Diff
        else if( j == n - 1 )
        {
            dDeriv = dCoeff * ( 25 * vecFirstDerivative[j]
                                - 48 * vecFirstDerivative[j-1]
                                + 36 * vecFirstDerivative[j-2]
                                - 16 * vecFirstDerivative[j-3]
                                + 3 * vecFirstDerivative[j-4] );
        }
        else if( j == n - 2 )
        {
            dDeriv = dCoeff * ( +3 * vecFirstDerivative[j+1]
                                + 10 * vecFirstDerivative[j]
                                - 18 * vecFirstDerivative[j-1]
                                + 6 * vecFirstDerivative[j-2]
                                - 1 * vecFirstDerivative[j-3] );
        }

        //Center
        else
        {
            dDeriv = dCoeff * ( -1 * vecFirstDerivative[j+2]
                                + 8 * vecFirstDerivative[j+1]
                                - 8 * vecFirstDerivative[j-1]
                                + 1 * vecFirstDerivative[j-2] );
        }

        vecSecondDerivative.push_back( dDeriv );
    }
}
