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
#include <vector>

#include "core/common/WAssert.h"
#include "core/common/WIOTools.h"
#include "core/common/WPropertyHelper.h"
#include "core/dataHandler/WDataHandler.h"
#include "core/dataHandler/WDataSet.h"
#include "core/dataHandler/WDataSetScalar.h"
#include "core/dataHandler/WDataSetSingle.h"
#include "core/dataHandler/WDataSetTimeSeries.h"
#include "core/dataHandler/WDataSetVector.h"
#include "core/dataHandler/WDataTexture3D.h"
#include "core/dataHandler/WEEG2.h"
#include "core/dataHandler/WSubject.h"
#include "core/dataHandler/exceptions/WDHException.h"
#include "core/graphicsEngine/WGEColormapping.h"
#include "core/kernel/WDataModuleInputFile.h"
#include "core/kernel/WDataModuleInputFilterFile.h"
#include "core/kernel/WModuleOutputData.h"

#ifdef WBIOSIG_ENABLED
    #include "io/WReaderBiosig.h"
#endif
#include "io/WReaderEEGASCII.h"
#include "io/WReaderNIfTI.h"
#include "io/WReaderELC.h"
#include "io/WReaderFiberVTK.h"
#include "io/WReaderLibeep.h"
#include "io/WReaderVTK.h"
#include "io/WPagerEEGLibeep.h"
#include "io/WReaderClustering.h"

#include "WMData.h"
#include "WMData.xpm"

WMData::WMData():
    WDataModule(),
    m_isTexture(),
    m_transformNoMatrix( 4, 4 ),
    m_transformSForm( 4, 4 ),
    m_transformQForm( 4, 4 )
{
    // initialize members
}

WMData::~WMData()
{
    // cleanup
}

boost::shared_ptr< WModule > WMData::factory() const
{
    return boost::shared_ptr< WModule >( new WMData() );
}

const char** WMData::getXPMIcon() const
{
    return WMData_xpm;
}

const std::string WMData::getName() const
{
    return "Data Module";
}

const std::string WMData::getDescription() const
{
    return "This module encapsulates data.";
}

std::vector< WDataModuleInputFilter::ConstSPtr > WMData::getInputFilter() const
{
    std::vector< WDataModuleInputFilter::ConstSPtr > filters;

    // NOTE: plain extension. No wildcards or prefixing "."!
    filters.push_back( WDataModuleInputFilter::ConstSPtr( new WDataModuleInputFilterFile( "nii", "NIfTI files" ) ) );
    filters.push_back( WDataModuleInputFilter::ConstSPtr( new WDataModuleInputFilterFile( "nii.gz", "Compressed NIfTI files" ) ) );
#ifdef WBIOSIG_ENABLED
    filters.push_back( WDataModuleInputFilter::ConstSPtr( new WDataModuleInputFilterFile( "edf", "EEG files (BioSig)" ) ) );
#endif
    filters.push_back( WDataModuleInputFilter::ConstSPtr( new WDataModuleInputFilterFile( "cnt", "EEG files" ) ) );
    filters.push_back( WDataModuleInputFilter::ConstSPtr( new WDataModuleInputFilterFile( "asc", "EEG files" ) ) );
    filters.push_back( WDataModuleInputFilter::ConstSPtr( new WDataModuleInputFilterFile( "vtk", "VTK files, limited support" ) ) );
    filters.push_back( WDataModuleInputFilter::ConstSPtr( new WDataModuleInputFilterFile( "fib", "VTK Fiber files" ) ) );
    filters.push_back( WDataModuleInputFilter::ConstSPtr( new WDataModuleInputFilterFile( "fdg", "Cluster Files" ) ) );
    return filters;
}

void WMData::connectors()
{
    // initialize connectors
    m_output= boost::shared_ptr< WModuleOutputData< WDataSet > >( new WModuleOutputData< WDataSet >(
                shared_from_this(), "out", "A loaded dataset." )
            );

    // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
    addConnector( m_output );

    // call WModules initialization
    WDataModule::connectors();
}

