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

#ifndef WQTTREEWIDGET_H
#define WQTTREEWIDGET_H

#include <QtGui/QTreeWidget>

/**
 * tree widget for the dataset browser
 */
class WQtTreeWidget  : public QTreeWidget
{
    Q_OBJECT

public:
    /**
     * default constructor
     * \param parent the parent widget of this widget, i.e. the widget that manages this widget
     */
    explicit WQtTreeWidget( QWidget* parent = 0 );

    /**
     * destructor
     */
    virtual ~WQtTreeWidget();

public slots:
    /**
     * change order of items, move currently selected item down
     */
    void moveTreeItemDown();

    /**
     * change order of items, move currently selected item up
     */
    void moveTreeItemUp();
protected:
private:
};

#endif  // WQTTREEWIDGET_H
