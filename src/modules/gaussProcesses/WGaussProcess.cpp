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


#include "core/common/datastructures/WFiber.h"
#include "core/common/WAssert.h"
#include "../../ext/Eigen/QR"
#include "WGaussProcess.h"

WGaussProcess::WGaussProcess( const size_t tractID,
                              boost::shared_ptr< const WDataSetFibers > tracts,
                              boost::shared_ptr< const WDataSetDTI > tensors,
                              double maxLevel )
    : m_tractID( tractID ),
      m_tracts( tracts ),
      m_tensors( tensors ),
      m_maxLevel( maxLevel )
{
    WFiber tract = generateTract();
    m_Cff_1_l_product = Eigen::VectorXd( static_cast< int >( tract.size() ) );
    m_R = 2.0 * maxSegmentLength( tract );
    m_Cff_1_l_product = generateCffInverse( tract ) * ( Eigen::VectorXd::Ones( m_Cff_1_l_product.size() ) * m_maxLevel );
    generateTauParameter();
    m_bb = computeBoundingBox( tract );
}

WGaussProcess::~WGaussProcess()
{
}

double WGaussProcess::mean( const WPosition& p ) const
{
    Eigen::VectorXd Sf( m_Cff_1_l_product.size() );

    // for further improvement we could work with the indices of the arrays inside of the WDataSetFibers instead of building up this point vector
    WFiber tract = generateTract();

    for( size_t i = 0; i < tract.size(); ++i )
    {
        Sf( i ) = cov_s( tract[i], p );
    }

    return Sf.dot( m_Cff_1_l_product );
}

WFiber WGaussProcess::generateTract() const
{
    return ( *m_tracts )[ m_tractID ];
}

Eigen::MatrixXd WGaussProcess::generateCffInverse( const WFiber& tract )
{
    Eigen::MatrixXd Cff( static_cast< int >( tract.size() ), static_cast< int >( tract.size() ) );
    size_t i = 0, j = 0;
    for( WFiber::const_iterator cit = tract.begin(); cit != tract.end(); ++cit, ++i )
    {
        for( WFiber::const_iterator cit2 = tract.begin(); cit2 != tract.end(); ++cit2, ++j )
        {
            Cff( i, j ) = cov( *cit, *cit2 );
        }
        j = 0; // reset every loop!
    }

    // Note: If Cff is constructed via a positive definite function itself is positive definite,
    // hence invertible
    Eigen::ColPivHouseholderQR< Eigen::MatrixXd > qrOfCff( Cff );
    return qrOfCff.inverse();
}

double WGaussProcess::generateTauParameter()
{
    double result = 0.0;

    // According to Demian this function is very complex, involing tensor interpolation (not
    // component wise) which is not trivial, but the out come does not contribute significantly to
    // the result, so I ommit the implementation at first.
    //
    // WTensorSym< 2, 3 > t = tensors->interpolate( *cit );
    // it may occur due to interpolation and noise that negative eigenvalues will occour!
    // double lambda_1 = 0.0; // = t.eigenvalues(
    // newTau = m_R / std::sqrt( lambda_1 );

    return result;
}

double WGaussProcess::cov_d( const WPosition& /* p1 */, const WPosition& /* p2 */ ) const
{
    // According to Demian this function is very complex, involing tensor interpolation (not
    // component wise) which is not trivial, but the out come does not contribute significantly to
    // the result, so I ommit the implementation at first.
    return 0.0;
}

const WBoundingBox& WGaussProcess::getBB() const
{
    return m_bb;
}

/**
 * This namespace is to prevent this method beeing public accessible. It should be used only inside here!
 */
