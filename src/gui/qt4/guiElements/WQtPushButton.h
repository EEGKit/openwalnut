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

#ifndef WQTPUSHBUTTON_H
#define WQTPUSHBUTTON_H

#include <string>

#include <QtGui/QPushButton>

/**
 * Implements a QPushButton with a boost signal for convenience.
 */
class WQtPushButton : public QPushButton
{
    Q_OBJECT

public:
    /**
     * standard constructor
     * \param icon The icon being displayed on the button.
     * \param name a name for the button. This name is sometimes used to identify signals.
     * \param parent the widget that takes care of the current widget.
     * \param label The optional label that will be displayed besides the icon.
     */
    explicit WQtPushButton( QIcon icon, QString name, QWidget* parent = 0, QString label = "" );

    /**
     * destructor
     */
    virtual ~WQtPushButton();

    /**
     * Setter for name.
     *
     * \param name The new name of the button
     */
    void setName( QString name );

    /**
     * Getter for this button's name.
     *
     * \return the name
     */
    QString getName();

private:

    QString m_name; //!< name of the push button

public slots:

    /**
     * Slot getting called when the button got pressed.
     */
    void emitPressed();

signals:

    /**
     * Signal whenever the button got pressed.
     *
     * \param name the name of the button.
     */
    void pushButtonPressed( QString name );

    /**
     * Signals whenever the button got toggled.
     *
     * \param name the name of the button.
     * \param value true if toggled.
     */
    void pushButtonToggled( QString name, bool value );
};

#endif  // WQTPUSHBUTTON_H
