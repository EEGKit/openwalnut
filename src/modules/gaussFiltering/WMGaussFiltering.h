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

#ifndef WMGAUSSFILTERING_H
#define WMGAUSSFILTERING_H

#include <map>
#include <string>
#include <vector>

#include <osg/Node>
#include <osg/Geode>
#include <osg/Uniform>

#include "../../kernel/WModule.h"
#include "../../kernel/WModuleInputData.h"

#include "../../common/math/WVector3D.h"

#include "../../dataHandler/WDataSetScalar.h"

/**
 * Gauss filtering for WDataSetScalar
 *
 * \problem It works only on double value sets so far.
 * \reminder The boundary values will not be touched an considered to be zero.
 *
 * \ingroup modules
 */
class WMGaussFiltering : public WModule
{
public:
    /**
     * Standard constructor.
     */
    WMGaussFiltering();

    /**
     * Destructor.
     */
    ~WMGaussFiltering();

    /**
     * Gives back the name of this module.
     * \return the module's name.
     */
    virtual const std::string getName() const;

    /**
     * Gives back a description of this module.
     * \return description of module.
     */
    virtual const std::string getDescription() const;

    /**
     * Determine what to do if a property was changed.
     * \param propertyName Name of the property.
     */
    void slotPropertyChanged( std::string propertyName );

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
     * A condition used to notify about changes in several properties.
     */
    boost::shared_ptr< WCondition > m_propCondition;

    /**
     * The number of iterations to use for filtering
     */
    WPropInt m_iterations;

    /**
     * Simple convolution using a small gauss-like mask
     * \param vals the valueset to work on
     * \param nX number of positions in x direction
     * \param nY number of positions in y direction
     * \param nZ number of positions in z direction
     * \param x index for x direction
     * \param y index for x direction
     * \param z index for x direction
     * \param offset the offset to add to the index (useful for addressing vector/tensor elements)
     */
    template< typename T > double filterAtPosition(  boost::shared_ptr< WValueSet< T > > vals,
                                                     size_t nX, size_t nY, size_t nZ, size_t x, size_t y, size_t z, size_t offset );

    /**
     * Run the filter over the field.
     * \param vals the valueset to work on
     * \param grid the grid for the valueset
     * \param prog the progress used for this filter iteration
     *
     * \return the filtered array of values.
     */
    template< typename T > std::vector< double > filterField( boost::shared_ptr< WValueSet< T > > vals,
                                                              boost::shared_ptr< WGridRegular3D > grid,
                                                              boost::shared_ptr< WProgress > prog );

    /**
     * Run the filter iteratively over the field. The number of iterations is determined by m_iterations.
     *
     * \param vals the valueset to work on
     * \param iterations the number of iterations. If this value is <=1 then the filter gets applied exactly once.
     *
     * \return the filtered valueset.
     */
    template< typename T > boost::shared_ptr< WValueSet< double > > iterativeFilterField( boost::shared_ptr< WValueSet< T > > vals,
                                                                                          unsigned int iterations );

    boost::shared_ptr< WModuleInputData< WDataSetScalar > > m_input;  //!< Input connector required by this module.
    boost::shared_ptr< WModuleOutputData< WDataSetScalar > > m_output; //!< The only output of this filter module.
    boost::shared_ptr< WDataSetScalar > m_dataSet; //!< Pointer providing access to the treated data set in the whole module.
};
#endif  // WMGAUSSFILTERING_H
