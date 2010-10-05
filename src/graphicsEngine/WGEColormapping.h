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

#ifndef WGECOLORMAPPING_H
#define WGECOLORMAPPING_H

#include <osg/Node>

#include "../common/WLogger.h"
#include "../common/WSharedSequenceContainer.h"

#include "WGETexture.h"

/**
 * Class implements a manager for multiple 3D textures. They can be applied to arbitrary osg::Node. This allows very comfortable use of dataset
 * based colormapping. The only requirement is that your geometry/node needs to specify texture coordinates in Object Space. That means: the
 * texture coordinates equal the regular 3D grid of the texture.
 */
class WGEColormapping
{
public:

    /**
     * Default constructor.
     */
    WGEColormapping();

    /**
     * Destructor.
     */
    virtual ~WGEColormapping();

    /**
     * Returns instance of the module factory to use to create modules.
     *
     * \return the running module factory.
     */
    static boost::shared_ptr< WGEColormapping > instance();

    /**
     * Apply the colormapping to the specified node.
     *
     * \param node the node.
     * \param useDefaultShader if true, a standard colormapping shader is used. This is useful for plain geometry.
     */
    static void apply( osg::ref_ptr< osg::Node > node, bool useDefaultShader = true );

    /**
     * Register the specified texture to the colormapper. The registered texture is the automatically applied to all users of WGEColormapping.
     *
     * \param texture the texture to add
     */
    static void registerTexture( osg::ref_ptr< WGETexture3D > texture );

    /**
     * De-register the specified texture to the colormapper. The texture is the automatically removed from all users of WGEColormapping. If the
     * texture is not in the list, nothing happens.
     *
     * \param texture the texture to remove
     */
    static void deregisterTexture( osg::ref_ptr< WGETexture3D > texture );

protected:

    /**
     * Apply the colormapping to the specified node.
     *
     * \param node the node.
     * \param useDefaultShader if true, a standard colormapping shader is used. This is useful for plain geometry.
     */
    void applyInst( osg::ref_ptr< osg::Node > node, bool useDefaultShader = true );

    /**
     * Register the specified texture to the colormapper. The registered texture is the automatically applied to all users of WGEColormapping.
     *
     * \param texture the texture to add
     */
    void registerTextureInst( osg::ref_ptr< WGETexture3D > texture );

    /**
     * De-register the specified texture to the colormapper. The texture is the automatically removed from all users of WGEColormapping.
     *
     * \param texture the texture to remove
     */
    void deregisterTextureInst( osg::ref_ptr< WGETexture3D > texture );

private:

    /**
     * Singleton instance of WGEColormapping
     */
    static boost::shared_ptr< WGEColormapping > m_instance;

    /**
     * The alias for a shared container.
     */
    typedef WSharedSequenceContainer< std::vector< osg::ref_ptr< WGETexture3D > > > TextureSharedContainerType;

    /**
     * The textures managed by this instance.
     */
    TextureSharedContainerType m_textures;
};

#endif  // WGECOLORMAPPING_H

