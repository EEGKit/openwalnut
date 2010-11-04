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

class WQtStatusIcon : public QWidget
{
    Q_OBJECT

    public:
        WQtStatusIcon( QColor color, QWidget *parent = 0 ) :
            QWidget( parent ), m_color( color )
        {
        }

        ~WQtStatusIcon()
        {
        }

        void setColor( QColor color )
        {
            m_color = color;
        }

        QColor getColor() const
        {
            return m_color;
        }

    protected:
        virtual void paintEvent( QPaintEvent* )
        {
            QPainter painter( this );
            painter.setRenderHint( QPainter::Antialiasing );
            painter.setBrush( QBrush( m_color ) );
            painter.drawEllipse( 0, 0, width(), height() );
        }

    private:
        QColor m_color; // TODO needed ?

};

#endif  // WQTSTATUSICON_H
