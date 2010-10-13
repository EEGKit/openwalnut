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

#ifndef WTUBEGEOMETRY_H
#define WTUBEGEOMETRY_H

#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/thread/thread.hpp>

#include <osg/Drawable>
#include <osg/ShapeDrawable>
#include <osg/Group>
#include <osg/Geometry>

#include "../../../graphicsEngine/WShader.h"
#include "../../WExportKernel.h"

class WDataSetFibers;

/**
 * Module for drawing fibers
 *
 * \ingroup modules
 */
class OWKERNEL_EXPORT WTubeGeometry : public osg::Geometry
{
public:
    WTubeGeometry();

	/**
	 * I can't say much about the methods below, but OSG seems to expect
	 * that we implement them.
	 *
	 * \param pg
	 * \param copyop
	 */
    WTubeGeometry( const WTubeGeometry& pg, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY );

	/**
	 * no clue why it's here and wehat it does
	 */
    virtual osg::Object* cloneType() const;

	/**
	 * clones it
	 *
	 * \param copyop
	 */
    virtual osg::Object* clone( const osg::CopyOp& copyop ) const;

	/** Draw Geometry directly ignoring an OpenGL display list which could be attached.
	  * This is the internal draw method which does the drawing itself,
	  * and is the method to override when deriving from Geometry for user-drawn objects.
      * \param renderInfo
	*/
    virtual void drawImplementation( osg::RenderInfo& renderInfo ) const;

    /**
     * sets the uniforms to inform the shader about the viewport size
     *
     * \param uniformViewportHeight
     * \param uniformViewportWidth
     */
    void setViewPortUniform( osg::ref_ptr<osg::Uniform> uniformViewportHeight, osg::ref_ptr<osg::Uniform> uniformViewportWidth );

	/**
	 * sets the dataset pointer which provides access to the necessary array
	 *
	 * \param dataset
	 */
    void setDataset( boost::shared_ptr< const WDataSetFibers > dataset );
    boost::shared_ptr< const WDataSetFibers > m_dataset; //!< stores pointer to dataset
    boost::shared_ptr< std::vector< size_t > > m_startIndices; //!< stores pointer to start indices

    /**
     * sets the start indices pointer for the start indices of the fibers
     *
     * \param startIndices
     */
    void setStartIndices( boost::shared_ptr< std::vector< size_t > > startIndices );
    osg::ref_ptr<osg::Uniform> m_uniformViewportHeight; //!< uniform to inform shader of viewport height
    osg::ref_ptr<osg::Uniform> m_uniformViewportWidth; //!< uniform to inform shader of viewport width
};
#endif  // WTUBEGEOMETRY_H
