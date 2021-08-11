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

#include <memory>
#include <string>

#include "../common/WAssert.h"
#include "../common/WCondition.h"
#include "../common/WTransferable.h"
#include "WDataSet.h"
#include "WDataSetVector.h"
#include "WDataTexture3D.h"
#include "exceptions/WDHException.h"

// prototype instance as singleton
std::shared_ptr< WPrototyped > WDataSet::m_prototype = std::shared_ptr< WPrototyped >();

WDataSet::WDataSet()
    : WTransferable(),
    m_properties( std::shared_ptr< WProperties >( new WProperties( "Data-Set Properties", "Properties of a data-set" ) ) ),
    m_infoProperties( std::shared_ptr< WProperties >( new WProperties( "Data-Set Info Properties", "Data-set's information properties" ) ) ),
    m_filename( "" )
{
    m_infoProperties->setPurpose( PV_PURPOSE_INFORMATION );
}

void WDataSet::setFileName( const std::string filename )
{
    setFilename( filename );
}

std::string WDataSet::getFileName() const
{
    return getFilename();
}

void WDataSet::setFilename( const std::string filename )
{
    WAssert( filename != "", "No filename set for data set." );
    m_filename = filename;
}

std::string WDataSet::getFilename() const
{
    return m_filename;
}

bool WDataSet::isTexture() const
{
    return false;
}

osg::ref_ptr< WDataTexture3D > WDataSet::getTexture() const
{
    throw WDHException( std::string( "This dataset does not provide a texture." ) );
}

const std::string WDataSet::getName() const
{
    return "WDataSet";
}

const std::string WDataSet::getDescription() const
{
    return "Encapsulates the whole common feature set of all datasets.";
}

std::shared_ptr< WPrototyped > WDataSet::getPrototype()
{
    if( !m_prototype )
    {
        m_prototype = std::shared_ptr< WPrototyped >( new WDataSet() );
    }

    return m_prototype;
}

std::shared_ptr< WDataSetVector > WDataSet::isVectorDataSet()
{
    return std::shared_ptr< WDataSetVector >();
}

std::shared_ptr< WProperties > WDataSet::getProperties() const
{
    return m_properties;
}

std::shared_ptr< WProperties > WDataSet::getInformationProperties() const
{
    return m_infoProperties;
}

