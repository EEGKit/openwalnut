//
// Created by top2021 on 12.02.21.
//

#include "WMconverterCSVdataToPoints.h"

W_LOADABLE_MODULE( WMconverterCSVdataToPoints )

WMconverterCSVdataToPoints::WMconverterCSVdataToPoints():
        WModule()
{
}

WMconverterCSVdataToPoints::~WMconverterCSVdataToPoints()
{
}

const std::string WMconverterCSVdataToPoints::getName() const
{
    return "ConverterCSVdataToPoints";
}

const std::string WMconverterCSVdataToPoints::getDescription() const
{
    return "Converter CSV-data To WDataSetPoints";
}

boost::shared_ptr< WModule > WMconverterCSVdataToPoints::factory() const
{
    return boost::shared_ptr< WModule >( new WMconverterCSVdataToPoints() );
}

const char** WMconverterCSVdataToPoints::getXPMIcon() const
{
    return NULL;
}

void WMconverterCSVdataToPoints::moduleMain()
{
    m_moduleState.setResetable( true, true );

    ready();

    waitRestored();

    while( !m_shutdownFlag() )
    {
        m_moduleState.wait();
    }
}

void WMconverterCSVdataToPoints::connectors()
{
    /*m_output = boost::shared_ptr< WModuleOutputData< WDataSet > >(
            new WModuleOutputData<WDataSet>(
                    shared_from_this(),
                    "out",
                    "TEST")
    );

    for (int i = 0; i < 3000; ++i)
    {
        m_vertices->push_back(i);
        m_vertices->push_back(i);
        m_vertices->push_back(i);

        m_colors->push_back(0);
        m_colors->push_back(0);
        m_colors->push_back(0);
    }

    m_points = boost::shared_ptr< WDataSetPoints >(
            new WDataSetPoints(
                        m_vertices,
                        m_colors
                    )
            );

    m_output->updateData(m_points);

    addConnector( m_output );*/

    WModule::connectors();
}

void WMconverterCSVdataToPoints::properties()
{

}

