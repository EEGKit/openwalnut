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

#include <map>
#include <string>
#include <algorithm>
#include <vector>

#include <boost/signals2/signal.hpp>
#include <boost/function.hpp>

#include <osg/Node>

#include "../common/WLogger.h"
#include "../common/WSharedSequenceContainer.h"
#include "../common/WSharedAssociativeContainer.h"

#include "callbacks/WGEFunctorCallback.h"

#include "WGETexture.h"
#include "WShader.h"
#include "WExportWGE.h"

/**
 * Class implements a manager for multiple 3D textures. They can be applied to arbitrary osg::Node. This allows very comfortable use of dataset
 * based colormapping. The only requirement is that your geometry/node needs to specify texture coordinates in Object Space. That means: the
 * texture coordinates equal the regular 3D grid of the texture.
 */
class WGE_EXPORT WGEColormapping // NOLINT
{
public:
    /**
     * The alias for a shared container.
     */
    typedef WSharedSequenceContainer< std::vector< osg::ref_ptr< WGETexture3D > > > TextureContainerType;

    /**
     * Iterator to access the texture list.
     */
    typedef TextureContainerType::Iterator TextureIterator;

    /**
     * Const iterator to access the texture list.
     */
    typedef TextureContainerType::ConstIterator TextureConstIterator;

    /**
     * The type of handler used for being notified about added textures.
     */
    typedef boost::function< void ( osg::ref_ptr< WGETexture3D > ) > TextureRegisterHandler;

    /**
     * The type of handler used for being notified about removed textures.
     */
    typedef TextureRegisterHandler TextureDeregisterHandler;

    /**
     * The type of handler called whenever the texture list got resorted.
     */
    typedef boost::function< void ( void ) > TextureSortHandler;

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
     * \param shader the shader to use for colormapping. Provide your own shader here to let WGEColormap set some defines needed. If not
     * specified, a default shader is used.
     * \param startTexUnit the first texture unit allowed to be used
     */
    static void apply( osg::ref_ptr< osg::Node > node, osg::ref_ptr< WShader > shader = osg::ref_ptr< WShader >(), size_t startTexUnit = 0 );

    /**
     * Register the specified texture to the colormapper. The registered texture is the automatically applied to all users of WGEColormapping.
     * The texture gets inserted at the beginning of the texture list.
     *
     * \param texture the texture to add
     * \param name the name of the texture to add
     */
    static void registerTexture( osg::ref_ptr< WGETexture3D > texture, std::string name = "" );

    /**
     * De-register the specified texture to the colormapper. The texture is the automatically removed from all users of WGEColormapping. If the
     * texture is not in the list, nothing happens.
     *
     * \param texture the texture to remove
     */
    static void deregisterTexture( osg::ref_ptr< WGETexture3D > texture );

    /**
     * Resorts the the texture list using the specified comparator.
     *
     * \tparam Comparator the comparator type. Usually a boost::function or class providing the operator().
     * \param comp the comparator
     */
    template < typename Comparator >
    void sort( Comparator comp );

    /**
     * Move the specified texture one item up in the list. Causes the sort signal to fire.
     *
     * \param texture the texture swapped with its ascendant
     * \return true if swap was successful. False if not (texture not found, texture already at beginning).
     */
    bool moveUp( osg::ref_ptr< WGETexture3D > texture );

    /**
     * Move the specified texture one item down in the list. Causes the sort signal to fire.
     *
     * \param texture the texture swapped with its descendant
     * \return true if swap was successful. False if not (texture not found, texture already at end).
     */
    bool moveDown( osg::ref_ptr< WGETexture3D > texture );

    /**
     * Possible signals that can be subscribed for being notified about texture list changes.
     */
    typedef enum
    {
        Registered = 0, //!< texture got added
        Deregistered,   //!< texture got removed
        Sorted          //!< texture list was resorted
    }
    TextureListSignal;

