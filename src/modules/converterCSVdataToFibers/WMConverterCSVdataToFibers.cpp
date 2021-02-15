//
// Created by top2021 on 15.02.21.
//

#include "WMConverterCSVdataToFibers.h"

W_LOADABLE_MODULE( WMConverterCSVdataToFibers )

WMConverterCSVdataToFibers::WMConverterCSVdataToFibers():
        WModule()
{
}

WMConverterCSVdataToFibers::~WMConverterCSVdataToFibers()
{
}

const std::string WMConverterCSVdataToFibers::getName() const
{
    return "hung";
}

const std::string WMConverterCSVdataToFibers::getDescription() const
{
    return "Converter CSV-data To WDataSetFibers";
}

boost::shared_ptr< WModule > WMConverterCSVdataToFibers::factory() const
{
    return boost::shared_ptr< WModule >( new WMConverterCSVdataToFibers() );
}

const char** WMConverterCSVdataToFibers::getXPMIcon() const
{
    return NULL;
}

void WMConverterCSVdataToFibers::moduleMain()
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

void WMConverterCSVdataToFibers::connectors()
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

void WMConverterCSVdataToFibers::properties()
{

}

int WMConverterCSVdataToFibers::getCol(std::string col, std::vector<std::string> in_row)
{
    int pos = 0;
    for (std::vector<std::string>::iterator it = in_row.begin(); it != in_row.end(); it++)
    {
        if (*it == col) return pos;
        pos++;
    }
    return pos;
}

void WMConverterCSVdataToFibers::FilterPoints(boost::shared_ptr< std::vector < std::vector < std::string > > > dataCSV)
{
    m_vertices = boost::shared_ptr< std::vector< float > >(new std::vector<float>());
    m_lineStartIndexes = boost::shared_ptr< std::vector < size_t > >(new std::vector < size_t >());
    m_lineLengths  = boost::shared_ptr< std::vector < size_t > >(new std::vector < size_t >());
    m_verticesReverse = boost::shared_ptr< std::vector < size_t > >(new std::vector < size_t >());

    std::vector <int> event_id;
    int is_header = true;

    int xpos_arr_col = 0;
    int ypos_arr_col = 0;
    int zpos_arr_col = 0;
    int idpos_arr_col = 0;

    for (std::vector< std::vector<std::string>>::iterator it = dataCSV->begin(); it != dataCSV->end(); it++)
    {
        int posX,posY,posZ,id;
        int count = 0;
        int finish_flag = 0;

        if (is_header)
        {
            xpos_arr_col = getCol("posX", *it);
            ypos_arr_col = getCol("posY", *it);
            zpos_arr_col = getCol("posZ", *it);
            idpos_arr_col = getCol("eventID", *it);

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
            if(count == idpos_arr_col)
            {
                id = std::stoi(*it_inner);
                finish_flag++;
            }
            if (finish_flag == 4) break;
            count++;
        }

        m_vertices->push_back(posX);
        m_vertices->push_back(posY);
        m_vertices->push_back(posZ);
        event_id.push_back(id);
    }

    int length = 0;
    int indexPos = 0;
    int reversePos = 0;
    int eventid = event_id[0];

    bool lastelement;
    m_lineStartIndexes->push_back(indexPos);

    for(std::vector<int>::iterator it = event_id.begin(); it != event_id.end(); it++)
    {
        if(eventid != *it)
        {
            eventid = *it;
            m_lineStartIndexes->push_back(indexPos);
            m_lineLengths->push_back(length);
            length=0;
            reversePos++;
            lastelement = true;
        }
        length++;
        indexPos++;
        m_verticesReverse->push_back(reversePos);
        lastelement = false;
    }
    if(!lastelement)
    {
        m_lineLengths->push_back(length);
    }

    m_points = boost::shared_ptr< WDataSetFibers>(
            new WDataSetFibers(
                    m_vertices,
                    m_lineStartIndexes,
                    m_lineLengths,
                    m_verticesReverse
            )
    );

    m_output->updateData(m_points);
}

