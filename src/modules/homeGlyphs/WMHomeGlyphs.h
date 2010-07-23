//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2009 OpenWalnut Community, BSV-Leipzig and CNCF-CBS
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

#ifndef WMHOMEGLYPHS_H
#define WMHOMEGLYPHS_H

#include <string>

#include <osg/Geode>

#include "../../dataHandler/WDataSetSphericalHarmonics.h"
#include "../../kernel/WModule.h"
#include "../../kernel/WModuleInputData.h"
#include "../../kernel/WModuleOutputData.h"

/**
 * Spherical harmonics glyphs usingf teem (http://teem.sourceforge.net/).
 * According to http://www.ci.uchicago.edu/~schultz/sphinx/home-glyph.htm
 * Thanks to Thomas Schultz.
 *
 * \warning Not yet in useful state.
 * \ingroup modules
 */
class WMHomeGlyphs: public WModule
{
public:

    /**
     * Nothing special with this constructor.
     */
    WMHomeGlyphs();

    /**
     * Nothing special with this constructor.
     */
    virtual ~WMHomeGlyphs();

    /**
     * Gives back the name of this module.
     * \return the module's name.
     */
    virtual const std::string getName() const;

    /**
     * Gives back a description of this module.
     * \return description to module.
     */
    virtual const std::string getDescription() const;

    /**
     * Due to the prototype design pattern used to build modules, this method returns a new instance of this method. NOTE: it
     * should never be initialized or modified in some other way. A simple new instance is required.
     *
     * \return the prototype used to create every module in OpenWalnut.
     */
    virtual boost::shared_ptr< WModule > factory() const;

    /**
     * Get the icon for this module in XPM format.
     */
    virtual const char** getXPMIcon() const;

protected:

    /**
     * Entry point after loading the module. Runs in separate thread.
     */
    virtual void moduleMain();

    /**
     * Initialize the connectors this module is using.
     */
    virtual void connectors();

    /**
     * Initialize the properties for this module.
     */
    virtual void properties();


private:
    /**
     * An input connector that accepts sperical harmonics datasets.
     */
    boost::shared_ptr< WModuleInputData< WDataSetSphericalHarmonics > > m_input;

    /**
     * This condition denotes whether we need to recompute the surface
     */
    boost::shared_ptr< WCondition > m_recompute;

    WPropInt m_glyphIdProp; //!< Property holding the glyph ID

    /**
     * Just a preliminary function to avoid polluting moduleMain()
     */
    void execute();

    WPropBool m_usePolarPlotProp; //!< Property indicating whether to use polar plot instead of HOME glyph

    osg::ref_ptr< osg::Geode > m_glyphsGeode; //!< Pointer to geode containing the glyphs.
    osg::ref_ptr< WGEGroupNode > m_moduleNode; //!< Pointer to the modules group node.
};

#endif  // WMHOMEGLYPHS_H
