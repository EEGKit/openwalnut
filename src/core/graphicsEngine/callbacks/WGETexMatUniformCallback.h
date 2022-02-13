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

#ifndef WGETEXMATUNIFORMCALLBACK_H
#define WGETEXMATUNIFORMCALLBACK_H

#include <osg/TexMat>
#include <osg/Uniform>

/**
 * Callback for the texture matrix.
 */
class WGETexMatUniformCallback: public osg::Uniform::Callback
{
public: // NOLINT
    /**
     * Constructor.
     * 
     * \param texMat the texture matrix this callback is applied to.
     */
    explicit WGETexMatUniformCallback( osg::TexMat* texMat );

    /**
     * operator () - called during the traversal
     * 
     * \param uniform the uniform
     * \param nv the node visitor
     */
    void operator()( osg::Uniform* uniform, osg::NodeVisitor* nv );

    /**
     * The texture matrix this callback is applied to.
     */
    osg::TexMat* m_texMat;
};

#endif  // WGETEXMATUNIFORMCALLBACK_H
