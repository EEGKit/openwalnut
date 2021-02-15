//
// Created by top2021 on 12.02.21.
//

#include "WMConverterCSVdataToPoints.h"

W_LOADABLE_MODULE(WMConverterCSVdataToPoints )

WMConverterCSVdataToPoints::WMConverterCSVdataToPoints():
        WModule()
{
}

WMConverterCSVdataToPoints::~WMConverterCSVdataToPoints()
{
}

const std::string WMConverterCSVdataToPoints::getName() const
{
    return "ConverterCSVdataToPoints";
}

const std::string WMConverterCSVdataToPoints::getDescription() const
{
    return "Converter CSV-data To WDataSetPoints";
}

boost::shared_ptr< WModule > WMConverterCSVdataToPoints::factory() const
{
    return boost::shared_ptr< WModule >( new WMConverterCSVdataToPoints() );
}

const char** WMConverterCSVdataToPoints::getXPMIcon() const
{
    return NULL;
}

void WMConverterCSVdataToPoints::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add(m_input->getDataChangedCondition());

    ready();

    waitRestored();

    createVertexSet();

    while( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        boost::shared_ptr< WDataSetCSV > dataset = m_input->getData();
        m_content = dataset->m_content;



    }
}

void WMConverterCSVdataToPoints::createVertexSet()
{
    //ToDo: Input instead of linear-Data-Set
    m_vertices = boost::shared_ptr< std::vector< float > >(new std::vector<float>());
    m_colors = boost::shared_ptr< std::vector< float > >(new std::vector<float>());

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
}

void WMConverterCSVdataToPoints::connectors()
{
    m_input = WModuleInputData< WDataSetCSV >::createAndAdd(shared_from_this(), "in", "TEST_input");

    m_output = boost::shared_ptr< WModuleOutputData< WDataSet > >(
            new WModuleOutputData<WDataSet>(
                    shared_from_this(),
                    "out",
                    "TEST_output")
    );


    addConnector( m_output );

    WModule::connectors();
}

void WMConverterCSVdataToPoints::properties()
{

}

