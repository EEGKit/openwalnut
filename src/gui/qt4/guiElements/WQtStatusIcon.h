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

#ifndef WQTSTATUSICON_H
#define WQTSTATUSICON_H

#include <QtGui/QWidget>
#include <QtGui/QPainter>

/**
 * Circle with different colors, used to indicate how severe the log events are.
 * Used by WQtStatusBar.
 **/
class WQtStatusIcon : public QWidget
{
    Q_OBJECT

    public:
        /**
         * constructor
         *
         * \param color the current color of the icon
         * \param parent the parent widget
         **/
        WQtStatusIcon( QColor color, QWidget *parent = 0 ) :
             QWidget( parent )
        {
            setMinimumSize( 15, 15 );
            m_color = color;
        }

        /**
         * destructor
         **/
        ~WQtStatusIcon()
        {
        }

        /**
         * Changes the current color of this item.
         *
         * \param color the new color
         **/
        void setColor( QColor color )
        {
            m_color = color;
            this->repaint();
        }

        /**
         * returns the current color of the item
         **/
        QColor getColor() const
        {
            return m_color;
        }

    protected:
        /**
         * Paints the circle with the current color.
         **/
        virtual void paintEvent( QPaintEvent* )
        {
            int size = 0;
            if( width() > height() )
            {
                size = height() - 2;
            }
            else
            {
                size = width() - 2;
            }
            QPainter painter( this );
            painter.setRenderHint( QPainter::Antialiasing );
            painter.setBrush( QBrush( m_color ) );
            painter.drawEllipse( 1, 1, size, size );
        }

    private:
        /**
         * The current color of the object.
         **/
        QColor m_color;
};

#endif  // WQTSTATUSICON_H
