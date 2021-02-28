#ifndef WMCONVERTERVECTORS_H
#define WMCONVERTERVECTORS_H

#include <vector>

#include <boost/shared_ptr.hpp>

/**
 * represents a boost::shared_ptr to a vector containing a vector of floats.
 */
typedef boost::shared_ptr< std::vector< float > > SPFloatVector;

/**
 * represents a boost::shared_ptr to a vector containing a vector of size_t.
 */
typedef boost::shared_ptr< std::vector< size_t > > SPSizeVector;

class WMConverterVectors
{
public:
    typedef boost::shared_ptr< WMConverterVectors > SPtr;

    WMConverterVectors();

    void clear();

    SPFloatVector getVertices();

    SPFloatVector getSizes();

    SPFloatVector getColors();

    SPFloatVector getEdeps();

    SPSizeVector getFiberStartIndexes();

    SPSizeVector getFiberLengths();

    SPSizeVector getVerticesReverse();

    SPSizeVector getEventIDs();

private:
    SPFloatVector m_vertices;

    SPFloatVector m_sizes;

    SPFloatVector m_colors;

    SPFloatVector m_edeps;

    SPSizeVector m_fiberStartIndexes;

    SPSizeVector m_fiberLengths;

    SPSizeVector m_verticesReverse;

    SPSizeVector m_eventIDs;
};

#endif  // WMCONVERTERVECTORS_H