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

#ifndef WPROGRESS_H
#define WPROGRESS_H

#include <set>
#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

/**
 * Class managing progress inside of modules. It interacts with the abstract WGUI class to present those information to the user.
 * At the same time, it also is a simple tree structure, allowing the programmer to arrange complex sub progress. This is
 * especially useful if several time-consuming tasks need to be performed.
 *
 * \see WGUI
 */
class WProgress
{
public:

    /**
     * Creates a new progress instance as child of the specified progress. The instance is instantly marked "running".
     *
     * \param parent parent progress or NULL if there is no parent.
     * \param name   name of the progress, can be empty.
     * \param count  value denoting the final value.
     *
     * \note reaching the count does not automatically stop the progress. You still need to call stop().
     */
    WProgress( std::string name, unsigned int count = 0 );

    /**
     * Destructor.
     */
    virtual ~WProgress();

    /**
     * Stops the progress. After finishing, the progress de-registers from its parent (if any).
     */
    virtual void finish();

    /**
     * Simple increment operator to signal a forward stepping.
     *
     * \note this actually is for ++p. p++ is not useful since it returns a copy of WProgress with the old count.
     *
     * \return the incremented WProgress instance.
     */
    virtual WProgress& operator++();

    /**
     * Returns the overall progress of this progress instance, including the child progress'.
     *
     * \return the progress.
     */
    virtual float getProgress();

    /**
     * Returns true when the operation is pending. After calling finish() this will always return false.
     *
     * \return true if not finished.
     */
    virtual bool isPending();

    /**
     * Returns the name of the progress.
     *
     * \return name
     */
    std::string getName() const;

    /**
     * Function updating the internal state. This needs to be called before any get function to ensure the getter return the right
     * values.
     */
    virtual void update();

protected:

    /**
     * Progress name. Can be set only once (during construction).
     */
    std::string m_name;

    /**
     * The maximum count (which marks the 100%).
     */
    unsigned int m_max;

    /**
     * The current counter.
     */
    unsigned int m_count;

    /**
     * Flag denoting whether the progress is running or not.
     */
    bool m_pending;

private:
};

#endif  // WPROGRESS_H

