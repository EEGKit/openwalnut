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

#ifndef WDATASETFIBERS_H
#define WDATASETFIBERS_H

#include <string>
#include <utility>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>

#include "../common/math/WPosition.h"

#include "WDataSet.h"

/**
 * converts an integer into a byte array and back
 */
union converterByteINT32
{
        unsigned char b[4]; //!< the bytes
        int i; //!< the int
};

/**
 * converts a float into a byte array and back
 */
union converterByteFloat
{
        unsigned char b[4]; //!< the bytes
        float f; //!< the float
};

/**
 * Represents a simple set of WFibers.
 */
class WDataSetFibers : public WDataSet
{
public:

    // some type alias for the used arrays.

    /**
     * List of vertex coordinates.
     */
    typedef boost::shared_ptr< std::vector< float > > VertexArray;

    /**
     * Index list indexing fibers in VertexArray.
     */
    typedef boost::shared_ptr< std::vector< size_t > > IndexArray;

    /**
     * Lengths of fibers.
     */
    typedef boost::shared_ptr< std::vector< size_t > > LengthArray;

    /**
     * Tangents at each vertex in VertexArray.
     */
    typedef boost::shared_ptr< std::vector< float > > TangentArray;

    /**
     * Colors for each vertex in VertexArray.
     */
    typedef boost::shared_ptr< std::vector< float > > ColorArray;

    /**
     * Constructs a new set of fibers, usage of WFiber here is for backward compatibility and should be removed
     *
     * \param vertices the vertices of the fibers, stored in x1,y1,z1,x2,y2,z2, ..., xn,yn,zn scheme
     * \param lineStartIndexes the index in which the fiber start (index of the 3D-vertex, not the index of the float in the vertices vector)
     * \param lineLengths how many vertices belong to a fiber
     * \param verticesReverse stores for each vertex the index of the corresponding fiber
     * \param boundingBox The bounding box of the fibers (first minimum, secon maximum). If not given both are (0,0,0).
     */
    WDataSetFibers( boost::shared_ptr< std::vector< float > >vertices,
                    boost::shared_ptr< std::vector< size_t > > lineStartIndexes,
                    boost::shared_ptr< std::vector< size_t > > lineLengths,
                    boost::shared_ptr< std::vector< size_t > > verticesReverse,
                    std::pair< wmath::WPosition, wmath::WPosition > boundingBox = std::make_pair( wmath::WPosition(), wmath::WPosition() )  );

    /**
     * Constructs a new set of WFibers. The constructed instance is not usable.
     */
    WDataSetFibers();

    /**
     * Get number of fibers in this data set.
     */
    size_t size() const;

    /**
     * Determines whether this dataset can be used as a texture.
     *
     * \return true if usable as texture.
     */
    virtual bool isTexture() const;

    /**
     * Gets the name of this prototype.
     *
     * \return the name.
     */
    virtual const std::string getName() const;

    /**
     * Gets the description for this prototype.
     *
     * \return the description
     */
    virtual const std::string getDescription() const;

    /**
     * Returns a prototype instantiated with the true type of the deriving class.
     *
     * \return the prototype.
     */
    static boost::shared_ptr< WPrototyped > getPrototype();

    /**
     * Getter for m_vertices
     */
    VertexArray getVertices() const;

    /**
     * Getter
     */
    IndexArray getLineStartIndexes() const;

    /**
     * Getter
     */
    LengthArray getLineLengths() const;

    /**
     * Getter
     */
    IndexArray getVerticesReverse() const;

    /**
     * Getter
     */
    TangentArray getTangents() const;

    /**
     * Reference to the vector storing the global colors.
     *
     * \return Pointer to the float array.
     */
    ColorArray getGlobalColors() const;

    /**
     * Reference to the vector storing the local colors.
     *
     * \return Pointer to the float array.
     */
    ColorArray getLocalColors() const;

    /**
     * This method adds a new color scheme to the list of available colors. The color scheme needs to have a name and description to allow the
     * user to identify which color has which meaning. If the specified color array already exists, only an update is triggered and the name and
     * description is ignored
     *
     * \param colors the color array. Needs to have exactly getVertices()->size() items.
     * \param name name of the color scheme. Should be a telling name.
     * \param description description. How calculated and so on.
     */
    void addColorScheme( ColorArray colors, std::string name, std::string description );

    /**
     * This method removes the specified color scheme from the list and triggers an update.
     * \param colors the color array.
     */
    void removeColorScheme( ColorArray colors );

    /**
     * returns the position in space for a vertex of a given fiber
     *
     * \param fiber
     * \param vertex
     */
    wmath::WPosition getPosition( size_t fiber, size_t vertex ) const;

    /**
     * calculates the tangent for a point on the fiber
     *
     * \param fiber
     * \param vertex
     */
    wmath::WPosition getTangent( size_t fiber, size_t vertex ) const;

    /**
     * saves the selected fiber bundles to a file
     *
     *\param filename
     * \param active bitfield of the fiber selection
     */
    void saveSelected( std::string filename, boost::shared_ptr< std::vector< bool > > active ) const;

    /**
     * Get the bounding box as pair of WPositions.
     */
    std::pair< wmath::WPosition, wmath::WPosition > getBoundingBox() const;

protected:

    /**
     * The prototype as singleton.
     */
    static boost::shared_ptr< WPrototyped > m_prototype;

private:
    /**
     * Point vector for all fibers
     */
    VertexArray m_vertices;

    /**
     * Point vector for tangents at each vertex, used for fake tubes
     */
    TangentArray m_tangents;

    // the following typedefs are for convenience.

    /**
     * For shortening: a type defining a shared vector of color arrays.
     */
    typedef std::vector< ColorArray > ColorArrayContainerType;

    /**
     * The alias for a shared container.
     */
    typedef WSharedSequenceContainer< ColorArrayContainerType > ColorArraySharedContainerType;

    /**
     * The const iterator type of the container.
     */
    typedef ColorArrayContainerType::const_iterator ColorArrayConstIterator;

    /**
     * The iterator type of the container.
     */
    typedef ColorArrayContainerType::iterator ColorIterator;

    /**
     * An array of color arrays. The first two elements are: 0: global color, 1: local color
     */
    ColorArraySharedContainerType m_colors;

    /**
     * Line vector that contains the start index of its first point for each line.
     * \warning The index returned cannot be used in the vertices array until
     * the number of components for each point is multiplied.
     */
    IndexArray m_lineStartIndexes;

    /**
     * Line vector that contains the number of vertices for each line
     */
    LengthArray m_lineLengths;

    /**
     * Reverse lookup table for which point belongs to which fiber
     */
    IndexArray m_verticesReverse;

    wmath::WPosition m_bbMin; //!< Minimum position of bounding box of all fibers.
    wmath::WPosition m_bbMax; //!< Maximum position of bounding box of all fibers.
};

#endif  // WDATASETFIBERS_H
