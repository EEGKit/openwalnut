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

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <boost/lexical_cast.hpp>

#include "WLoaderEEGASCII.h"
#include "../WSubject.h"
#include "../WEEG.h"
#include "../../common/WException.h"
#include "../../common/WStringUtils.hpp"

WLoaderEEGASCII::WLoaderEEGASCII( std::string fileName, boost::shared_ptr< WDataHandler > dataHandler )
    : WLoader( fileName ),
      m_dataHandler( dataHandler )
{
}

void WLoaderEEGASCII::operator()()
{
    std::ifstream in( m_fileName.c_str() );
    if( in.fail() )
        throw WException( "Could not read file \"" + m_fileName + "\"" );

    std::string tmp;
    getline( in, tmp );
    std::vector< std::string > tokens = string_utils::tokenize( tmp );

    const unsigned int nbChannels = tokens.size() / 3;
    WEEGChannelLabels labels( nbChannels );
    for( unsigned int i = 0; i < nbChannels; ++i )
    {
        labels[i].first = tokens[3*i] + " " + tokens[3*i+1] + " " + tokens[3*i+2];
        // TODO(wiebel): set second channel
    }

    WEEGSegmentArray segments( 1 );
    segments[0].clear();
    for( unsigned int i = 0; i < nbChannels; ++i )
    {
        segments[0].push_back( WEEGElectrode( 0 ) );
    }

    getline( in, tmp );
    while( !in.eof() )
    {
        tokens = string_utils::tokenize( tmp );
        assert( tokens.size() == nbChannels );
        for( unsigned int i = 0; i < nbChannels; ++i )
        {
            segments[0][i].push_back(  boost::lexical_cast< double >( tokens[i].c_str() ) );
        }
        getline( in, tmp );
    }

    in.close();


    WEEGElectrodeLibrary lib;  // TODO(wiebel): this is a dummy

    boost::shared_ptr< WEEG > eeg = boost::shared_ptr< WEEG >( new WEEG( segments, lib, labels ) );

    // TODO(wiebel): this is a dummy implementation. We need to fix
    // this as soon as we can distinguish which data belongs to which subject.
    boost::shared_ptr< WSubject > subject;
    if( m_dataHandler->getNumberOfSubjects() == 0 )
    {
        subject = boost::shared_ptr< WSubject >( new WSubject );
        m_dataHandler->addSubject( subject );
    }
    else
    {
        subject = m_dataHandler->getSubject( 0 );
    }
    eeg->setFileName( m_fileName );
    subject->addDataSet( eeg );
}
