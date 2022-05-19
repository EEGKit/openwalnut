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

#ifndef WPICKINFO_H
#define WPICKINFO_H

#include <stdint.h>

#include <string>
#include <utility>

#include "../common/math/linearAlgebra/WVectorFixed.h"
#include "../common/math/linearAlgebra/WPosition.h"
#include "../common/WDefines.h"


/**
 * Encapsulates info for pick action.
 */
class WPickInfo
{
public:
    /**
     * Different types of modifier keys.
     */
    enum modifierKey
    {
        NONE,
        SHIFT,
        STRG,
        ALT,
        WIN
    };

    /**
     * Different types of mouse buttons.
     */
    typedef enum
    {
        NOMOUSE,
        MOUSE_LEFT,
        MOUSE_RIGHT,
        MOUSE_MIDDLE,
        MOUSE4,
        MOUSE5
    }
    WMouseButton;

    /**
     * Creates an object with the needed information.
     * \param name name of picked object
     * \param viewerName name of the viewer
     * \param pickPosition position where object was hit
     * \param pixelCoords pixel coordinates of the mouse
     * \param modKey relevant modifier key pressed during the pick
     * \param mButton mouse button that initiated the pick
     * \param pickNormal normal at position where object was hit. (0,0,0) means not set.
     * \param wheelValue the value of the scroll wheel
     * \param pickPath Names of objects hit in pick
     */
    inline WPickInfo( std::string name,
                      std::string viewerName,
                      WPosition pickPosition,
                      std::pair< float, float > pixelCoords,
                      modifierKey modKey,
                      WMouseButton mButton = WPickInfo::MOUSE_LEFT,
                      WVector3d pickNormal = WVector3d(),
                      int32_t wheelValue = 0,
                      std::string pickPath = "" );

    /**
     * Creates an object with the empty name, zero position and no modkey.
     */
    inline WPickInfo();

    /**
     * Get the modifier key associated with the pick
     *
     * \return the mod key
     */
    inline modifierKey getModifierKey() const;

    /**
     * Get the mouse button associated with the pick
     *
     * \return the mouse button
     */
    inline WMouseButton getMouseButton() const;

    /**
     * Set the modifier key associated with the pick
     * \param modKey new modifier key
     */
    inline void setModifierKey( const modifierKey& modKey );

    /**
     * Set the modifier key associated with the pick
     * \param mButton new mouse button
     */
    inline void setMouseButton( const WMouseButton& mButton );


    /**
     * Get name of picked object.
     *
     * \return object name
     */
    inline std::string getName() const;

    /**
     * Get name of all picked objects in ray.
     *
     * \return Names
     */
    inline std::string getPickPath() const;

    /**
     * Set pick path string.
     *
     * \param path String representing the pick path
     */
    inline void setPickPath( std::string path );

    /**
     * Add object to pick path.
     *
     * \param name Name of new object in path.
     */
    inline void appendToPickPath( std::string name );

    /**
     * Get name of the viewer.
     *
     * \return viewer name
     */
    inline std::string getViewerName() const;

    /**
     * Get position where object was hit.
     *
     * \return the pick position
     */
    inline WPosition getPickPosition() const;

    /**
     * Get normal at position where object was hit.
     *
     * \return pick normal
     */
    inline WVector3d getPickNormal() const;

    /**
     * Returns the picked pixel coordinates in screen-space.
     *
     * \return the coordinates
     */
    inline WVector2d getPickPixel() const;

    /**
     * Returns an integer denoting the wheel movement. If the value gets smaller, the wheel scrolled down.
     *
     * \return the value.
     */
    inline int32_t getScrollWheel() const;

    /**
     * Tests two pick infos for equality
     * \param rhs right hand side of comparison
     *
     * \return true if equal
     */
    inline bool operator==( WPickInfo rhs ) const;

    /**
     * Tests two pick infos for inequality
     *
     * \param rhs right hand side of comparison
     *
     * \return true if not equal
     */
    inline bool operator!=( WPickInfo rhs ) const;

protected:
private:
    std::string m_name; //!< Name of picked object.
    std::string m_viewerName; //!< Name of the viewer
    WPosition m_pickPosition; //!< Position where object was hit.
    std::pair< float, float > m_pixelCoords; //!< Pixel coordinates of the mouse.
    modifierKey m_modKey; //!< Modifier key associated with the pick
    WMouseButton m_mouseButton; //!< Which mouse button was used for the pick
    WVector3d m_pickNormal; //!< Normal at position where object was hit.
    int32_t m_scrollValue; //!< The scroll wheel value.
    std::string m_pickPath; //!< Name of all picked objects along path.
};

WPickInfo::WPickInfo( std::string name,
                      std::string viewerName,
                      WPosition pickPosition,
                      std::pair< float, float > pixelCoords,
                      modifierKey modKey,
                      WMouseButton mButton,
                      WVector3d pickNormal,
                      int32_t wheelValue,
                      std::string pickPath ) :
    m_name( name ),
    m_viewerName( viewerName ),
    m_pickPosition( pickPosition ),
    m_pixelCoords( pixelCoords ),
    m_modKey( modKey ),
    m_mouseButton( mButton ),
    m_pickNormal( pickNormal ),
    m_scrollValue( wheelValue ),
    m_pickPath( pickPath )
{
}

WPickInfo::WPickInfo() :
    m_name( "" ),
    m_viewerName( "" ),
    m_pickPosition( WPosition() ),
    m_pixelCoords( std::make_pair( 0.0, 0.0 ) ),
    m_modKey( WPickInfo::NONE ),
    m_mouseButton( WPickInfo::MOUSE_LEFT ),
    m_scrollValue( 0 ),
    m_pickPath( "" )
{
}

WPickInfo::modifierKey WPickInfo::getModifierKey() const
{
    return m_modKey;
}

void WPickInfo::setModifierKey( const modifierKey& modKey )
{
    m_modKey = modKey;
}

WPickInfo::WMouseButton WPickInfo::getMouseButton() const
{
    return m_mouseButton;
}

void WPickInfo::setMouseButton( const WMouseButton& mButton )
{
    m_mouseButton = mButton;
}

std::string WPickInfo::getName() const
{
    return m_name;
}

std::string WPickInfo::getPickPath() const
{
    return m_pickPath;
}

void WPickInfo::setPickPath( std::string path )
{
     m_pickPath = path;
}

void WPickInfo::appendToPickPath( std::string name )
{
     m_pickPath += " -- " + name + "\n ";
}

std::string WPickInfo::getViewerName() const
{
    return m_viewerName;
}

WPosition WPickInfo::getPickPosition() const
{
    return m_pickPosition;
}

WVector3d WPickInfo::getPickNormal() const
{
    return m_pickNormal;
}

inline bool WPickInfo::operator==( WPickInfo rhs ) const
{
    return ( this->m_name == rhs.m_name
             && this->m_pickPosition == rhs.m_pickPosition
             && this->m_modKey == rhs.m_modKey );
}

inline bool WPickInfo::operator!=( WPickInfo rhs ) const
{
    return !( *this == rhs );
}

inline WVector2d WPickInfo::getPickPixel() const
{
    WVector2d v;
    v[0] = m_pixelCoords.first;
    v[1] = m_pixelCoords.second;
    return v;
}

inline int32_t WPickInfo::getScrollWheel() const
{
    return m_scrollValue;
}

#endif  // WPICKINFO_H
