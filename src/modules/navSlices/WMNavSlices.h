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

#ifndef WMNAVSLICES_H
#define WMNAVSLICES_H

#include <list>
#include <string>
#include <vector>

#include <osg/Node>

#include "../../dataHandler/WDataSet.h"
#include "../../graphicsEngine/WShader.h"
#include "../../graphicsEngine/WGEGroupNode.h"
#include "../../kernel/WModule.h"
#include "../../kernel/WModuleConnector.h"
#include "../../kernel/WModuleInputData.h"

/**
 * Navigation slice module
 * \ingroup modules
 */
class WMNavSlices: public WModule, public osg::Referenced
{
public:

    /**
     * \par Description
     * Default constructor.
     */
    WMNavSlices();

    /**
     * \par Description
     * Destructor.
     */
    virtual ~WMNavSlices();

    /**
     * \par Description
     * Gives back the name of this module.
     * \return the module's name.
     */
    virtual const std::string getName() const;

    /**
     * \par Description
     * Gives back a description of this module.
     * \return description to module.
     */
    virtual const std::string getDescription() const;

    /**
     * updates the positions of the navigation slices
     */
    void updateGeometry();

    /**
     *  updates textures and shader parameters
     */
    void updateTextures();

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

    /**
     * Checks whether the nav slices are already running.
     *
     * \return true if running.
     */
    static bool isRunning();

protected:

    /**
     * \par Description
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

    /**
     * Receive DATA_CHANGE notifications.
     *
     * \param input the input connector that got the change signal. Typically it is one of the input connectors from this module.
     * \param output the output connector that sent the signal. Not part of this module instance.
     */
    virtual void notifyDataChange( boost::shared_ptr<WModuleConnector> input,
                                   boost::shared_ptr<WModuleConnector> output );


private:

    /**
     * Used as callback which simply sets m_textureChanged to true. Called by WSubject whenever the datasets change.
     */
    void notifyTextureChange();

    /**
     * True when textures have changed.
     */
    bool m_textureChanged;

    /**
     * Axial slice position.
     */
    WPropInt m_axialPos;

    /**
     * Coronal slice position.
     */
    WPropInt m_coronalPos;

    /**
     * Sagittal slice position.
     */
    WPropInt m_sagittalPos;

    /**
     * Maximum position of axial slice.
     */
    WPropInt m_maxAxial;

    /**
     * Maximum position of coronal slice.
     */
    WPropInt m_maxCoronal;

    /**
     * Maximum position of sagittal slice.
     */
    WPropInt m_maxSagittal;

    /**
     * True if axial slice should be visible.
     */
    WPropBool m_showAxial;

    /**
     * True if coronal slice should be visible.
     */
    WPropBool m_showCoronal;

    /**
     * True if sagittal slice should be visible.
     */
    WPropBool m_showSagittal;

    /**
     * initial create method
     */
    void create();

    /**
     * Initial creation function for the slice geometry
     * \param slice ID of the slice to be drawn. 0=y, 1=x, 2=z
     */
    osg::ref_ptr<osg::Geometry> createGeometry( int slice );

    /**
     * creates and initializes the uniform parameters for the shader
     * \param rootState The uniforms will be applied to this state.
     */
    void initUniforms( osg::StateSet* rootState );

    /**
     * the root node for this module
     */
    osg::ref_ptr< WGEGroupNode > m_rootNode;

    /**
     * nodes for each slice, to be reused in other widgets
     */
    osg::ref_ptr<osg::Geode> m_xSliceNode;

    /**
     * nodes for each slice, to be reused in other widgets
     */
    osg::ref_ptr<osg::Geode> m_ySliceNode;

    /**
     * nodes for each slice, to be reused in other widgets
     */
    osg::ref_ptr<osg::Geode> m_zSliceNode;

    /**
     * the shader object for this module
     */
    osg::ref_ptr< WShader >m_shader;

    /**
     * lock to prevent concurrent threads trying to update the osg node
     */
    boost::shared_mutex m_updateLock;

    /**
     * vector of uniforms for type of texture
     */
    std::vector< osg::ref_ptr<osg::Uniform> > m_typeUniforms;

    /**
     * vector of alpha values per texture
     */
    std::vector< osg::ref_ptr<osg::Uniform> > m_alphaUniforms;

    /**
     * vector of thresholds per texture
     */
    std::vector< osg::ref_ptr<osg::Uniform> > m_thresholdUniforms;

    /**
     * vector of samplers
     */
    std::vector< osg::ref_ptr<osg::Uniform> > m_samplerUniforms;

    /**
     * To avoid multiple instances of the nav slices.
     */
    static bool m_navsliceRunning;

    /**
     * Node callback to handle updates properly
     */
    class sliceNodeCallback : public osg::NodeCallback
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
            osg::ref_ptr< WMNavSlices > module = static_cast< WMNavSlices* > ( node->getUserData() );

            if ( module )
            {
                module->updateGeometry();
                module->updateTextures();
            }
            traverse( node, nv );
        }
    };
};



#endif  // WMNAVSLICES_H

