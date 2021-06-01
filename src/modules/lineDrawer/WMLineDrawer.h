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

#ifndef WMLINEDRAWER_H
#define WMLINEDRAWER_H

#include <string>
#include <vector>

#include <osg/Depth>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/LineWidth>

#include "core/common/math/WLine.h"
#include "core/kernel/WKernel.h"
#include "core/kernel/WModule.h"
#include "WDrawHandler.h"
#include "WOverlay.h"

class WMLineDrawer : public WModule
{
public:
    typedef boost::shared_ptr< WMLineDrawer > SPtr;

    WMLineDrawer();
    virtual ~WMLineDrawer();
    virtual const std::string getName() const;
    virtual const std::string getDescription() const;
    virtual boost::shared_ptr< WModule > factory() const;
    virtual const char** getXPMIcon() const;

    void updateLines();
    void startNewLine();
    void addPoint( float x, float y );

protected:
    virtual void moduleMain();
    virtual void conenctors();
    virtual void properties();

private:
    osg::ref_ptr< WOverlay > m_overlay;
    boost::shared_ptr< std::vector< WLine > > m_lines;
};

#endif  // WMLINEDRAWER_H
