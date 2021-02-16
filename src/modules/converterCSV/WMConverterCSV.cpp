//
// Created by top2021 on 15.02.21.
//

#include "WMConverterCSV.h"

W_LOADABLE_MODULE(WMConverterCSV )

WMConverterCSV::WMConverterCSV():
        WModule()
{
}

WMConverterCSV::~WMConverterCSV()
{
}

const std::string WMConverterCSV::getName() const
{
    return "FilterProtonData";
}

const std::string WMConverterCSV::getDescription() const
{
    return "Converter CSV-data To WDataSetPoints and WDataSetFibers";
}

boost::shared_ptr< WModule > WMConverterCSV::factory() const
{
    return boost::shared_ptr< WModule >( new WMConverterCSV() );
}

const char** WMConverterCSV::getXPMIcon() const
{
    return NULL;
}

void WMConverterCSV::moduleMain()
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
        FilterFibers(m_content);
    }
}

void WMConverterCSV::connectors()
{
    m_input = WModuleInputData< WDataSetCSV >::createAndAdd(shared_from_this(), "in", "TEST_input");

    m_output_points = boost::shared_ptr< WModuleOutputData< WDataSet > >(
            new WModuleOutputData<WDataSet>(
                    shared_from_this(),
                    "output points",
                    "Output proton data as Point data")

    );

    m_output_fibers = boost::shared_ptr< WModuleOutputData< WDataSet > >(
            new WModuleOutputData<WDataSet>(
                    shared_from_this(),
                    "output fibers",
                    "Output proton data as Fiber data")

    );

    //std::vector< boost::shared_ptr< WModuleOutputConnector > >
   addConnector(m_output_points);
   addConnector(m_output_fibers);






    WModule::connectors();
}

void WMConverterCSV::properties()
{

}

int WMConverterCSV::getColumnNumberByName(std::string col, std::vector<std::string> in_row)
{
    int pos = 0;
    for (std::vector<std::string>::iterator it = in_row.begin(); it != in_row.end(); it++)
    {
        if (*it == col) return pos;
        pos++;
    }
    return pos;
}

void WMConverterCSV::FilterFibers(boost::shared_ptr< std::vector < std::vector < std::string > > > dataCSV)
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
            xpos_arr_col = getColumnNumberByName("posX", *it);
            ypos_arr_col = getColumnNumberByName("posY", *it);
            zpos_arr_col = getColumnNumberByName("posZ", *it);
            idpos_arr_col = getColumnNumberByName("eventID", *it);

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

    m_fibers = boost::shared_ptr< WDataSetFibers>(
            new WDataSetFibers(
                    m_vertices,
                    m_lineStartIndexes,
                    m_lineLengths,
                    m_verticesReverse
            )
    );

    m_output_fibers->updateData(m_fibers);
}

void WMConverterCSV::FilterPoints(boost::shared_ptr< std::vector < std::vector < std::string > > > dataCSV)
{
    m_vertices = boost::shared_ptr< std::vector< float > >(new std::vector<float>());
    m_colors = boost::shared_ptr< std::vector< float > >(new std::vector<float>());

    int is_header = true;

    int xpos_arr_col = 0;
    int ypos_arr_col = 0;
    int zpos_arr_col = 0;

    for (std::vector< std::vector<std::string>>::iterator it = dataCSV->begin(); it != dataCSV->end(); it++)
    {
        int posX, posY, posZ;
        int count = 0;
        int finish_flag = 0;

        if (is_header)
        {
            xpos_arr_col = getColumnNumberByName("posX", *it);
            ypos_arr_col = getColumnNumberByName("posY", *it);
            zpos_arr_col = getColumnNumberByName("posZ", *it);

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

    m_output_points->updateData(m_points);
}
