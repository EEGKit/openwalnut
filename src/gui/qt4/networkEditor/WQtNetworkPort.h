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

#ifndef WQTNETWORKPORT_H
#define WQTNETWORKPORT_H

#include <QtGui/QGraphicsRectItem>
#include <QtGui/QGraphicsLineItem>

#include "WQtNetworkArrow.h"

// forward declaration
//class WQtNetworkArrow;
//class WQtNetworkInputPort;
//class WQtNetworkOutputPort;

/**
 * Abstract class to distinguish between input- and output ports from a module.
 * This class handels if a connection between two ports is possible or not.
 */
class WQtNetworkPort : public QGraphicsRectItem
{
public:

    WQtNetworkPort();

    ~WQtNetworkPort();

    /**
     * Reimplementation from QGraphicsItem
     * \return the type of the item as int
     */
    virtual int type() const = 0;

    /**
     * The position of every arrow connected with this port is updating its position in the
     * scene.
     */
    virtual void updateArrows() = 0;

    /**
     * Removes a specific arrow.
     * \param arrow an arrow
     */
    virtual void removeArrow( WQtNetworkArrow *arrow ) = 0;

    /**
     * Removes all connected arrows.
     */
    virtual void removeArrows();

    /**
     * Calculates the position inside a item for each port to get a correct
     * alignment
     * \param size the total number of ports ( distinguished by in- and
     *          outport
     * \param portNumber the number of the current port (distinguised by in-
     *          and outport
     * \param rect the rect of the parent item
     * \param outPort is it an in- or outport
     */
    void alignPosition( int size, int portNumber, QRectF rect, bool outPort );

    /**
     * Set the type of the port.
     * \param type true if out / false if in
     */
    virtual void setOutPort( bool type ) = 0;

    /**
     * Returns the porttype - true if outputport, false if inputport
     * \return is it a outport?
     */
    virtual bool isOutPort() = 0;

    /**
     * Returns the portname.
     * \return portname
     */
    virtual QString getPortName() = 0;

    /**
     * Set the Name
     * \param str portname as string
     */
    virtual void setPortName( QString str ) = 0;

    /**
     * Return the number of connections
     *
     * \return number of connections
     */
    virtual int getNumberOfArrows() = 0;

    /**
     * Adds an arrow to the port
     *
     * \param arrow the arrow to add
     */
    virtual void addArrow( WQtNetworkArrow* arrow ) = 0;

    /**
     * Get a QList of all arrows connected to this port
     *
     * \return a QList of WQtNetworkArrows
     */
    virtual QList< WQtNetworkArrow* > getArrowList() = 0;

protected:

    /**
     * Start drawing an arrow temporary.
     *
     * \param mouseEvent the mouse event
     */
    void mousePressEvent( QGraphicsSceneMouseEvent *mouseEvent );

    /**
     * Updates the temporary arrows endpoint.
     * Arrow is colored green when connection possible, red if no connection
     * is possible, or black when cursor doesent covers an WQtNetworkPort.
     *
     * \param mouseEvent the mouse event
     */
    void mouseMoveEvent( QGraphicsSceneMouseEvent *mouseEvent );

    /**
     * Send a connect request to kernel when start- and endport are
     * connectable
     *
     * \param mouseEvent the mouse event
     */
    void mouseReleaseEvent( QGraphicsSceneMouseEvent *mouseEvent );

    QList< WQtNetworkArrow *> m_arrows; //!< the connected arrows

private:

    QGraphicsLineItem * line; //!< the temporary line when you connect two ports
};
#endif  // WQTNETWORKPORT_H
