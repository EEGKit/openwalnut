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

#ifndef WMFIBERDISPLAY_H
#define WMFIBERDISPLAY_H

// This is the number of the attribute channel used
// to pass the second vertex attributes for morphing.
// According to NVidia spec not all channels are free,
// but number 6 is guaranteed to be free.
#define VERTEXBIND 6

#include <string>

#include <osg/Geode>
#include <osg/Switch>
#include <osg/ref_ptr>
#include <osg/Group>

#include "../../../dataHandler/datastructures/WFiberCluster.h"
#include "../../../dataHandler/WDataSetFibers.h"
#include "../../../graphicsEngine/WROI.h"
#include "../../../graphicsEngine/WROIBox.h"
#include "../../../graphicsEngine/WShader.h"
#include "../../WModule.h"
#include "../../WModuleInputData.h"
#include "WTubeDrawable.h"
#include "WTubeGeometry.h"

#include "../../WExportKernel.h"

/**
 * Module for drawing fibers
 *
 * \ingroup modules
 */
class OWKERNEL_EXPORT WMFiberDisplay : public WModule
{
public:
    /**
     * Constructs new FiberTestModule
     */
    WMFiberDisplay();

    /**
     * Destructs this FiberTestModule
     */
    virtual ~WMFiberDisplay();

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
     * Due to the prototype design pattern used to build modules, this method
     * returns a new instance of this method. NOTE: it should never be
     * initialized or modified in some other way. A simple new instance is
     * required.
     *
     * \return the prototype used to create every module in OpenWalnut.
     */
    virtual boost::shared_ptr< WModule > factory() const;

    /**
     * Get the icon for this module in XPM format.
     */
    virtual const char** getXPMIcon() const;

    /**
     * Checks whether the fiber display is already running.
     *
     * \return true if running.
     */
    static bool isRunning();

protected:
    /**
     * Callback for m_active. Overwrite this in your modules to handle m_active changes separately.
     */
    virtual void activate();

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

    /**
     * Redraws the scene.
     *
     */
    void update();

    /**
     * initial creation of the osg geometry
     */
    void create();

    /**
     * Used as callback which simply sets m_textureChanged to true. Called by WSubject whenever the datasets change.
     */
    void notifyTextureChange();

    /**
    * switches between fiber display and tube representation,
    * texturing and box culling
    * activates the neccesary shaders
    */
    void updateRenderModes();

    /**
    * Enable disable global or local coloring
    */
    void toggleColoring();

private:
    /**
     * Updates the output with the current selection and generates a WFiberCluster out of it.
     */
    void updateOutput() const;

    WPropBool m_coloring; //!< Enable/Disable global (true) or local (false) coloring of the fiber tracts
    WPropBool m_customColoring; //!< Enable/Disable custom colors
    WPropBool m_useTubesProp; //!< Property indicating whether to use tubes for the fibers tracts.
    WPropBool m_useTextureProp; //!< Property indicating whether to use tubes for the fibers tracts.
    WPropDouble m_tubeThickness; //!< Property determining the thickness of tubes .
    WPropBool m_save; //!< this should be a button
    WPropFilename m_saveFileName; //!< the filename for saving
    WPropTrigger m_updateOC; //!< updates the output connector

	WPropBool m_depthCueing;
	WPropBool m_lightModel;
	WPropBool m_usePointSprites;
	WPropBool m_useQuadStripes;
	WPropDouble m_lowDepthCueingFactor;
	WPropDouble m_upperDepthCueingFactor;

    WBoolFlag m_noData; //!< Flag indicating whether there is data to display.

    WPropGroup m_cullBoxGroup; //!< property group for box culling
    WPropBool m_activateCullBox; //!< if true fibers are culled depending on a cull box
    WPropBool m_showCullBox; //!< Enable/Disable showing of the cull box
    WPropBool m_insideCullBox; //!< if true fibers inside the cull box are shown, outside if false

	/**
     * Input connector for a fiber dataset.
     */
    boost::shared_ptr< WModuleInputData< const WDataSetFibers > > m_fiberInput;