void WMData::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    // Add standard datamodule props
    WDataModule::properties();
    // m_reloadTrigger->setHidden( true ); // reload not supported

    // properties
    m_dataName = m_infoProperties->addProperty( "Filename", "The filename of the dataset.", std::string( "" ) );
    m_dataType = m_infoProperties->addProperty( "Data type", "The type of the single data values.", std::string( "" ) );
    m_dataSetType = m_infoProperties->addProperty( "Dataset type", "The name of the dataset type.", std::string( "" ) );

    m_matrixSelectionsList = boost::shared_ptr< WItemSelection >( new WItemSelection() );
    m_matrixSelectionsList->addItem( "No matrix", "" );
    m_matrixSelectionsList->addItem( "sform", "" );
    m_matrixSelectionsList->addItem( "qform", "" );

    m_matrixSelection = m_properties->addProperty( "Transformation matrix",  "matrix",
            m_matrixSelectionsList->getSelectorFirst(), m_propCondition );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_matrixSelection );

    // use this callback for the other properties
    WPropertyBase::PropertyChangeNotifierType propertyCallback = boost::bind( &WMData::propertyChanged, this, _1 );
}

void WMData::propertyChanged( boost::shared_ptr< WPropertyBase > property )
{
    if( m_isTexture )
    {
        if( property == m_active )
        {
            // forward to texture
            m_dataSet->getTexture()->active()->set( m_active->get( true ) );
        }
    }
    else
    {
        if( property == m_active )
        {
            if( m_active->get() )
            {
                m_output->updateData( m_dataSet );
            }
            else
            {
                m_output->updateData( boost::shared_ptr< WDataSet >() );
            }
        }
    }
}

void WMData::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_propCondition );
    m_moduleState.add( m_reloadTriggered );
    m_oldDataSet = WDataSet::SPtr();

    ready();
    waitRestored();

    load();

    while( !m_shutdownFlag() )
    {
        m_moduleState.wait();
        if( m_shutdownFlag() )
        {
            break;
        }

        // Not supported.
        if( m_reloadTrigger->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
        {
            load();
            m_reloadTrigger->set( WPVBaseTypes::PV_TRIGGER_READY );
        }

        // change transform matrix (only if we have a dataset single which contains the grid)
        if( m_matrixSelection->changed() && m_dataSetAsSingle )
        {
            matrixUpdate();
        }
    }

    // remove dataset from datahandler
    updateColorMap(  boost::shared_ptr< WDataSet >() );
}

// TODO(wiebel): move this to some central place.
std::string WMData::getDataTypeString( boost::shared_ptr< WDataSetSingle > dss )
{
    std::string result;
    switch( (*dss).getValueSet()->getDataType() )
    {
        case W_DT_NONE:
            result = "none";
            break;
        case W_DT_BINARY:
            result = "binary (1 bit)";
            break;
        case W_DT_UNSIGNED_CHAR:
            result = "unsigned char (8 bits)";
            break;
        case W_DT_SIGNED_SHORT:
            result = "signed short (16 bits)";
            break;
        case W_DT_SIGNED_INT:
            result = "signed int (32 bits)";
            break;
        case W_DT_FLOAT:
            result = "float (32 bits)";
            break;
        case W_DT_COMPLEX:
            result = "complex";
            break;
        case W_DT_DOUBLE:
            result = "double (64 bits)";
            break;
        case W_DT_RGB:
            result = "RGB triple (24 bits)";
            break;
        case W_DT_ALL:
            result = "ALL (not very useful)";
            break;
        case W_DT_INT8:
            result = "signed char (8 bits)";
            break;
        case W_DT_UINT16:
            result = "unsigned short (16 bits)";
            break;
        case W_DT_UINT32 :
            result = "unsigned int (32 bits)";
            break;
        case W_DT_INT64:
            result = "int64";
            break;
        case W_DT_UINT64:
            result = "unsigned long long (64 bits)";
            break;
        case W_DT_FLOAT128:
            result = "float (128 bits)";
            break;
        case W_DT_COMPLEX128:
            result = "double pair (128 bits)";
            break;
        case W_DT_COMPLEX256:
            result = " long double pair (256 bits)";
            break;
        case W_DT_RGBA32:
            result = "4 byte RGBA (32 bits)";
            break;
        default:
            WAssert( false, "Unknow data type in getDataTypeString" );
    }
    return result;
}

