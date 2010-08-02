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

#ifndef WTHREADEDFUNCTION_H
#define WTHREADEDFUNCTION_H

#include <memory.h>

#include <vector>
#include <boost/thread.hpp>

#include "WAssert.h"
#include "WWorkerThread.h"
#include "WSharedObject.h"

//! an enum indicating the status of a multithreaded computation
enum WThreadedFunctionStatus
{
    W_THREADS_INITIALIZED,      // the status after constructing the function
    W_THREADS_RUNNING,          // the threads were started
    W_THREADS_STOP_REQUESTED,   // a stop was requested and not all threads have stopped yet
    W_THREADS_ABORTED,          // at least one thread was aborted due to a stop request or an exception
    W_THREADS_FINISHED          // all threads completed their work successfully
};

/**
 * \class WThreadedFunction
 *
 * Creates threads that compute a function in a multithreaded fashion.
 *
 * \note This class is NOT thread-safe, do not access it from different threads simultaneously.
 */
template< class Function_T >
class WThreadedFunction
{
    //! a type for exception signals
    typedef boost::signal< void ( WException const& ) > ExceptionSignal;

public:

    //! a type for exception callbacks
    typedef boost::function< void ( WException const& ) > ExceptionFunction;

    /**
     * Creates the thread pool with a given number of threads.
     *
     * \param numThreads The number of threads to create.
     * \param function The function object.
     *
     * \note If the number of threads equals 0, a good number of threads will be determined by the threadpool.
     */
    WThreadedFunction( std::size_t numThreads, boost::shared_ptr< Function_T > function );

    /**
     * Destroys the thread pool and stops all threads, if any one of them is still running.
     *
     * \note Of course, the client has to make sure the threads do not work endlessly on a single job.
     */
    ~WThreadedFunction();

    /**
     * Starts the threads.
     */
    void run();

    /**
     * Request all threads to stop. Returns immediately, so you might
     * have to wait() for the threads to actually finish.
     */
    void stop();

    /**
     * Wait for all threads to stop.
     */
    void wait();

    /**
     * Get the status of the threads.
     *
     * \return The current status.
     */
    WThreadedFunctionStatus status();

    /**
     * Returns a condition that gets fired when all threads have finished.
     *
     * \return The condition indicating all threads are done.
     */
    boost::shared_ptr< WCondition > getThreadsDoneCondition();

    /**
     * Subscribe a function to an exception signal.
     *
     * \param func The function to subscribe.
     */
    void subscribeExceptionSignal( ExceptionFunction func );

private:
    /**
     * WThreadedFunction is non-copyable, so the copy constructor is not implemented.
     */
    WThreadedFunction( WThreadedFunction const& ); // NOLINT

    /**
     * WThreadedFunction is non-copyable, so the copy operator is not implemented.
     */
    WThreadedFunction& operator = ( WThreadedFunction const& );

    /**
     * This function gets subscribed to the threads' stop signals.
     */
    void handleThreadDone();

    /**
     * This function handles exceptions thrown in the worker threads.
     *
     * \param e The exception that was thrown.
     */
    void handleThreadException( WException const& e );

    //! the number of threads to manage
    std::size_t m_numThreads;

    //! the threads
    // use shared_ptr here, because WWorkerThread is non-copyable
    std::vector< boost::shared_ptr< WWorkerThread< Function_T > > > m_threads;

    //! the function object
    boost::shared_ptr< Function_T > m_func;

    //! a condition that gets notified when the work is complete
    boost::shared_ptr< WCondition > m_doneCondition;

    //! a counter that keeps track of how many threads have finished
    WSharedObject< std::size_t > m_threadsDone;

    //! a signal for exceptions
    ExceptionSignal m_exceptionSignal;

    //! the current status
    WSharedObject< WThreadedFunctionStatus > m_status;
};

