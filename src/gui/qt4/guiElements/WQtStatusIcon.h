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

// TESTING
#include <iostream>

/**
 * Circle with different colors
 **/
class WQtStatusIcon : public QWidget
{
    Q_OBJECT

    public:
        /**
         * constructor
         *
         * \param color the color of the icon
         * \param parent the parent widget
         **/
        WQtStatusIcon( QColor color, QWidget *parent = 0 ) :
             QWidget( parent )
        {
            m_color = color ;
            //std::cout << this << std::endl << &m_color << std::endl;
        }


        ~WQtStatusIcon()
        {
        }

        /**
         * to change the color
         *
         * \param color the new color
         **/
        void setColor( QColor color )
        {
            //std::cout << this << std::endl << &m_color << std::endl;
            m_color = color;
        }

        /**
         * returns the color
         **/
        QColor getColor() const
        {
            //std::cout << this << std::endl << &m_color << std::endl;
            return m_color;
        }

    protected:
        /**
         * paints the circle with the color
         **/
        virtual void paintEvent( QPaintEvent* )
        {
            QPainter painter( this );
            painter.setRenderHint( QPainter::Antialiasing );
            painter.setBrush( QBrush( m_color ) );
            painter.drawEllipse( 2, 2, 14, 14 );
        }

    private:
        /**
         * Color of the object
         **/
        QColor m_color; // TODO needed ?

};

#endif  // WQTSTATUSICON_H
