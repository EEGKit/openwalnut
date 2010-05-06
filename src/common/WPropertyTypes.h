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

#ifndef WPROPERTYTYPES_H
#define WPROPERTYTYPES_H

#include <stdint.h>

#include <string>
#include <list>
#include <utility>

#include <boost/filesystem.hpp>

#include "math/WPosition.h"
#include "WColor.h"

template < typename T >
class WPropertyVariable;
class WProperties;

////////////////////////////////////////////////////////////////////////////////////////////////////////
// NOTE: If you add new types here, please also add corresponding addProperty methods to WProperties
////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////
// NOTE: Always use the WPVBaseTypes in all your declarations to allow easy type modifications later on
////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Enum of all possible types, that can be used with WProperty.
 */
typedef enum
{
    PV_UNKNOWN,        //!< type not known
    PV_GROUP,          //!< the group property
    PV_INT,            //!< integer value
    PV_DOUBLE,         //!< floating point value
    PV_BOOL,           //!< boolean
    PV_STRING,         //!< a string
    PV_PATH,           //!< a Boost Path object denoting a filename/path
    PV_SELECTION,      //!< a list of strings, selectable
    PV_POSITION,       //!< a position property
    PV_COLOR,          //!< a color property
    PV_TRIGGER         //!< for triggering an event
}
PROPERTY_TYPE;

/**
 * Enum of all possible purpose of a property. The purpose describes which meaning a property has for the creator of it. A PP_PARAMETER is a
 * property which is meant to be modified to adopt the behaviour of the module (or whomever has created it). A PP_INFORMATION is only an output
 * from the creator who wants to inform the outside world about values, states or whatever.
 */
typedef enum
{
    PV_PURPOSE_INFORMATION,     //!< information property not meant to be modified from someone (except the creating object)
    PV_PURPOSE_PARAMETER        //!< a parameter meant to be modified by others to manipulate the behaviour of the module (or whomever created
                                //!< the property)
}
PROPERTY_PURPOSE;

/**
 * Namespace containing all base types of the WPropertyVariables. Use these types instead of issuing int32_t, double, bool, ...
 * directly. It also contains some user defined types including the needed operators.
 *
 * \note You can use only types which overwrite the << and >> operators!
 */
namespace WPVBaseTypes
{
    typedef int32_t                                         PV_INT;         //!< base type used for every WPVInt
    typedef double                                          PV_DOUBLE;      //!< base type used for every WPVDouble
    typedef bool                                            PV_BOOL;        //!< base type used for every WPVBool
    typedef std::string                                     PV_STRING;      //!< base type used for every WPVString
    typedef boost::filesystem::path                         PV_PATH;        //!< base type used for every WPVFilename
    typedef void*                                           PV_SELECTION;   //!< base type used for every WPVSelection
    typedef wmath::WPosition                                PV_POSITION;    //!< base type used for every WPVPosition
    typedef WColor                                          PV_COLOR;       //!< base type used for every WPVColor

    /**
     * Enum denoting the possible trigger states. It is used for trigger properties.
     */
    typedef enum
    {
        PV_TRIGGER_READY = 0,                                               //!< Trigger property: is ready to be triggered (again)
        PV_TRIGGER_TRIGGERED                                                //!< Trigger property: got triggered
    }
                                                            PV_TRIGGER;     //!< base type used for every WPVTrigger

    /**
     * Write a PV_TRIGGER in string representation to the given output stream.
     *
     * \param out the output stream to print the value to
     * \param c the trigger value to output
     *
     * \return the output stream extended by the trigger value.
     */
    std::ostream& operator<<( std::ostream& out, const PV_TRIGGER& c );

    /**
     * Write a PV_TRIGGER in string representation to the given input stream.
     *
     * \param in the input stream to read the value from
     * \param c  set the value red to this
     *
     * \return the input stream.
     */
    std::istream& operator>>( std::istream& in, PV_TRIGGER& c );
}

/**
 * Some convenience type alias for a even more easy usage of WPropertyVariable.
 * These typedefs are useful for casts, as they alias the PropertyVariable types. Please use these types instead of directly
 * int32_t, double, bool, ... so we are able to change the type later on without modifications of thousands of modules.
 */

/**
 * Group properties.
 */
typedef WProperties WPVGroup;

/**
 * Int properties.
 */
typedef WPropertyVariable< WPVBaseTypes::PV_INT > WPVInt;

/**
 * Floating point properties.
 */
typedef WPropertyVariable< WPVBaseTypes::PV_DOUBLE > WPVDouble;

/**
 * Boolean properties.
 */
typedef WPropertyVariable< WPVBaseTypes::PV_BOOL > WPVBool;

/**
 * String properties.
 */
typedef WPropertyVariable< WPVBaseTypes::PV_STRING > WPVString;

/**
 * Filename properties.
 */
typedef WPropertyVariable< WPVBaseTypes::PV_PATH > WPVFilename;

/**
 * Selection properties
 */
typedef WPropertyVariable< WPVBaseTypes::PV_SELECTION > WPVSelection;

/**
 * position (vec3d) properties
 */
typedef WPropertyVariable< WPVBaseTypes::PV_POSITION > WPVPosition;

/**
 * Color properties
 */
typedef WPropertyVariable< WPVBaseTypes::PV_COLOR > WPVColor;