    /**
     * Subscribe to the specified signal. See \ref TextureListSignal for details about their meaning.
     *
     * \param signal the signal to subscribe
     * \param notifier the notifier
     *
     * \return the connection. Keep this and disconnect it properly!
     */
    boost::signals2::connection subscribeSignal( TextureListSignal signal, TextureRegisterHandler notifier );

    /**
     * Subscribe to the specified signal. See \ref TextureListSignal for details about their meaning.
     *
     * \param signal the signal to subscribe
     * \param notifier the notifier
     *
     * \return the connection. Keep this and disconnect it properly!
     */
    boost::signals2::connection subscribeSignal( TextureListSignal signal, TextureSortHandler notifier );

    /**
     * Returns a read ticket to the texture array. Useful to iterate the textures.
     *
     * \return the read ticket
     */
    TextureContainerType::ReadTicket getReadTicket();

protected:

    /**
     * Default constructor.
     */
    WGEColormapping();

    /**
     * Apply the colormapping to the specified node.
     *
     * \param node the node.
     * \param shader the shader to use for colormapping. Provide your own shader here to let WGEColormap set some defines needed. If not
     * specified, a default shader is used.
     * \param startTexUnit the first texture unit allowed to be used
     */
    void applyInst( osg::ref_ptr< osg::Node > node, osg::ref_ptr< WShader > shader = osg::ref_ptr< WShader >(), size_t startTexUnit = 0 );

    /**
     * Register the specified texture to the colormapper. The registered texture is the automatically applied to all users of WGEColormapping.
     *
     * \param texture the texture to add
     * \param name the name of the texture.
     */
    void registerTextureInst( osg::ref_ptr< WGETexture3D > texture, std::string name );

    /**
     * De-register the specified texture to the colormapper. The texture is the automatically removed from all users of WGEColormapping.
     *
     * \param texture the texture to remove
     */
    void deregisterTextureInst( osg::ref_ptr< WGETexture3D > texture );

    /**
     * This callback handles all the updates needed. It is called by the m_callback instance every update cycle for each node using this
     * WGEColormapping instance.
     *
     * \param node
     */
    void callback( osg::Node* node );

    /**
     * Called whenever the texture list is updated.
     */
    void textureUpdate();

private:

    /**
     * Singleton instance of WGEColormapping
     */
    static boost::shared_ptr< WGEColormapping > m_instance;

    /**
     * The textures managed by this instance.
     */
    TextureContainerType m_textures;

    /**
     * The callback used for all the texture update handling on several nodes.
     */
    osg::ref_ptr< WGEFunctorCallback< osg::Node > > m_callback;

    /**
     * Simple structure to store some additional node-related info like texture units and so on.
     */
    struct NodeInfo
    {
        bool   m_rebind;        //!< true if the node has not been callback'ed before
        size_t m_texUnitStart;  //!< the start index of the texture unit to use
    };

    /**
     * The alias for a shared container with a set of node-nodeInfo pairs
     */
    typedef WSharedAssociativeContainer< std::map< osg::Node*, NodeInfo* > > NodeInfoContainerType;

    /**
     * This map is needed to keep track of several node specific settings
     */
    NodeInfoContainerType m_nodeInfo;

    /**
     * This shader is used wherever no other shader was specified.
     */
    osg::ref_ptr< WShader > m_defaultShader;

    /**
     * Called whenever a texture got registered.
     */
    boost::signals2::signal< void( osg::ref_ptr< WGETexture3D > ) > m_registerSignal;

    /**
     * Called whenever a texture got removed.
     */
    boost::signals2::signal< void( osg::ref_ptr< WGETexture3D > ) > m_deregisterSignal;

    /**
     * Called whenever the texture list got resorted
     */
    boost::signals2::signal< void( void ) > m_sortSignal;
};

template < typename Comparator >
void WGEColormapping::sort( Comparator comp )
{
    m_textures.sort< Comparator >( comp );
}

#endif  // WGECOLORMAPPING_H