namespace
{
    // helper which gets a WMatrix< double > and returns a WMatrix4d
    WMatrix4d WMatrixDoubleToWMatrix4d( const WMatrix< double >& matrix )
    {
        WAssert( matrix.getNbRows() == 4, "" );
        WAssert( matrix.getNbCols() == 4, "" );
        WMatrix4d result;
        for( size_t i = 0; i < 4; ++i )
        {
            for( size_t j = 0; j < 4; ++j )
            {
                result( i, j ) = matrix( i, j );
            }
        }
        return result;
    }
}

void WMData::matrixUpdate()
{
    // incase that m_dataSet is not a WDataSetSingle e.g. WDataSetFibers, the cast might be NULL
    if( boost::dynamic_pointer_cast< WDataSetSingle >( m_dataSet ) )
    {
        debugLog() << "Matrix mode update.";

        // a new grid
        boost::shared_ptr< WGrid > newGrid;
        boost::shared_ptr< WGridRegular3D > oldGrid = boost::dynamic_pointer_cast< WGridRegular3D >( m_dataSetAsSingle->getGrid() );

        switch( m_matrixSelection->get( true ).getItemIndexOfSelected( 0 ) )
        {
        case 0:
            newGrid = boost::shared_ptr< WGrid >( new WGridRegular3D( oldGrid->getNbCoordsX(), oldGrid->getNbCoordsY(), oldGrid->getNbCoordsZ(),
                                                                      WGridTransformOrtho( m_transformNoMatrix ) ) );
            break;
        case 1:
            newGrid = boost::shared_ptr< WGrid >( new WGridRegular3D( oldGrid->getNbCoordsX(), oldGrid->getNbCoordsY(), oldGrid->getNbCoordsZ(),
                                                                      WGridTransformOrtho( m_transformSForm ) ) );
            break;
        case 2:
            newGrid = boost::shared_ptr< WGrid >( new WGridRegular3D( oldGrid->getNbCoordsX(), oldGrid->getNbCoordsY(), oldGrid->getNbCoordsZ(),
                                                                      WGridTransformOrtho( m_transformQForm ) ) );
            break;
        }

        // Update the m_dataSet
        m_dataSet = m_dataSetAsSingle->clone( newGrid );
        // Update textures? Keep trakc of old
        if( m_dataSet->isTexture() && m_dataSet->getTexture() )
        {
            m_dataSet->getTexture()->getProperties()->set( m_dataSetAsSingle->getTexture()->getProperties() );
        }
        m_dataSetAsSingle = boost::dynamic_pointer_cast< WDataSetSingle >( m_dataSet );

        // Remove all old props
        if( m_oldDataSet )
        {
            m_properties->removeProperty( m_oldDataSet->getProperties() );
            m_infoProperties->removeProperty( m_oldDataSet->getInformationProperties() );
            m_infoProperties->removeProperty( m_infoProperties->findProperty( "Transformations" ) );
        }

        // Add new props
        m_properties->addProperty( m_dataSet->getProperties() );
        m_infoProperties->addProperty( m_dataSet->getInformationProperties() );
        m_infoProperties->addProperty( getTransformationProperties() );

        // the clone() may have returned a zero-pointer, only update if it hasn't
        // this may happen if the clone() operation has not been implemented in the derived dataset class
        updateColorMap( m_dataSet );

        m_oldDataSet = m_dataSet;
    }

    m_output->updateData( m_dataSet ); // need to update output connector also in case of non WDataSetSingle, as this hasnt been done before
}

