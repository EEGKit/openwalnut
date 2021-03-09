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

#ifndef WMWRITECSV_H
#define WMWRITECSV_H

#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"

#include "core/dataHandler/WDataSetCSV.h"
#include "core/dataHandler/WDataSetPointsAndFibers.h"
#include "core/dataHandler/WDataSetPoints.h"
#include "core/dataHandler/WDataSetFibers.h"

#include "../filterProtonData/WMProtonData.h"

class WMWriteCSV : public WModule
{
public:
    /**
     * Standard constructor.
     */
    WMWriteCSV();

    /**
     * Destructor.
     */
    ~WMWriteCSV();

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
    /**
     * This performs all work necessary to actually write the data to the file
     */
    void writeToFile();

    /**
     * Helpermethod to create a List of internal vertex with id
     */
    std::list< std::tuple < osg::Vec3, int > > getListOfInternalVertex( WDataSetFibers::SPtr fibers );

    /**
     * Helpermethod to create a List of internal Position with id
     */
    std::list< std::tuple < int, int > > getListOfPositionAndID( 
                            std::list< std::tuple < osg::Vec3, int > > listOfInternalVertex, 
                            WDataSetPoints::SPtr points);

    /**
     * The filename property -> where to write the nifty file
     */
    WPropFilename m_filename;
    
    WPropTrigger  m_saveTriggerProp; //!< This property triggers the actual writing,

    boost::shared_ptr< WModuleInputData< WDataSetCSV > > m_CSVInput;  //!< Input connector required by this module.
    boost::shared_ptr< WModuleInputData< WDataSetPointsAndFibers > > m_PointsAndFibersInput;  //!< Input connector required by this module.
};


#endif  // WMWRITECSV_H
