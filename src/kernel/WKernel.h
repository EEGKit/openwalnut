//---------------------------------------------------------------------------
//
// Project: OpenWalnut
//
// Copyright 2009 SomeCopyrightowner
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

#ifndef WKERNEL_H
#define WKERNEL_H

#include <list>

#include <boost/shared_ptr.hpp>

#include "WModule.h"
#include "../graphicsEngine/WGraphicsEngine.h"

/**
 * \par Description:
 * OpenWalnut kernel, managing modules and interaction between GUI, GE and DataHandler
 */
class WKernel
{
public:

    /**
     * \par Description
     * Default constructor. Also initializes Graphics Engine.
     */
    WKernel();

    /**
     * \par Description
     * Destructor.
     */
    virtual ~WKernel();

    /**
     * \par Description
     * Copy constructor
     * \param other Reference on object to copy.
     */
    WKernel( const WKernel& other );

protected:

    /**
     * \par Description
     * All the loaded modules.
     */
    std::list<WModule> m_modules;

    /**
     * \par Description
     * Pointer to an initialized graphics engine.
     */
    boost::shared_ptr<WGraphicsEngine> m_GraphicsEngine;

private:
    /**
     * \par Description
     * Loads all the modules it can find.
     * \exception
     */
    void loadModules();

    /**
     * \par Description
     * Initializes the graphics engine, data handler and so on.
     */
    void init();
};

#endif  // WKERNEL_H