boost::shared_ptr< WProperties > WMData::getTransformationProperties() const
{
    boost::shared_ptr< WProperties > result( new WProperties( "Transformations", "Availabe transformation matrices for data-set " ) );
    WPropGroup transformation = result->addPropertyGroup( "Transformation",
                                                          "The transformation of this grid." );
    WPropMatrix4X4 noMatrixTransformation = result->addProperty( "No matrix transformation",
                                                                 "The no matrix transformation for this data set.",
                                                                 WMatrixDoubleToWMatrix4d( m_transformNoMatrix ) );
    WPropMatrix4X4 sformMatrixTransformation = result->addProperty( "sform matrix transformation",
                                                                    "The sform matrix transformation for this data set.",
                                                                    WMatrixDoubleToWMatrix4d( m_transformSForm ) );
    WPropMatrix4X4 qformMatrixTransformation = result->addProperty( "qform matrix transformation",
                                                                    "The qform matrix transformation for this data set.",
                                                                    WMatrixDoubleToWMatrix4d( m_transformQForm ) );
    return result;
}

void WMData::updateColorMap( boost::shared_ptr< WDataSet > dataSet )
{
    // remove dataset from datahandler
    if( m_oldColormap )
    {
        m_properties->removeProperty( m_oldColormap->getProperties() );
        m_infoProperties->removeProperty( m_oldColormap->getInformationProperties() );
        WGEColormapping::deregisterTexture( m_oldColormap );
        m_oldColormap = osg::ref_ptr< WDataTexture3D >();
    }

    if( dataSet )
    {
        // remove dataset from datahandler
        if( dataSet->isTexture() )
        {
            if( !getSuppressColormaps() )
            {
                WGEColormapping::registerTexture( dataSet->getTexture(), m_runtimeName->get() );
            }
            m_properties->addProperty( dataSet->getTexture()->getProperties() );
            m_infoProperties->addProperty( dataSet->getTexture()->getInformationProperties() );

            m_oldColormap = dataSet->getTexture();
        }
    }
}