namespace
{
    /**
     * This implements the integral over the intersecton of the two shperes defined by the base
     * points of the two fibers and the maximal segement length. It is generated by Maple^TM and
     * therefore not really readable.
     *
     * \param w The distance between the two base points
     * \param Q The radius of the first sphere
     * \param R The radius of the second sphere
     *
     * \return The integral as defined in eq. (19) of the Demian Wasserman paper.
     */
    double covIntegralThinPlateR3Normalized( const double w, const double Q, const double R )
    {
        if( w == 0.0e0 )
            return(0.3141592654e1 * pow(Q, 0.9e1) / 0.9e1 + (-0.9e1 / 0.35e2 * 0.3141592654e1 * pow(Q, 0.8e1) + 0.4e1 / 0.15e2 * 0.3141592654e1 * pow(Q, 0.6e1) * R * R) * R); // NOLINT line length
        else if (w <= R - Q)
            if (w <= Q / 0.2e1)
                return(-0.4e1 / 0.1575e4 * 0.3141592654e1 * pow(w, 0.9e1) + (0.3141592654e1 * pow(w, 0.8e1) / 0.105e3 + (-0.4e1 / 0.105e3 * 0.3141592654e1 * pow(w, 0.6e1) + (0.6e1 / 0.25e2 * 0.3141592654e1 * pow(w, 0.4e1) + (0.4e1 / 0.7e1 * 0.3141592654e1 * w * w + 0.3141592654e1 * Q * Q / 0.9e1) * Q * Q) * Q * Q) * Q * Q) * Q + ((-0.4e1 / 0.5e1 * 0.3141592654e1 * w * w - 0.9e1 / 0.35e2 * 0.3141592654e1 * Q * Q) * pow(Q, 0.6e1) + 0.4e1 / 0.15e2 * 0.3141592654e1 * pow(Q, 0.6e1) * R * R) * R); // NOLINT line length
            else if (w < Q)
                return(-0.4e1 / 0.1575e4 * 0.3141592654e1 * pow(w, 0.9e1) + (0.3141592654e1 * pow(w, 0.8e1) / 0.105e3 + (-0.4e1 / 0.105e3 * 0.3141592654e1 * pow(w, 0.6e1) + (0.6e1 / 0.25e2 * 0.3141592654e1 * pow(w, 0.4e1) + (0.4e1 / 0.7e1 * 0.3141592654e1 * w * w + 0.3141592654e1 * Q * Q / 0.9e1) * Q * Q) * Q * Q) * Q * Q) * Q + ((-0.4e1 / 0.5e1 * 0.3141592654e1 * w * w - 0.9e1 / 0.35e2 * 0.3141592654e1 * Q * Q) * pow(Q, 0.6e1) + 0.4e1 / 0.15e2 * 0.3141592654e1 * pow(Q, 0.6e1) * R * R) * R); // NOLINT line length
            else if (Q <= w)
                return((0.8e1 / 0.15e2 * 0.3141592654e1 * pow(w, 0.3e1) + (0.12e2 / 0.35e2 * 0.3141592654e1 * w + 0.8e1 / 0.525e3 * 0.3141592654e1 / w * Q * Q) * Q * Q) * pow(Q, 0.6e1) + ((-0.4e1 / 0.5e1 * 0.3141592654e1 * w * w - 0.9e1 / 0.35e2 * 0.3141592654e1 * Q * Q) * pow(Q, 0.6e1) + 0.4e1 / 0.15e2 * 0.3141592654e1 * pow(Q, 0.6e1) * R * R) * R);  // NOLINT line length
            else
                return(0.0e0);
        else if (R - Q < w && w <= R)
            if (w <= Q / 0.2e1)
                return(-0.2e1 / 0.525e3 * 0.3141592654e1 * pow(w, 0.9e1) + (0.3141592654e1 * pow(w, 0.8e1) / 0.210e3 + (-0.2e1 / 0.105e3 * 0.3141592654e1 * pow(w, 0.6e1) + (0.3e1 / 0.25e2 * 0.3141592654e1 * pow(w, 0.4e1) + (-0.4e1 / 0.15e2 * 0.3141592654e1 * pow(w, 0.3e1) + (0.2e1 / 0.7e1 * 0.3141592654e1 * w * w + (-0.6e1 / 0.35e2 * 0.3141592654e1 * w + (0.3141592654e1 / 0.18e2 - 0.4e1 / 0.525e3 * 0.3141592654e1 / w * Q) * Q) * Q) * Q) * Q) * Q * Q) * Q * Q) * Q + (0.3141592654e1 * pow(w, 0.8e1) / 0.210e3 + (0.9e1 / 0.560e3 * 0.3141592654e1 * pow(w, 0.7e1) + (-0.3141592654e1 * pow(w, 0.5e1) / 0.20e2 + (0.9e1 / 0.40e2 * 0.3141592654e1 * pow(w, 0.3e1) + (-0.2e1 / 0.5e1 * 0.3141592654e1 * w * w + (0.9e1 / 0.28e2 * 0.3141592654e1 * w + (-0.9e1 / 0.70e2 * 0.3141592654e1 + 0.3141592654e1 / w * Q / 0.48e2) * Q) * Q) * Q) * Q * Q) * Q * Q) * Q + (-0.2e1 / 0.105e3 * 0.3141592654e1 * pow(w, 0.6e1) + (-0.3141592654e1 * pow(w, 0.5e1) / 0.20e2 + (0.3141592654e1 * pow(w, 0.3e1) / 0.12e2 + (-0.3e1 / 0.20e2 * 0.3141592654e1 * w + (0.2e1 / 0.15e2 * 0.3141592654e1 - 0.3141592654e1 / w * Q / 0.28e2) * Q) * Q * Q) * Q * Q) * Q + (0.3e1 / 0.25e2 * 0.3141592654e1 * pow(w, 0.4e1) + (0.9e1 / 0.40e2 * 0.3141592654e1 * pow(w, 0.3e1) + (-0.3e1 / 0.20e2 * 0.3141592654e1 * w + 0.9e1 / 0.200e3 * 0.3141592654e1 / w * Q * Q) * Q * Q) * Q + (-0.4e1 / 0.15e2 * 0.3141592654e1 * pow(w, 0.3e1) + (-0.2e1 / 0.5e1 * 0.3141592654e1 * w * w + 0.2e1 / 0.15e2 * 0.3141592654e1 * Q * Q) * Q + (0.2e1 / 0.7e1 * 0.3141592654e1 * w * w + (0.9e1 / 0.28e2 * 0.3141592654e1 * w - 0.3141592654e1 / w * Q * Q / 0.28e2) * Q + (-0.6e1 / 0.35e2 * 0.3141592654e1 * w - 0.9e1 / 0.70e2 * 0.3141592654e1 * Q + (0.3141592654e1 / 0.18e2 + 0.3141592654e1 / w * Q / 0.48e2 - 0.4e1 / 0.525e3 * 0.3141592654e1 / w * R) * R) * R) * R) * R) * R * R) * R * R) * R); // NOLINT line length
            else if (w < Q)
                return(-0.2e1 / 0.525e3 * 0.3141592654e1 * pow(w, 0.9e1) + (0.3141592654e1 * pow(w, 0.8e1) / 0.210e3 + (-0.2e1 / 0.105e3 * 0.3141592654e1 * pow(w, 0.6e1) + (0.3e1 / 0.25e2 * 0.3141592654e1 * pow(w, 0.4e1) + (-0.4e1 / 0.15e2 * 0.3141592654e1 * pow(w, 0.3e1) + (0.2e1 / 0.7e1 * 0.3141592654e1 * w * w + (-0.6e1 / 0.35e2 * 0.3141592654e1 * w + (0.3141592654e1 / 0.18e2 - 0.4e1 / 0.525e3 * 0.3141592654e1 / w * Q) * Q) * Q) * Q) * Q) * Q * Q) * Q * Q) * Q + (0.3141592654e1 * pow(w, 0.8e1) / 0.210e3 + (0.9e1 / 0.560e3 * 0.3141592654e1 * pow(w, 0.7e1) + (-0.3141592654e1 * pow(w, 0.5e1) / 0.20e2 + (0.9e1 / 0.40e2 * 0.3141592654e1 * pow(w, 0.3e1) + (-0.2e1 / 0.5e1 * 0.3141592654e1 * w * w + (0.9e1 / 0.28e2 * 0.3141592654e1 * w + (-0.9e1 / 0.70e2 * 0.3141592654e1 + 0.3141592654e1 / w * Q / 0.48e2) * Q) * Q) * Q) * Q * Q) * Q * Q) * Q + (-0.2e1 / 0.105e3 * 0.3141592654e1 * pow(w, 0.6e1) + (-0.3141592654e1 * pow(w, 0.5e1) / 0.20e2 + (0.3141592654e1 * pow(w, 0.3e1) / 0.12e2 + (-0.3e1 / 0.20e2 * 0.3141592654e1 * w + (0.2e1 / 0.15e2 * 0.3141592654e1 - 0.3141592654e1 / w * Q / 0.28e2) * Q) * Q * Q) * Q * Q) * Q + (0.3e1 / 0.25e2 * 0.3141592654e1 * pow(w, 0.4e1) + (0.9e1 / 0.40e2 * 0.3141592654e1 * pow(w, 0.3e1) + (-0.3e1 / 0.20e2 * 0.3141592654e1 * w + 0.9e1 / 0.200e3 * 0.3141592654e1 / w * Q * Q) * Q * Q) * Q + (-0.4e1 / 0.15e2 * 0.3141592654e1 * pow(w, 0.3e1) + (-0.2e1 / 0.5e1 * 0.3141592654e1 * w * w + 0.2e1 / 0.15e2 * 0.3141592654e1 * Q * Q) * Q + (0.2e1 / 0.7e1 * 0.3141592654e1 * w * w + (0.9e1 / 0.28e2 * 0.3141592654e1 * w - 0.3141592654e1 / w * Q * Q / 0.28e2) * Q + (-0.6e1 / 0.35e2 * 0.3141592654e1 * w - 0.9e1 / 0.70e2 * 0.3141592654e1 * Q + (0.3141592654e1 / 0.18e2 + 0.3141592654e1 / w * Q / 0.48e2 - 0.4e1 / 0.525e3 * 0.3141592654e1 / w * R) * R) * R) * R) * R) * R * R) * R * R) * R); // NOLINT line length
            else if (Q <= w)
                return(-0.2e1 / 0.1575e4 * 0.3141592654e1 * pow(w, 0.9e1) + (-0.3141592654e1 * pow(w, 0.8e1) / 0.210e3 + (0.2e1 / 0.105e3 * 0.3141592654e1 * pow(w, 0.6e1) + (-0.3e1 / 0.25e2 * 0.3141592654e1 * pow(w, 0.4e1) + (0.4e1 / 0.15e2 * 0.3141592654e1 * pow(w, 0.3e1) + (-0.2e1 / 0.7e1 * 0.3141592654e1 * w * w + (0.6e1 / 0.35e2 * 0.3141592654e1 * w + (-0.3141592654e1 / 0.18e2 + 0.4e1 / 0.525e3 * 0.3141592654e1 / w * Q) * Q) * Q) * Q) * Q) * Q * Q) * Q * Q) * Q + (0.3141592654e1 * pow(w, 0.8e1) / 0.210e3 + (0.9e1 / 0.560e3 * 0.3141592654e1 * pow(w, 0.7e1) + (-0.3141592654e1 * pow(w, 0.5e1) / 0.20e2 + (0.9e1 / 0.40e2 * 0.3141592654e1 * pow(w, 0.3e1) + (-0.2e1 / 0.5e1 * 0.3141592654e1 * w * w + (0.9e1 / 0.28e2 * 0.3141592654e1 * w + (-0.9e1 / 0.70e2 * 0.3141592654e1 + 0.3141592654e1 / w * Q / 0.48e2) * Q) * Q) * Q) * Q * Q) * Q * Q) * Q + (-0.2e1 / 0.105e3 * 0.3141592654e1 * pow(w, 0.6e1) + (-0.3141592654e1 * pow(w, 0.5e1) / 0.20e2 + (0.3141592654e1 * pow(w, 0.3e1) / 0.12e2 + (-0.3e1 / 0.20e2 * 0.3141592654e1 * w + (0.2e1 / 0.15e2 * 0.3141592654e1 - 0.3141592654e1 / w * Q / 0.28e2) * Q) * Q * Q) * Q * Q) * Q + (0.3e1 / 0.25e2 * 0.3141592654e1 * pow(w, 0.4e1) + (0.9e1 / 0.40e2 * 0.3141592654e1 * pow(w, 0.3e1) + (-0.3e1 / 0.20e2 * 0.3141592654e1 * w + 0.9e1 / 0.200e3 * 0.3141592654e1 / w * Q * Q) * Q * Q) * Q + (-0.4e1 / 0.15e2 * 0.3141592654e1 * pow(w, 0.3e1) + (-0.2e1 / 0.5e1 * 0.3141592654e1 * w * w + 0.2e1 / 0.15e2 * 0.3141592654e1 * Q * Q) * Q + (0.2e1 / 0.7e1 * 0.3141592654e1 * w * w + (0.9e1 / 0.28e2 * 0.3141592654e1 * w - 0.3141592654e1 / w * Q * Q / 0.28e2) * Q + (-0.6e1 / 0.35e2 * 0.3141592654e1 * w - 0.9e1 / 0.70e2 * 0.3141592654e1 * Q + (0.3141592654e1 / 0.18e2 + 0.3141592654e1 / w * Q / 0.48e2 - 0.4e1 / 0.525e3 * 0.3141592654e1 / w * R) * R) * R) * R) * R) * R * R) * R * R) * R); // NOLINT line length
            else
                return(0.0e0);
        else if (R < w && w <= Q)
            return((-0.3e1 / 0.25e2 * 0.3141592654e1 * pow(w, 0.4e1) + (0.9e1 / 0.40e2 * 0.3141592654e1 * pow(w, 0.3e1) + (-0.3e1 / 0.20e2 * 0.3141592654e1 * w + 0.9e1 / 0.200e3 * 0.3141592654e1 / w * Q * Q) * Q * Q) * Q + (0.4e1 / 0.15e2 * 0.3141592654e1 * pow(w, 0.3e1) + (-0.2e1 / 0.5e1 * 0.3141592654e1 * w * w + 0.2e1 / 0.15e2 * 0.3141592654e1 * Q * Q) * Q + (-0.2e1 / 0.7e1 * 0.3141592654e1 * w * w + (0.9e1 / 0.28e2 * 0.3141592654e1 * w - 0.3141592654e1 / w * Q * Q / 0.28e2) * Q + (0.6e1 / 0.35e2 * 0.3141592654e1 * w - 0.9e1 / 0.70e2 * 0.3141592654e1 * Q + (-0.3141592654e1 / 0.18e2 + 0.3141592654e1 / w * Q / 0.48e2 + 0.4e1 / 0.525e3 * 0.3141592654e1 / w * R) * R) * R) * R) * R) * pow(R, 0.5e1));  // NOLINT line length
        else if (R < w && Q < w && w <= R + Q)
            return(0.2e1 / 0.1575e4 * 0.3141592654e1 * pow(w, 0.9e1) + (-0.3141592654e1 * pow(w, 0.8e1) / 0.210e3 + (0.2e1 / 0.105e3 * 0.3141592654e1 * pow(w, 0.6e1) + (-0.3e1 / 0.25e2 * 0.3141592654e1 * pow(w, 0.4e1) + (0.4e1 / 0.15e2 * 0.3141592654e1 * pow(w, 0.3e1) + (-0.2e1 / 0.7e1 * 0.3141592654e1 * w * w + (0.6e1 / 0.35e2 * 0.3141592654e1 * w + (-0.3141592654e1 / 0.18e2 + 0.4e1 / 0.525e3 * 0.3141592654e1 / w * Q) * Q) * Q) * Q) * Q) * Q * Q) * Q * Q) * Q + (-0.3141592654e1 * pow(w, 0.8e1) / 0.210e3 + (0.9e1 / 0.560e3 * 0.3141592654e1 * pow(w, 0.7e1) + (-0.3141592654e1 * pow(w, 0.5e1) / 0.20e2 + (0.9e1 / 0.40e2 * 0.3141592654e1 * pow(w, 0.3e1) + (-0.2e1 / 0.5e1 * 0.3141592654e1 * w * w + (0.9e1 / 0.28e2 * 0.3141592654e1 * w + (-0.9e1 / 0.70e2 * 0.3141592654e1 + 0.3141592654e1 / w * Q / 0.48e2) * Q) * Q) * Q) * Q * Q) * Q * Q) * Q + (0.2e1 / 0.105e3 * 0.3141592654e1 * pow(w, 0.6e1) + (-0.3141592654e1 * pow(w, 0.5e1) / 0.20e2 + (0.3141592654e1 * pow(w, 0.3e1) / 0.12e2 + (-0.3e1 / 0.20e2 * 0.3141592654e1 * w + (0.2e1 / 0.15e2 * 0.3141592654e1 - 0.3141592654e1 / w * Q / 0.28e2) * Q) * Q * Q) * Q * Q) * Q + (-0.3e1 / 0.25e2 * 0.3141592654e1 * pow(w, 0.4e1) + (0.9e1 / 0.40e2 * 0.3141592654e1 * pow(w, 0.3e1) + (-0.3e1 / 0.20e2 * 0.3141592654e1 * w + 0.9e1 / 0.200e3 * 0.3141592654e1 / w * Q * Q) * Q * Q) * Q + (0.4e1 / 0.15e2 * 0.3141592654e1 * pow(w, 0.3e1) + (-0.2e1 / 0.5e1 * 0.3141592654e1 * w * w + 0.2e1 / 0.15e2 * 0.3141592654e1 * Q * Q) * Q + (-0.2e1 / 0.7e1 * 0.3141592654e1 * w * w + (0.9e1 / 0.28e2 * 0.3141592654e1 * w - 0.3141592654e1 / w * Q * Q / 0.28e2) * Q + (0.6e1 / 0.35e2 * 0.3141592654e1 * w - 0.9e1 / 0.70e2 * 0.3141592654e1 * Q + (-0.3141592654e1 / 0.18e2 + 0.3141592654e1 / w * Q / 0.48e2 + 0.4e1 / 0.525e3 * 0.3141592654e1 / w * R) * R) * R) * R) * R) * R * R) * R * R) * R); // NOLINT line length
        else
            return(0.0e0);
    }
}

double gauss::innerProduct( const WGaussProcess& p1, const WGaussProcess& p2 )
{
    WFiber f1 = p1.generateTract();
    WFiber f2 = p2.generateTract();
    double Q = p1.getRadius();
    double R = p2.getRadius();

    Eigen::MatrixXd integralMatrix( static_cast< int >( f1.size() ), static_cast< int >( f2.size() ) );
    size_t i = 0, j = 0;
    for( WFiber::const_iterator cit = f1.begin(); cit != f1.end(); ++cit, ++i )
    {
        for( WFiber::const_iterator cit2 = f2.begin(); cit2 != f2.end(); ++cit2, ++j )
        {
            integralMatrix( i, j ) = covIntegralThinPlateR3Normalized( length( *cit - *cit2 ), Q, R );
        }
        j = 0; // reset every loop!
    }
    const Eigen::VectorXd left = p1.getCff1lProduct();
    const Eigen::VectorXd right = p2.getCff1lProduct();

    return left.dot( integralMatrix * right );
}

