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

#ifndef WEEG_H
#define WEEG_H

#include <map>
#include <string>
#include <vector>
#include "WRecording.h"
#include "../common/WPrototyped.h"
#include "../common/math/linearAlgebra/WLinearAlgebra.h"
#include "WExportDataHandler.h"


///======================================
// TODO(wiebel): use this stuff or remove it
typedef double WDummyType;

/**
 * An incomplete implementation to store information about electrodes of EEG data
 */
class OWDATAHANDLER_EXPORT WEEGElectrodeObject // NOLINT
{
public:
    /**
     * Contructor taking the position of the elctrode.
     * \param position The position of the electrode in world space.
     */
    explicit WEEGElectrodeObject( WPosition_2 position );

    /**
     * Returns the position of the electrode.
     */
    WPosition_2 getPosition() const;
protected:
private:
    WPosition_2 m_position; //!< Position of the electrode in space
    // TODO(wiebel): The following is suggested in the
    // eemagine design document
    // WColor color;
};

/**
 * An incomplete dummy implementation to store information about segments of EEG data
 */
class WEEGSegementInfo
{
public:
protected:
private:
    WDummyType m_startSample; //!< Dummy variable of not yet completely implemented class
    WDummyType m_endSample; //!< Dummy variable of not yet completely implemented class
    WDummyType m_segmentStartDate; //!< Dummy variable of not yet completely implemented class
};
//================================================

typedef std::vector< double > WEEGElectrode;
typedef std::vector< WEEGElectrode > WEEGSegment;
typedef std::vector< WEEGSegment > WEEGSegmentArray;

typedef std::vector< WEEGElectrodeObject > WEEGElectrodeLibrary;
typedef std::vector< std::pair< std::string, std::string > > WEEGChannelLabels;
/**
 * Contains EEG recording data.
 * \ingroup dataHandler
 */
class OWDATAHANDLER_EXPORT WEEG : public WRecording // NOLINT
{
public:


    /**
     * Constructs a WEEG object from the give infos.
     * \param data Array of segments
     * \param electrodeLib Information about the electrodes
     * \param channelLabels The names of the channels.
     */
    explicit WEEG( const WEEGSegmentArray& data,
                   const WEEGElectrodeLibrary& electrodeLib,
                   const WEEGChannelLabels& channelLabels );

    /**
     * Constructor creating a quite unusable instance. Useful for prototype mechanism.
     */
    WEEG();

    /**
     * Access operator for single samples.
     * \param segment id of segement to access
     * \param signal id of signal to access
     * \param sample id of sample to access
     */
    const double& operator()( size_t segment, size_t signal, size_t sample ) const;

    /**
     * Returns number of samples of a given segment.
     * \param segmentId id of segment beeing inspected.
     */
    size_t getNumberOfSamples( size_t segmentId ) const;

    /**
     * Return the number of channels this EEG has.
     */
    size_t getNumberOfChannels() const;

    /**
     * Return the number of segments this EEG consists of.
     */
    size_t getNumberOfSegments() const;

    /**
     * Return the label of a certain channel.
     * \param channelId id of channel beeing inspected.
     */
    std::string getChannelLabel( size_t channelId ) const;

    /**
     * Return the position of the sensor for a certain channel.
     * \param channelId id of channel beeing inspected.
     */
    WPosition_2 getChannelPosition( size_t channelId ) const;

    /**
     * Determines whether this dataset can be used as a texture.
     *
     * \return true if usable as texture.
     */
    virtual bool isTexture() const;

    /**
     * Gets the name of this prototype.
     *
     * \return the name.
     */
    virtual const std::string getName() const;

    /**
     * Gets the description for this prototype.
     *
     * \return the description
     */
    virtual const std::string getDescription() const;

    /**
     * Returns a prototype instantiated with the true type of the deriving class.
     *
     * \return the prototype.
     */
    static boost::shared_ptr< WPrototyped > getPrototype();

protected:

    /**
     * The prototype as singleton.
     */
    static boost::shared_ptr< WPrototyped > m_prototype;

private:
    /**
     * We have only on sampling rate for all channels.
     */
    double m_samplingRate;
    /**
     * Information about segments. An array of WEEGSegmentInfo objects.
     */
    std::vector< WEEGSegementInfo > m_segmentInfo;
    /**
     * Description of electrodes
     */
    std::map< std::string, size_t > m_electrodeDescriptions;

    /**
     * Information about the electrodes.
     */
    WEEGElectrodeLibrary m_electrodeLibrary;

    /**
     * Contains the EEG data as an arry of segements
     * of data which consist of an array of electrodes
     * which again consist of an array of samples over time.
     */
    WEEGSegmentArray m_segments;

    /**
     * Label for each channel.
     */
    WEEGChannelLabels m_channelLabels;

    /**
     * Is the channel enabled?
     */
    std::vector< bool > m_channelEnabled;
};

inline const double& WEEG::operator()( size_t segment, size_t signal, size_t sample ) const
{
    return m_segments[segment][signal][sample];
}

inline size_t WEEG::getNumberOfSamples( size_t segmentId ) const
{
    return m_segments[segmentId][0].size();
}

inline size_t WEEG::getNumberOfChannels() const
{
    return m_segments[0].size();
}

inline size_t WEEG::getNumberOfSegments() const
{
    return m_segments.size();
}

inline std::string WEEG::getChannelLabel( size_t channelId ) const
{
    // TODO(wiebel): what is done with the second string of the label?
    return  m_channelLabels[channelId].first;
}

inline WPosition_2 WEEG::getChannelPosition( size_t channelId ) const
{
    return  m_electrodeLibrary[channelId].getPosition();
}

#endif  // WEEG_H
