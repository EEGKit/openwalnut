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

#ifndef WDATAHANDLER_H
#define WDATAHANDLER_H

#include <string>
#include <vector>

#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "../common/WSharedObject.h"
#include "../common/WSharedSequenceContainer.h"

#include "WDataSet.h"

class WSubject;

/**
 * Provides the environment for storing and accessing different subjects.
 * As all measured data belongs to one subject, this is the main access point
 * to our data.
 *
 * \ingroup dataHandler
 */
class WDataHandler
{
/**
 * Only UnitTests may be friends.
 */
friend class WDataHandlerTest;

public:
    /**
     * For shortening: a type defining a shared vector of WSubject pointers.
     */
    typedef std::vector< boost::shared_ptr< WSubject > > SubjectContainerType;

    /**
     * The alias for a shared container.
     */
    typedef WSharedSequenceContainer< boost::shared_ptr< WSubject >, SubjectContainerType > SubjectSharedContainerType;

    /**
     * Empty standard constructor.
     */
    WDataHandler();

    /**
     * Destructor.
     */
    virtual ~WDataHandler();

    /**
     * As WDataHandler is a singleton -> return instance.
     *
     * \return the instance.
     */
    static boost::shared_ptr< WDataHandler > getDataHandler();

    /**
     * Insert a new subject referenced by a pointer.
     *
     * \param subject a pointer to the subject that will be added
     */
    void addSubject( boost::shared_ptr< WSubject > subject );

    /**
     * Removes the specified subject if it is in the set.
     *
     * \param subject the subject to remove.
     */
    void removeSubject( boost::shared_ptr< WSubject > subject );

    /**
     * Returns the subject which corresponds to the specified ID. It throws an exception, if the subject does not exists anymore.
     *
     * \param subjectID the ID to search the subject for
     *
     * \return the subject.
     *
     * \throw WNoSuchSubject in case the subject can't be found.
     */
    boost::shared_ptr< WSubject > getSubjectByID( size_t subjectID );

    /**
     * Gets an access object which allows thread save iteration over the subjects.
     *
     * \return the access object.
     */
    SubjectSharedContainerType::WSharedAccess getAccessObject();

protected:

    /**
     * A container for all WSubjects.
     */
    SubjectSharedContainerType m_subjects;

    /**
     * The access object used for thread safe access.
     */
    SubjectSharedContainerType::WSharedAccess m_subjectAccess;

private:

    /**
     * Singleton instance of WDataHandler.
     */
    static boost::shared_ptr< WDataHandler > m_instance;

};

/**
 * \defgroup dataHandler Data Handler
 *
 * \brief
 * This module implements the data storage facility of OpenWalnut.
 */

#endif  // WDATAHANDLER_H
