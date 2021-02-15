//
// Created by top2021 on 12.02.21.
//

#include "WMConverterCSVdataToPoints.h"

//W_LOADABLE_MODULE(WMConverterCSVdataToPoints )

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


    while( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        boost::shared_ptr< WDataSetCSV > dataset = m_input->getData();
        m_content = dataset->m_content;

        FilterPoints(m_content);
    }
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

int WMConverterCSVdataToPoints::getCol(std::string col, std::vector<std::string> in_row)
{
    int pos = 0;
    for (std::vector<std::string>::iterator it = in_row.begin(); it != in_row.end(); it++)
    {
        if (*it == col) return pos;
        pos++;
    }
    return pos;
}

void WMConverterCSVdataToPoints::FilterPoints(boost::shared_ptr< std::vector < std::vector < std::string > > > dataCSV)
{
    m_vertices = boost::shared_ptr< std::vector< float > >(new std::vector<float>());
    m_colors = boost::shared_ptr< std::vector< float > >(new std::vector<float>());

    int is_header = true;

    int xpos_arr_col = 0;
    int ypos_arr_col = 0;
    int zpos_arr_col = 0;

    for (std::vector< std::vector<std::string>>::iterator it = dataCSV->begin(); it != dataCSV->end(); it++)
    {
        int posX;
        int posY;
        int posZ;
        int count = 0;
        int finish_flag = 0;

        if (is_header)
        {
            xpos_arr_col = getCol("posX", *it);
            ypos_arr_col = getCol("posY", *it);
            zpos_arr_col = getCol("posZ", *it);

            is_header = false;
            continue;
        }

        if (it->empty())
        {
            continue;
        }

        for (std::vector<std::string>::iterator it_inner = it->begin(); it_inner != it->end(); it_inner++)
        {

            if (count == xpos_arr_col)
            {
                posX = std::stof(*it_inner);
                finish_flag++;
            }
            if (count == ypos_arr_col)
            {
                posY = std::stof(*it_inner);
                finish_flag++;
            }
            if (count == zpos_arr_col)
            {
                posZ = std::stof(*it_inner);
                finish_flag++;
            }
            if (finish_flag == 3) break;
            count++;
        }

        m_vertices->push_back(posX);
        m_vertices->push_back(posY);
        m_vertices->push_back(posZ);

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

