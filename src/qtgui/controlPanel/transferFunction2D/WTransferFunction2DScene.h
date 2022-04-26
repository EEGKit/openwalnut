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

#ifndef WTRANSFERFUNCTION2DSCENE_H
#define WTRANSFERFUNCTION2DSCENE_H

#include "QGraphicsScene"

class WTransferFunction2DGUIWidget;

/**
 * The scene for our visualization of the 2D transfer function editor.
 */
class WTransferFunction2DScene : public QGraphicsScene
{
public:
    /** type of base class for cleaner programming */
    typedef QGraphicsScene BaseClass;

    /**
     * Default constructor.
     *
     * \param parent the parent QObject, typically the QWidget
     */
    explicit WTransferFunction2DScene( QObject*parent =0 );

    /**
     * Default destructor
     */
    virtual ~WTransferFunction2DScene();
private:
    /** storage of the widget as this currently manages the data */
    WTransferFunction2DGUIWidget *myparent;
};

#endif  // WTRANSFERFUNCTION2DSCENE_H