/**
 * Trigger properties
 */
typedef WPropertyVariable< WPVBaseTypes::PV_TRIGGER > WPVTrigger;

/**
 * Some convenience type alias for a even more easy usage of WPropertyVariable.
 * These typdefs define some pointer alias.
 */

/**
 * Alias for int32_t property variables.
 */
typedef boost::shared_ptr< WPVInt > WPropInt;

/**
 * Alias for int32_t property variables.
 */
typedef boost::shared_ptr< WPVDouble > WPropDouble;

/**
 * Alias for bool property variables.
 */
typedef boost::shared_ptr< WPVBool > WPropBool;

/**
 * Alias for string property variables.
 */
typedef boost::shared_ptr< WPVString > WPropString;

/**
 * Alias for filename property variables.
 */
typedef boost::shared_ptr< WPVFilename > WPropFilename;

/**
 * Alias for string list property variables.
 */
typedef boost::shared_ptr< WPVSelection > WPropSelection;

/**
 * Alias for position property variables.
 */
typedef boost::shared_ptr< WPVPosition > WPropPosition;

/**
 * Alias for color property variables.
 */
typedef boost::shared_ptr< WPVColor > WPropColor;

/**
 * Alias for the group properties.
 */
typedef boost::shared_ptr< WPVGroup > WPropGroup;

/**
 * Alias for the trigger properties.
 */
typedef boost::shared_ptr< WPVTrigger > WPropTrigger;

/**
 * This namespace contains several helper classes which translate their template type to an enum.
 */
namespace PROPERTY_TYPE_HELPER
{
    /**
     * Class helping to adapt types specified as template parameter into an enum.
     */
    template< typename T >
    class WTypeIdentifier
    {
    public:
        /**
         * Get type identifier of the template type T.
         *
         * \return type identifier-
         */
        PROPERTY_TYPE getType()
        {
            return PV_UNKNOWN;
        }
    };

    /**
     * Class helping to adapt types specified as template parameter into an enum.
     */
    template<>
    class WTypeIdentifier< WPVBaseTypes::PV_BOOL >
    {
    public:
        /**
         * Get type identifier of the template type T.
         *
         * \return type identifier-
         */
        PROPERTY_TYPE getType()
        {
            return PV_BOOL;
        }
    };

    /**
     * Class helping to adapt types specified as template parameter into an enum.
     */
    template<>
    class WTypeIdentifier< WPVBaseTypes::PV_INT >
    {
    public:
        /**
         * Get type identifier of the template type T.
         *
         * \return type identifier-
         */
        PROPERTY_TYPE getType()
        {
            return PV_INT;
        }
    };

    /**
     * Class helping to adapt types specified as template parameter into an enum.
     */
    template<>
    class WTypeIdentifier< WPVBaseTypes::PV_DOUBLE >
    {
    public:
        /**
         * Get type identifier of the template type T.
         *
         * \return type identifier-
         */
        PROPERTY_TYPE getType()
        {
            return PV_DOUBLE;
        }
    };

    /**
     * Class helping to adapt types specified as template parameter into an enum.
     */
    template<>
    class WTypeIdentifier< WPVBaseTypes::PV_STRING >
    {
    public:
        /**
         * Get type identifier of the template type T.
         *
         * \return type identifier-
         */
        PROPERTY_TYPE getType()
        {
            return PV_STRING;
        }
    };

    /**
     * Class helping to adapt types specified as template parameter into an enum.
     */
    template<>
    class WTypeIdentifier< WPVBaseTypes::PV_PATH >
    {
    public:
        /**
         * Get type identifier of the template type T.
         *
         * \return type identifier-
         */
        PROPERTY_TYPE getType()
        {
            return PV_PATH;
        }
    };

    /**
     * Class helping to adapt types specified as template parameter into an enum.
     */
    template<>
    class WTypeIdentifier< WPVBaseTypes::PV_SELECTION >
    {
    public:
        /**
         * Get type identifier of the template type T.
         *
         * \return type identifier-
         */
        PROPERTY_TYPE getType()
        {
            return PV_SELECTION;
        }
    };

    /**
     * Class helping to adapt types specified as template parameter into an enum.
     */
    template<>
    class WTypeIdentifier< WPVBaseTypes::PV_POSITION >
    {
    public:
        /**
         * Get type identifier of the template type T.
         *
         * \return type identifier-
         */
        PROPERTY_TYPE getType()
        {
            return PV_POSITION;
        }
    };

    /**
     * Class helping to adapt types specified as template parameter into an enum.
     */
    template<>
    class WTypeIdentifier< WPVBaseTypes::PV_COLOR >
    {
    public:
        /**
         * Get type identifier of the template type T.
         *
         * \return type identifier-
         */
        PROPERTY_TYPE getType()
        {
            return PV_COLOR;
        }
    };

    /**
     * Class helping to adapt types specified as template parameter into an enum.
     */
    template<>
    class WTypeIdentifier< WPVBaseTypes::PV_TRIGGER >
    {
    public:
        /**
         * Get type identifier of the template type T.
         *
         * \return type identifier-
         */
        PROPERTY_TYPE getType()
        {
            return PV_TRIGGER;
        }
    };
}

#endif  // WPROPERTYTYPES_H
