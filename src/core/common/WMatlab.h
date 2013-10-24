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

#ifndef WMATLAB_H
#define WMATLAB_H

#include <engine.h>

#include <string>

#include "core/common/math/WMatrix.h"

// TODO(philips): use forward declaration
// forward declaration
// typedef struct Engine;

/**
 * Class to interfacing matlab from open walnut.
 */
class WMatlab
{
public:
    /**
     * Default constructor.
     */
    explicit WMatlab( const std::string startCommand = "" );

    /**
     * Destructor.
     */
    ~WMatlab();

    /**
     *
     */
    void putVariable( const std::string& varName, const WMatrix<double>& matrix );

    /**
     *
     */
    void putVariable( const std::string& varName, const WValue<double>& vector );

    /**
     *
     */
    void putVariable( const std::string& varName, const WVector3d& vector );

    /**
     *
     */
    void putVariable( const std::string& varName, double val );

    /**
     *
     */
    void putVariable( const std::string& varName, mxArray* matlabArray );

    /**
     *
     */
    WMatrix<double> getVariable( const std::string& varName ) const;

    /**
     *
     */
    double getScalarVariable( const std::string& varName ) const;

    /**
     *
     */
    void evalString( const std::string& function );

    Engine* getEngine();

protected:
private:
    Engine* m_engine;
};

inline Engine* WMatlab::getEngine()
{
    return m_engine;
}

#endif  // WMATLAB_H