template< class Function_T >
WThreadedFunction< Function_T >::WThreadedFunction( std::size_t numThreads, boost::shared_ptr< Function_T > function )
    : m_numThreads( numThreads ),
      m_threads(),
      m_func( function ),
      m_doneCondition( new WCondition ),
      m_threadsDone(),
      m_exceptionSignal(),
      m_status()
{
    if( !m_func )
    {
        throw WException( "No valid thread function pointer." );
    }

    // find a suitable number of threads
    if( m_numThreads == 0 )
    {
        m_numThreads = 1;
        while( m_numThreads < boost::thread::hardware_concurrency() / 2 && m_numThreads < 1024 )
        {
            m_numThreads *= 2;
        }
    }

    // set number of finished threads to 0
    m_threadsDone.getWriteTicket()->get() = 0;

    // set initial status
    m_status.getWriteTicket()->get() = W_THREADS_INITIALIZED;

    // create threads
    for( std::size_t k = 0; k < m_numThreads; ++k )
    {
        boost::shared_ptr< WWorkerThread< Function_T > > t( new WWorkerThread< Function_T >( m_func, k, m_numThreads ) );
        t->subscribeStopSignal( boost::bind( &WThreadedFunction::handleThreadDone, this ) );
        t->subscribeExceptionSignal( boost::bind( &WThreadedFunction::handleThreadException, this, _1 ) );
        m_threads.push_back( t );
    }
}

template< class Function_T >
WThreadedFunction< Function_T >::~WThreadedFunction()
{
    stop();
    m_exceptionSignal.disconnect_all_slots();
}

template< class Function_T >
void WThreadedFunction< Function_T >::run()
{
    // set the number of finished threads to 0
    m_threadsDone.getWriteTicket()->get() = 0;
    // change status
    m_status.getWriteTicket()->get() = W_THREADS_RUNNING;
    // start threads
    for( std::size_t k = 0; k < m_numThreads; ++k )
    {
        m_threads[ k ]->run();
    }
}

template< class Function_T >
void WThreadedFunction< Function_T >::stop()
{
    // change status
    m_status.getWriteTicket()->get() = W_THREADS_STOP_REQUESTED;

    typename std::vector< boost::shared_ptr< WWorkerThread< Function_T > > >::iterator it;
    // tell the threads to stop
    for( it = m_threads.begin(); it != m_threads.end(); ++it )
    {
        ( *it )->requestStop();
    }
}

template< class Function_T >
void WThreadedFunction< Function_T >::wait()
{
    typename std::vector< boost::shared_ptr< WWorkerThread< Function_T > > >::iterator it;
    // wait for the threads to stop
    for( it = m_threads.begin(); it != m_threads.end(); ++it )
    {
        ( *it )->wait();
    }
}

template< class Function_T >
WThreadedFunctionStatus WThreadedFunction< Function_T >::status()
{
    return m_status.getReadTicket()->get();
}

template< class Function_T >
boost::shared_ptr< WCondition > WThreadedFunction< Function_T >::getThreadsDoneCondition()
{
    return m_doneCondition;
}

template< class Function_T >
void WThreadedFunction< Function_T >::subscribeExceptionSignal( ExceptionFunction func )
{
    if( func )
    {
        m_exceptionSignal.connect( func );
    }
}

template< class Function_T >
void WThreadedFunction< Function_T >::handleThreadDone()
{
    typedef typename WSharedObject< std::size_t >::WriteTicket WT;

    WT t = m_threadsDone.getWriteTicket();
    WAssert( t->get() < m_numThreads, "" );
    ++t->get();
    std::size_t k = t->get();
    t = WT();

    if( m_numThreads == k )
    {
        typedef typename WSharedObject< WThreadedFunctionStatus >::WriteTicket ST;
        ST s = m_status.getWriteTicket();
        if( s->get() == W_THREADS_RUNNING )
        {
            s->get() = W_THREADS_FINISHED;
        }
        else if( s->get() == W_THREADS_STOP_REQUESTED )
        {
            s->get() = W_THREADS_ABORTED;
        }
        else
        {
            throw WException( "Invalid status change." );
        }
        m_doneCondition->notify();
    }
}

template< class Function_T >
void WThreadedFunction< Function_T >::handleThreadException( WException const& e )
{
    // change status
    typedef typename WSharedObject< WThreadedFunctionStatus >::WriteTicket WT;
    WT w = m_status.getWriteTicket();
    WAssert( w->get() != W_THREADS_FINISHED &&
             w->get() != W_THREADS_ABORTED, "" );
    if( w->get() == W_THREADS_RUNNING )
    {
        w->get() = W_THREADS_STOP_REQUESTED;
    }
    // force destruction of the write ticket
    w = WT();
    // update the number of finished threads
    handleThreadDone();

    m_exceptionSignal( e );
}

#endif  // WTHREADEDFUNCTION_H