    /**
     * Output connector for the fiber cluster dataset.
     */
    boost::shared_ptr< WModuleOutputData< WFiberCluster > > m_clusterOC;

    /**
     * Pointer to the fiber data set
     */
    boost::shared_ptr< const WDataSetFibers > m_dataset;

    /**
     * OSG node for this module. All other OSG nodes of this module should be
     * placed as child to this node.
     */
	osg::ref_ptr< osg::Group > m_osgNode;


	/**
	 * OSG node for this module. All other OSG nodes of this module should be
	 * placed as child to this node.
	 */
	osg::ref_ptr< osg::Switch > m_osgSwitch;

	osg::ref_ptr< osg::Vec3Array > m_tubeVerts;
	osg::ref_ptr< osg::Vec3Array > m_tubeTangents;
	osg::ref_ptr< osg::Vec2Array > m_tubeTexCoords;
	osg::ref_ptr< osg::Vec3Array > m_tubeColors;
	osg::ref_ptr< osg::Vec2Array > m_pointTexCoords;
	osg::ref_ptr< osg::Vec3Array > m_pointVerts;
	osg::ref_ptr< osg::Vec3Array > m_pointTangents;
	osg::ref_ptr< osg::Vec3Array > m_pointColors;
	osg::ref_ptr< osg::VectorGLuint > m_fiberQuadStartIndexes;
	osg::ref_ptr< osg::VectorGLuint > m_fiberPointStartIndexes;

	osg::ref_ptr< osg::Geode > m_geodeTubeDrawable;
	osg::ref_ptr< osg::Geode > m_geodePointSprite;
	osg::ref_ptr< osg::Geode > m_geodeQuadStripes;

    /**
     * stores pointer to the fiber drawer
     */
    osg::ref_ptr< WTubeDrawable > m_tubeDrawable;

	/**
	 * stores pointer to the fiber drawer
	 */
	osg::ref_ptr< WTubeGeometry > m_tubeGeometryPointSprite;

	/**
	 * stores pointer to the fiber drawer
	 */
	osg::ref_ptr< WTubeGeometry > m_tubeGeometryQuadStripes;

	/**
     * lock to prevent concurrent threads trying to update the osg node
     */
    boost::shared_mutex m_updateLock;

    /**
     * the shader object for rendering tubes
     */
    osg::ref_ptr< WShader >m_shaderTubes;
	osg::ref_ptr< WShader >m_shaderTubesPS;
	osg::ref_ptr< WShader >m_shaderTubesQS;


    /**
     * the shader object for rendering textured lines
     */
    osg::ref_ptr< WShader >m_shaderTexturedFibers;

    osg::ref_ptr<osg::Uniform> m_uniformTubeThickness; //!< tube thickness

    /**
     * True when textures have changed.
     */
    bool m_textureChanged;

    /**
     * boolean to notify the shader to use the texture instead of glColor
     */
    osg::ref_ptr<osg::Uniform> m_uniformUseTexture;

    /**
     * uniform for type of texture
     */
    osg::ref_ptr<osg::Uniform> m_uniformType;

    /**
     * threshold for texture
     */
    osg::ref_ptr<osg::Uniform> m_uniformThreshold;

    /**
     * color map for the  texture
     */
    osg::ref_ptr<osg::Uniform> m_uniformsColorMap;

    /**
     * vector of samplers
     */
    osg::ref_ptr<osg::Uniform> m_uniformSampler;

    osg::ref_ptr<osg::Uniform> m_uniformDimX; //!< x dimension of the dataset for calculating the texture coord in the shader
    osg::ref_ptr<osg::Uniform> m_uniformDimY; //!< y dimension of the dataset for calculating the texture coord in the shader
    osg::ref_ptr<osg::Uniform> m_uniformDimZ; //!< z dimension of the dataset for calculating the texture coord in the shader

    osg::ref_ptr<osg::Uniform> m_uniformUseCullBox; //!< notify shader that cull box is activated
    osg::ref_ptr<osg::Uniform> m_uniformInsideCullBox; //!< notify shader that fibers insider or outside cull box are shown