void WMData::load()
{
    // Get the input
    WDataModuleInputFile::SPtr inputFile = getInputAs< WDataModuleInputFile >();
    if( !inputFile )
    {
        throw WModuleException( "Data modules cannot be used directly." );
    }
    std::string fileName = inputFile->getFilename().string();

    m_transformNoMatrix.makeIdentity();
    m_transformSForm.makeIdentity();
    m_transformQForm.makeIdentity();

    debugLog() << "Loading data from \"" << fileName << "\".";
    m_dataName->set( fileName );

    // load it now
    std::string suffix = getSuffix( fileName );
    if( suffix == ".nii" || ( suffix == ".gz" && ::nifti_compiled_with_zlib() ) )
    {
        if( suffix == ".gz" )  // it may be a NIfTI file too
        {
            boost::filesystem::path p( fileName );
            p.replace_extension( "" );

            if( boost::filesystem::exists( p ) && !boost::filesystem::is_directory( p ) )
            {
                warnLog() << "Loading file " << fileName << " and a file with the name " << p <<
                            " was found. This may lead to problems loading the data due to an issue with the niftiio-lib.";
            }

            suffix = getSuffix( p.string() );
            WAssert( suffix == ".nii", "Currently only nii files may be gzipped." );
        }

        WReaderNIfTI niiLoader( fileName );
        m_dataSet = niiLoader.load();
        m_transformNoMatrix = niiLoader.getStandardTransform();
        m_transformSForm = niiLoader.getSFormTransform();
        m_transformQForm = niiLoader.getQFormTransform();

        m_isTexture = m_dataSet->isTexture();

        boost::shared_ptr< WDataSetSingle > dss = boost::dynamic_pointer_cast< WDataSetSingle >( m_dataSet );
        if( dss )
        {
            m_dataType->set( getDataTypeString( dss ) );
            if( dss->isTexture() )
            {
                switch( (*dss).getValueSet()->getDataType() )
                {
                    case W_DT_UNSIGNED_CHAR:
                    case W_DT_INT16:
                    case W_DT_UINT16:
                    case W_DT_UINT32:
                    case W_DT_SIGNED_INT:
                        m_dataSet->getTexture()->colormap()->set(
                            m_dataSet->getTexture()->colormap()->get().newSelector( WItemSelector::IndexList( 1, 0 ) )
                        );
                        break;
                    case W_DT_FLOAT:
                    case W_DT_DOUBLE:
                        if( boost::dynamic_pointer_cast< WDataSetVector >( m_dataSet ) )
                        {
                            m_dataSet->getTexture()->colormap()->set(
                                m_dataSet->getTexture()->colormap()->get().newSelector( WItemSelector::IndexList( 1, 6 ) )
                            );
                            m_dataSet->getTexture()->interpolation()->set( false );
                        }
                        else
                        {
                            m_dataSet->getTexture()->colormap()->set(
                                m_dataSet->getTexture()->colormap()->get().newSelector( WItemSelector::IndexList( 1, 5 ) )
                            );
                        }
                        break;
                    default:
                        WAssert( false, "Could not load \"" + fileName + "\". Reason: unknown data type in Data module" );
                }
            }
        }
    }
#ifdef WBIOSIG_ENABLED
    else if( suffix == ".edf" ) // NOLINT: An else should appear after closing } in previous line
    {
        WReaderBiosig biosigLoader( fileName );
        m_dataSet = biosigLoader.load();
    }
#endif
    else if( suffix == ".asc" ) // NOLINT: An else should appear after closing } in previous line
    {
        WReaderEEGASCII eegAsciiLoader( fileName );
        m_dataSet = eegAsciiLoader.load();
    }
    else if( suffix == ".cnt" )
    {
        boost::shared_ptr< WPagerEEG > pager( new WPagerEEGLibeep( fileName ) );

        std::string elcFileName = fileName;
        elcFileName.resize( elcFileName.size() - 3 ); // drop suffix
        elcFileName += "elc"; // add new suffix
        WReaderELC elcReader( elcFileName );
        boost::shared_ptr< WEEGPositionsLibrary > eegPositionsLibrary = elcReader.read();

        m_dataSet = boost::shared_ptr< WEEG2 >( new WEEG2( pager, eegPositionsLibrary ) );
    }
    else if( suffix == ".fib" )
    {
        WReaderFiberVTK fibReader( fileName );
        m_dataSet = fibReader.read();
    }
    else if( suffix == ".fdg" )
    {
        WReaderClustering clusterReader( fileName );
        m_dataSet = clusterReader.read();
    }
    else if( suffix == ".vtk" )
    {
        WReaderVTK vtkReader( fileName );
        m_dataSet = vtkReader.read();
    }
    else
    {
        throw WDHException( std::string( "Could not load \"" + fileName + "\". Reason: unknown file type: \"" + suffix + "\"" ) );
    }

    if( !m_dataSet )
    {
        throw WDHException( std::string( "Could not load \"" + fileName + "\". Possibly format not supported." ) );
    }

    debugLog() << "Loading data done.";

    // set the dataset name
    m_dataSetType->set( m_dataSet->getName() );

    // I am interested in the active property ( manually subscribe signal )
    m_active->getCondition()->subscribeSignal( boost::bind( &WMData::propertyChanged, this, m_active ) );

    // notify
    m_dataSetAsSingle = boost::dynamic_pointer_cast< WDataSetSingle >( m_dataSet );

    // Update matrix
    matrixUpdate();
}
