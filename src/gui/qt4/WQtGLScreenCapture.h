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

#ifndef WQTGLSCREENCAPTURE_H
#define WQTGLSCREENCAPTURE_H

#include <limits>

#include <boost/signals2.hpp>

#include <QtGui/QDockWidget>
#include <QtGui/QToolBox>
#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>

#include <osg/Camera>
#include <osg/Image>
#include <osg/RenderInfo>

#include "../../graphicsEngine/WGEScreenCapture.h"

#include "WIconManager.h"

class WMainWindow;

/**
 * This class is a screen recorder adapter in QT. It uses WGEScreenCapture and provides a nice widget around it.
 */
class WQtGLScreenCapture: public QDockWidget,
                          public WGEScreenCapture
{
    Q_OBJECT
public:

    /**
     * Creates a screen capture callback.
     */
    explicit WQtGLScreenCapture( WMainWindow* parent );

    /**
     * Destructor. Cleans up.
     */
    virtual ~WQtGLScreenCapture();

protected:

    /**
     * The function handles new images. Implement it.
     *
     * \param framesLeft how much frames to come
     * \param totalFrames the total number of frames until now
     * \param image the image
     */
    virtual void handleImage( size_t framesLeft, size_t totalFrames, osg::ref_ptr< osg::Image > image ) const;

    /**
     * Custom event dispatcher. Gets called by QT's Event system every time an event got sent to this widget. This event handler
     * processes several custom events, like WModuleAssocEvent.
     *
     * \note QT Doc says: use event() for custom events.
     * \param event the event that got transmitted.
     *
     * \return true if the event got handled properly.
     */
    virtual bool event( QEvent* event );

private slots:

    /**
     * Initiates taking a screenshot
     */
    void screenShot();

    /**
     * Slot triggered by m_moveRecButton
     */
    void startRec();

    /**
     * Slot triggered by m_moveStopButton
     */
    void stopRec();

    /**
     * Slot triggered by m_configFrameResetButton
     */
    void resetFrames();

private:

    /**
     * Called by the screencapture callback to notify this widget about recording
     */
    void recCallback();

    /**
     * Recording - callback connection
     */
    boost::signals2::connection m_recordConnection;

    /**
     * The toolbox containing all the stuff
     */
    QToolBox* m_toolbox;

    /**
     * The icons used here.
     */
    WIconManager* m_iconManager;

    /**
     * The configuration widget. It contains the common config stuff
     */
    QWidget* m_configWidget;

    /**
     * The filename for the screenshot
     */
    QLineEdit* m_configFileEdit;

    /**
     * Shows recorded frames.
     */
    QLabel* m_configFrameLabel;

    /**
     * Triggers frame counter reset.
     */
    QPushButton* m_configFrameResetButton;

    /**
     * Widget containing all the widgets to control movie recording
     */
    QWidget* m_screenshotWidget;

    /**
     * The trigger which triggers the screenshot.
     */
    QPushButton* m_screenshotButton;

    /**
     * Shows recorded time in movie-time.
     */
    QLabel* m_movieTimeLabel;

    /**
     * Widget containing all the widgets to control movie recording
     */
    QWidget* m_movieWidget;

    QPushButton* m_movieRecButton;  //!< record button
    QPushButton* m_movieStopButton;  //!< stop button
};

#endif  // WQTGLSCREENCAPTURE_H
