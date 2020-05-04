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

// Some rules to the inclusion of headers:
//  * Ordering:
//    * C Headers
//    * C++ Standard headers
//    * External Lib headers (like OSG or Boost headers)
//    * headers of other classes inside OpenWalnut
//    * your own header file

#include "OpenVRDrawCallback.h"

#include <osgViewer/GraphicsWindow>
#include <osgViewer/Renderer>

void OpenVRPreDrawCallback::operator()( osg::RenderInfo &renderInfo ) const
{
    m_textureBuffer->onPreRender( renderInfo );
}

void OpenVRPostDrawCallback::operator()( osg::RenderInfo &renderInfo ) const
{
    m_textureBuffer->onPostRender( renderInfo );
}

void OpenVRInitialDrawCallback::operator()( osg::RenderInfo &renderInfo ) const
{
    osg::GraphicsOperation *graphicsOperation =
        renderInfo.getCurrentCamera()->getRenderer();
    osgViewer::Renderer *renderer =
        dynamic_cast<osgViewer::Renderer *>( graphicsOperation );
    if (renderer != nullptr)
    {
        // Disable normal OSG FBO camera setup because it will undo the MSAA FBO
        // configuration.
        renderer->setCameraRequiresSetUp( false );
    }
}