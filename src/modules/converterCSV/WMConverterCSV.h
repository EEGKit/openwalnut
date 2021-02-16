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

#ifndef WMCONVERTERCSV_H
#define WMCONVERTERCSV_H

#include <string>
#include <vector>

#include "core/dataHandler/WDataSetCSV.h"
#include "core/dataHandler/WDataSetFibers.h"
#include "core/kernel/WModuleOutputData.h"
#include "core/dataHandler/WDataSetPoints.h"

#include "core/kernel/WModule.h"

/**
 * This module simply registers a given csv dataset to the csv handling mechanism.
 * This allows all outputs to be shown as a fiber- or a point-dataset.
 *
 * \ingroup modules
 */
class WMConverterCSV : public WModule
{
public:
    /**
     * Standard constructor.
     */
    WMConverterCSV();

    /**
     * Destructor.
     */
    virtual ~WMConverterCSV();

    /**
     * Gives back the name of this module.
     * \return the module's name.
     */
    virtual const std::string getName() const;

    /**
     * Gives back the description of this module.
     * \return description of the module.
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
     * \return The icon.
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
    boost::shared_ptr< WModuleInputData< WDataSetCSV > > m_input;    //!< Input connector required for this module.
    boost::shared_ptr< WModuleOutputData< WDataSet > > m_output_points;    //!< Output connector required for this module.
    boost::shared_ptr< WModuleOutputData< WDataSet  > > m_output_fibers;


    boost::shared_ptr< std::vector < std::vector < std::string > > > m_content;
    boost::shared_ptr< std::vector < float > > m_vertices;
    boost::shared_ptr< std::vector < size_t > > m_lineStartIndexes;
    boost::shared_ptr< std::vector < size_t > > m_lineLengths;
    boost::shared_ptr< std::vector < size_t > > m_verticesReverse;

    boost::shared_ptr< WDataSetFibers> m_fibers;
    boost::shared_ptr< WDataSetPoints> m_points;

    boost::shared_ptr< std::vector < float > > m_colors;

    int getColumnNumberByName( std::string col, std::vector<std::string> in_row );
    void FilterFibers( boost::shared_ptr< std::vector < std::vector < std::string > > >  dataCSV );
    void FilterPoints( boost::shared_ptr< std::vector < std::vector < std::string > > >  dataCSV );
};

#endif  // WMCONVERTERCSV_H
