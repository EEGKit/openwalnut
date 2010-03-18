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

#ifndef WPLANE_H
#define WPLANE_H

#include <set>

#include <boost/shared_ptr.hpp>

#include "../../common/math/WPosition.h"
#include "../../common/math/WVector3D.h"

/**
 * Represents a plane with a normal vector and a position in space.
 */
class WPlane
{
public:
    /**
     * Constructs a plane with its normal and containing the point.
     *
     * \param normal Direction of the plane
     * \param pos Position of the plane
     *
     * \return
     */
    WPlane( const wmath::WVector3D& normal, const wmath::WPosition& pos );

    /**
     * Destructor.
     */
    virtual ~WPlane();

    /**
     * Determines whether a given point is in this plane or not.
     *
     * \param point Position to query
     *
     * \return True if and only if the point is in this plane.
     */
    bool isInPlane( wmath::WPosition point ) const;

    /**
     * Reset the position of the plane, normal remains the same.
     *
     * \param newPos New Position (point in plane).
     */
    void resetPosition( wmath::WPosition newPos );

    /**
     * Computes with relative coordinates a point in this plane. (0,0) means its position is returned.
     *
     * \param x how far along the direction of the first vector which spans the plane
     * \param y how far along the direction of the second vector which spans the plane too
     *
     * \return the new calculated position
     */
    wmath::WPosition getPointInPlane( double x, double y ) const;

    /**
     * Returns a point in that plane.
     *
     * \return The point in that plane describing its position
     */
    const wmath::WPosition& getPosition() const;

    /**
     * Returns the normal of the plane.
     *
     * \return Normalized normal vector.
     */
    const wmath::WVector3D& getNormal() const;

//    \cond
//    /**
//     * Computes sample points on that plane.
//     *
//     * \param grid
//     * \param stepWidth
//     *
//     * \return Set of positions on the plane
//     */
//    boost::shared_ptr< std::set< wmath::WPosition > > samplePoints( const WGridRegular3D& grid, double stepWidth );
//    \endcond

    /**
     * Computes a fixed number of sample points on that plane.
     *
     * \param stepWidth
     * \param numX
     * \param numY
     *
     * \return Set of positions on the plane
     */
    boost::shared_ptr< std::set< wmath::WPosition > > samplePoints( double stepWidth, size_t numX, size_t numY ) const;

protected:
    wmath::WVector3D m_normal; //!< Direction of the plane
    wmath::WPosition m_pos; //!< Position of the plane specifying the center
    wmath::WVector3D m_first; //!< First vector in the plane
    wmath::WVector3D m_second; //!< Second vector in the plane

private:
};

inline const wmath::WPosition& WPlane::getPosition() const
{
    return m_pos;
}

inline const wmath::WVector3D& WPlane::getNormal() const
{
    return m_normal;
}

#endif  // WPLANE_H
