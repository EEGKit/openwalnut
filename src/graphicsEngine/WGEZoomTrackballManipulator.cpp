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

#include "WGEZoomTrackballManipulator.h"

WGEZoomTrackballManipulator::WGEZoomTrackballManipulator():
    TrackballManipulator()
{
    m_zoom = 1.0;
}

void WGEZoomTrackballManipulator::setByMatrix( const osg::Matrixd& matrix )
{
    m_zoom = 1.0 / matrix.getScale()[0];
    TrackballManipulator::setByMatrix( matrix );
}

osg::Matrixd WGEZoomTrackballManipulator::getMatrix() const
{
    return osg::Matrixd::scale( 1.0 / m_zoom, 1.0 / m_zoom, 1.0 ) * TrackballManipulator::getMatrix();
}

osg::Matrixd WGEZoomTrackballManipulator::getInverseMatrix() const
{
    return TrackballManipulator::getInverseMatrix() * osg::Matrixd::scale( m_zoom, m_zoom, 1.0 );
}

void WGEZoomTrackballManipulator::home( double currentTime )
{
    m_zoom = 1.0;
    TrackballManipulator::home( currentTime );
}

bool WGEZoomTrackballManipulator::handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us )
{
    if( ea.getEventType() == osgGA::GUIEventAdapter::SCROLL )
    {
        double zoomDelta;
        switch( ea.getScrollingMotion() )
        {
            case osgGA::GUIEventAdapter::SCROLL_UP:
                zoomDelta = -0.05;
                break;
            case osgGA::GUIEventAdapter::SCROLL_DOWN:
                zoomDelta = 0.05;
                break;
            case osgGA::GUIEventAdapter::SCROLL_2D:
                zoomDelta = -0.05 / 120.0 * ea.getScrollingDeltaY();
                break;
            // case osgGA::GUIEventAdapter::SCROLL_LEFT:
            // case osgGA::GUIEventAdapter::SCROLL_RIGHT:
            // case osgGA::GUIEventAdapter::SCROLL_NONE:
            default:
                // do nothing
                zoomDelta = 0.0;
                break;
        }

        if (zoomDelta != 0.0)
        {
            m_zoom *= 1.0 + zoomDelta;
            us.requestRedraw();
        }

        us.requestContinuousUpdate( false );
        _thrown = false;
        return true;
    }
    else
    {
        return TrackballManipulator::handle( ea, us );
    }
}