    osg::ref_ptr<osg::Uniform> m_uniformCullBoxLBX; //!< cull box lower bound
    osg::ref_ptr<osg::Uniform> m_uniformCullBoxLBY; //!< cull box lower bound
    osg::ref_ptr<osg::Uniform> m_uniformCullBoxLBZ; //!< cull box lower bound
    osg::ref_ptr<osg::Uniform> m_uniformCullBoxUBX; //!< cull box upper bound
    osg::ref_ptr<osg::Uniform> m_uniformCullBoxUBY; //!< cull box upper bound
    osg::ref_ptr<osg::Uniform> m_uniformCullBoxUBZ; //!< cull box upper bound

	osg::ref_ptr<osg::Uniform> m_uniformViewportHeight;
	osg::ref_ptr<osg::Uniform> m_uniformViewportWidth;

	osg::ref_ptr<osg::Uniform> m_uniformDepthCueing;
	osg::ref_ptr<osg::Uniform> m_uniformLightModel;
	osg::ref_ptr<osg::Uniform> m_uniformUsePointSprites;
	osg::ref_ptr<osg::Uniform> m_uniformUseQuadStripes;
	osg::ref_ptr<osg::Uniform> m_uniformLowDepthCueingFactor;
	osg::ref_ptr<osg::Uniform> m_uniformUpperDepthCueingFactor;

    /**
     * To avoid multiple instances of the fiber display.
     */
    static bool m_fiberDisplayRunning;

    osg::ref_ptr< WROIBox > m_cullBox; //!< stores a pointer to the cull box


    /**
     * changes tube parameters
     */
    void adjustTubes();

    /**
     * saves the currently selected (active field from roi manager) fibers to a file
     */
    void saveSelected();

    /**
     * creates and initializes the uniform parameters for the shader
     * \param rootState The uniforms will be applied to this state.
     */
    void initUniforms( osg::StateSet* rootState );

    /**
     *  updates textures and shader parameters
     */
    void updateTexture();

    /**
     * create a selection box to cull the fibers
     */
    void initCullBox();


	/**
	  * Creates a texture to simmulate diffuse and specular lightning
	  * of a tube from aside
	  */
	void create1DTextureRectLightning(osg::StateSet* rootState) const;

	/**
	  * Creates a texture to simmulate diffuse and specular lightning
	  * of a tube from top or bottom
	  */
	void create2DTextureCycleLightning(osg::StateSet* rootState) const;

	void create2DTexDiffuseLightning(osg::StateSet* rootState) const;
	void create2DTexSpecularLightning(osg::StateSet* rootState) const;

	void createTextures(osg::StateSet* rootState) const;
	void createTubeData();




    /**
    * Wrapper class for userData to prevent cyclic destructor calls
    */
    class userData: public osg::Referenced
    {
    public:
        /**
        * userData Constructur with shared pointer to module
        * \param _parent pointer to the module 
        */
        explicit userData( boost::shared_ptr< WMFiberDisplay > _parent )
        {
            parent = _parent;
        }

        /**
        * update wrapper Function
        */
        void update();

        /**
        * updateRenderModes wrapper Function
        */
        void updateRenderModes();

        /**
        * toggleColoring wrapper Function
        */
        void toggleColoring();
    private:
        /**
        * shared pointer to the module
        */
        boost::shared_ptr< WMFiberDisplay > parent;
    };


    /**
     * Node callback to handle updates properly
     */
    class fdNodeCallback : public osg::NodeCallback
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
            osg::ref_ptr< userData > module = static_cast< userData* > ( node->getUserData() );

            if ( module )
            {
                module->update();
                module->updateRenderModes();
                module->toggleColoring();
            }
            traverse( node, nv );
        }
    };
};

inline const std::string WMFiberDisplay::getName() const
{
    return std::string( "Fiber Display" );
}

inline const std::string WMFiberDisplay::getDescription() const
{
    return std::string( "Draws fibers out of a WDataSetFibers" );
}

#endif  // WMFIBERDISPLAY_H
