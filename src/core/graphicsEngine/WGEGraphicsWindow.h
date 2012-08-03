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

#ifndef WGEGRAPHICSWINDOW_H
#define WGEGRAPHICSWINDOW_H

#include <boost/shared_ptr.hpp>
#include <osgViewer/GraphicsWindow>

#include "WGraphicsEngineMode.h"

/**
 * Class managing a single graphics context and OSG GraphicsWindow.
 * \ingroup ge
 */
class WGEGraphicsWindow
{
public:
    /**
     * Default constructor.
     *
     * \param wdata the WindowData instance for the widget to use as render widget. NULL on Mac!
     * \param x X coordinate of widget where to create the context.
     * \param y Y coordinate of widget where to create the context.
     * \param width Width of the widget.
     * \param height Height of the Widget.
     * \exception WGEInitFailed thrown if initialization of graphics context or graphics window has failed.
     */
    WGEGraphicsWindow( osg::ref_ptr<osg::Referenced> wdata, int x, int y, int width, int height );

    /**
     * Destructor.
     */
    virtual ~WGEGraphicsWindow();

    /**
     * Getter for m_GraphicsWindow.
     *
     * \return the OSG GraphicsWindow instance.
     */
    osg::ref_ptr<osgViewer::GraphicsWindow> getGraphicsWindow();

protected:
    /**
     * OpenSceneGraph render window.
     */
    osg::ref_ptr<osgViewer::GraphicsWindow> m_GraphicsWindow;

#ifdef WGEMODE_MULTITHREADED
     /**
     * Creates a new OpenGL context in the calling thread. Needs to be called before any other OpenGL operation.
     *
     * \param x X coordinate of widget where to create the context.
     * \param y Y coordinate of widget where to create the context.
     * \param width Width of the widget.
     * \param height Height of the Widget.
     */
    void createContext( int x, int y, int width, int height );

    /**
     * OpenSceneGraph render context.
     */
    osg::ref_ptr<osg::GraphicsContext> m_GraphicsContext;

    /**
     * Widget window data.
     */
    osg::ref_ptr<osg::Referenced> m_WindowData;
#endif
private:
};

#endif  // WGEGRAPHICSWINDOW_H

