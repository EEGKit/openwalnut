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

#include <list>
#include <vector>

#include <osg/ref_ptr>

#include "../../common/exceptions/WTypeMismatch.h"
#include "../../common/WLogger.h"
#include "../../graphicsEngine/WGEGroupNode.h"
#include "../../dataHandler/WDataSetScalar.h"
#include "../../dataHandler/WGridRegular3D.h"
#include "WSPSliceBuilder.h"

WSPSliceBuilder::WSPSliceBuilder( ProbTractList probTracts, WPropGroup sliceGroup, std::vector< WPropGroup > colorMap )
    : m_slicePos( 3 ),
      m_probTracts( probTracts ),
      m_colorMap( colorMap ) // yes this is a COPY of the vector but WPropGroup is a boost::shared_ptr so updates will propagate!
{
    m_slicePos[2] = sliceGroup->findProperty( "Axial Position" )->toPropInt();
    m_slicePos[1] = sliceGroup->findProperty( "Coronal Position" )->toPropInt();
    m_slicePos[0] = sliceGroup->findProperty( "Sagittal Position" )->toPropInt();

    checkAndExtractGrids();
}

WSPSliceBuilder::~WSPSliceBuilder()
{
    // since we are having virtual member functions we also need a virtual destructor
}

// helper functions only to be DRY
namespace
{
    /**
     * Try a cast to WGridRegular3D, and return the cast result if it was successful, otherwise throw an exception of
     * WTypeMismatched.
     *
     * \param dataset The dataset of which the grid is taken from to check.
     *
     * \return The grid of the dataset casted to WGridRegular3D.
     */
    boost::shared_ptr< const WGridRegular3D > ensureWGridRegular3D( boost::shared_ptr< const WDataSetScalar > dataset )
    {
        boost::shared_ptr< const WGridRegular3D > result = boost::shared_dynamic_cast< WGridRegular3D >( dataset->getGrid() );
        if( !result )
        {
            wlog::error( "WSPSliceBuilder" ) << "Cast to WGridRegular3D failed.";
            throw WTypeMismatch( "WSPSliceBuilder::extractGrid(): WGridRegular3D expected, but cast failed." );
        }
        return result;
    }
}

void WSPSliceBuilder::checkAndExtractGrids()
{
    if( m_probTracts.empty() )
    {
        m_grid.reset();
    }
    else
    {
        try
        {
            m_grid = ensureWGridRegular3D( m_probTracts.front() );

            for( ProbTractList::const_iterator cit = m_probTracts.begin(); cit != m_probTracts.end(); ++cit )
            {
                boost::shared_ptr< const WGridRegular3D > grid = ensureWGridRegular3D( *cit );
                // TODO(math): ensure that each WGridRegular3D is the same once the operator== is available for WGridRegular3D
                // grid == m_grid
            }
        }
        catch( const WTypeMismatch& e )
        {
            wlog::error( "WSPSliceBuilder" ) << "At least one probabilistic tractogram has a grid which is not castable to WGridRegluar3D";
            throw e;
        }
    }
}

WColor WSPSliceBuilder::colorMap( size_t probTractNum ) const
{
    return m_colorMap.at( probTractNum )->findProperty( "Color" )->toPropColor()->get();
}

//osg::ref_ptr< WGEGroupNode > WSPSliceBuilder::generateSlice( size_t /* sliceNum */ ) const
//{
//    wlog::error( "WSPSliceBuilder" ) << "Bug: This virtual function should never be called, I should consider making this abstract";
//    return osg::ref_ptr< WGEGroupNode >( new WGEGroupNode );
//}
//
//void WSPSliceBuilder::preprocess()
//{
//    wlog::error( "WSPSliceBuilder" ) << "Bug: This virtual function should never be called, I should consider making this abstract";
//}
//

bool WSPSliceBuilder::alphaBelowThreshold( const WColor& c, const double threshold ) const
{
    return c[3] < threshold;
}

osg::ref_ptr< osg::Vec4Array > WSPSliceBuilder::computeColorsFor( const osg::Vec3& pos ) const
{
    osg::ref_ptr< osg::Vec4Array > result( new osg::Vec4Array );
    result->reserve( m_probTracts.size() );

    size_t interpolationFailures = 0;
    size_t probTractNum = 0;
    // for each probabilisitc tractogram look up if its probability at this vertex is below a certain threshold or not
    for( ProbTractList::const_iterator probTract = m_probTracts.begin(); probTract != m_probTracts.end(); ++probTract, ++probTractNum )
    {
        bool success = false;
        double probability = ( *probTract )->interpolate( wmath::WPosition( pos ), &success );
        if( ( *probTract )->getMax() > 10 )
        {
            // Todo(math): Solve this hack in a better way!
            // the probability is clearly not 0..1 distributed, we assume 0..255 instead.
            // We compute probability:
            probability /= 255.0;
        }
        if( success )
        {
            WColor c = colorMap( probTractNum );
            c[3] = static_cast< float >( probability );
            result->push_back( c );
        }
        else
        {
            ++interpolationFailures;
        }
    }

    if( interpolationFailures > 0 )
    {
        wlog::warn( "WSPSliceBuilderTracts" ) << "While computing colors for the probabilistic tracts, the interpolation of probabilistic"
            " tractograms failed: " << interpolationFailures << " many times.";
    }
    return result;
}
