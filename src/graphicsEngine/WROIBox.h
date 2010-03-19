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

#ifndef WROIBOX_H
#define WROIBOX_H

#include <string>
#include <utility>

#include <boost/thread.hpp>

#include "../common/math/WPosition.h"
#include "WPickHandler.h"
#include "WGEViewer.h"

#include "WROI.h"

/**
 * A box representing a region of interest.
 */
class WROIBox : public WROI
{
public:
    /**
     * Yields box with desired extremal points minPos and maxPos
     * \param minPos Left, lower, front corner. Minimal x, y and z coordinates.
     * \param maxPos Right, upper, back corner. Maximal x, y and z coordinates.
     */
    WROIBox(  wmath::WPosition minPos, wmath::WPosition maxPos );

    virtual ~WROIBox();

    /**
     * Get the corner of the box that has minimal x, y and z values
     */
    wmath::WPosition getMinPos() const;

    /**
     * Get the corner of the box that has maximal x, y and z values
     */
    wmath::WPosition getMaxPos() const;


protected:
private:
    static size_t maxBoxId; //!< Current maximum boxId over all boxes.
    size_t boxId; //!< Id of the current box.

    wmath::WPosition m_minPos; //!< The minimum position of the box
    wmath::WPosition m_maxPos; //!< The maximum position of the box
    bool m_isPicked; //!< Indicates whether the box is currently picked or not.
    wmath::WPosition m_pickedPosition; //!< Caches the old picked position to a allow for cmoparison
    std::pair< float, float > m_oldPixelPosition; //!< Caches the old picked position to a allow for cmoparison
    boost::shared_mutex m_updateLock; //!< Lock to prevent concurrent threads trying to update the osg node
    osg::ref_ptr< osg::Geometry > m_surfaceGeometry; //!< store this pointer for use in updates

    WPickInfo m_pickInfo; //!< Stores the pick information for potential redraw

    boost::shared_ptr< WGEViewer > m_viewer; //!< makes viewer available all over this class.

    /**
     * note that there was a pick
     * \param pickInfo info from pick
     */
    void registerRedrawRequest( WPickInfo pickInfo );

    /**
     *  updates the graphics
     */
    virtual void updateGFX();

    /**
     * Node callback to handle updates properly
     */
    class ROIBoxNodeCallback : public osg::NodeCallback
    {
    public: // NOLINT
        /**
         * operator ()
         *
         * \param node the osg node
         * \param nv the node visitor
         */
        virtual void operator()( osg::Node* node, osg::NodeVisitor* nv )
        {
            osg::ref_ptr< WROIBox > module = static_cast< WROIBox* > ( node->getUserData() );
            if ( module )
            {
                module->updateGFX();
            }
            traverse( node, nv );
        }
    };
};

#endif  // WROIBOX_H
