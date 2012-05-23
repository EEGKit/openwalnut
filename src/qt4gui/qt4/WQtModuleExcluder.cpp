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

#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <QtGui/QVBoxLayout>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QCheckBox>
#include <QtGui/QPushButton>
#include <QtGui/QDialogButtonBox>

#include "core/kernel/WModuleFactory.h"
#include "core/common/WPathHelper.h"

#include "WQt4Gui.h"
#include "WMainWindow.h"

#include "WQtModuleExcluder.h"
#include "WQtModuleExcluder.moc"

/**
 * Simple modified checkbox which is two-state by using an additional flag but tristate under the hood. See nextCheckState.
 */
class WQtItemCheckbox: public QCheckBox
{
public:
    /**
     * Constructs checkbox with the specified flag.
     *
     * \param recommended if true, this checkbox cannot be unchecked.
     */
    explicit WQtItemCheckbox( bool recommended ):
        QCheckBox(),
        m_isRecommended( recommended )
    {
        setTristate( true );
        if( m_isRecommended )
        {
            setCheckState( Qt::PartiallyChecked );
        }
        else
        {
            setCheckState( Qt::Unchecked );
        }
    }

    /**
     * Destruction.
     */
    virtual ~WQtItemCheckbox()
    {
        // cleanup
    }

protected:
    /**
     * Called by several QCheckBox slots when the next state needs to be set. This modifies the standard behaviour in a certain way: If
     * recommended flag was set, this allows switching between Check and Partial Check. If the flag was false, it allows switching between check
     * and unchecked.
     */
    virtual void nextCheckState()
    {
        if( m_isRecommended )
        {
            if( checkState() == Qt::Checked )
            {
                setCheckState( Qt::PartiallyChecked );
            }
            else if( checkState() == Qt::PartiallyChecked )
            {
                setCheckState( Qt::Checked );
            }
        }
        else
        {
            if( checkState() == Qt::Checked )
            {
                setCheckState( Qt::Unchecked );
            }
            else if( checkState() == Qt::Unchecked )
            {
                setCheckState( Qt::Checked );
            }
        }
    }
private:
    /**
     * If true, the checkbox cannot be unchecked.
     */
    bool m_isRecommended;
};

WQtModuleExcluder::WQtModuleExcluder( QWidget* parent, Qt::WindowFlags f ):
    QDialog( parent, f )
{
    // configure the dialog
    setModal( true );

    // some minimum size
    resize( 600, 400 );

    // load recommended modules
    loadRecommends();

    // get a list of all the modules available
    WModuleFactory::PrototypeSharedContainerType::ReadTicket r = WModuleFactory::getModuleFactory()->getPrototypes();
    typedef WModuleFactory::PrototypeContainerConstIteratorType ProtoIter;
    for( ProtoIter iter = r->get().begin(); iter != r->get().end(); ++iter )
    {
        m_moduleList.push_back( *iter );
    }

    // initialize members
    QVBoxLayout* layout = new QVBoxLayout;

    QString helpText = "This dialog allows you to modify the list of modules used everywhere in OpenWalnut. The list contains all loaded modules."
                       " Select the modules you want "
                       "to use and disable those you won't use. This way, the module toolbar and the context menu stay clean. The "
                       "OpenWalnut-Team provides a list of recommended modules. This list is always active, unless you turn it off. Recommended "
                       "modules are visually marked by being partially checked.";
    QLabel* hint = new QLabel( helpText, this );
    hint->setWordWrap( true );
    layout->addWidget( hint );

    // always show all modules?
    m_showThemAll = new QCheckBox( "Always show all modules.", this );
    m_showThemAll->setToolTip(
        "Recommended option for developer. This ensures that all modules get shown all them time, regardless of the list below."
    );
    connect( m_showThemAll, SIGNAL( stateChanged( int ) ), this, SLOT( showThemAllUpdated() ) );
    layout->addWidget( m_showThemAll );

    m_ignoreRecommends = new QCheckBox( "Ignore official recommendation.", this );
    m_ignoreRecommends->setToolTip(
        "By default, OpenWalnut provides a list of recommended modules. This list overrides your custom selection. To disable this, activate this "
        "option."
    );
    layout->addWidget( m_ignoreRecommends );

    // create the module list
    m_list = new QListWidget( this );
    layout->addWidget( m_list );
    setLayout( layout );

    // for modules without icon, use this
    QIcon noIcon = WQt4Gui::getMainWindow()->getIconManager()->getIcon( "DefaultModuleIcon" );

    // fill the list item
    for( std::vector< WModule::ConstSPtr >::const_iterator iter = m_moduleList.begin(); iter != m_moduleList.end(); ++iter )
    {
        // Create a custom widget which contains the name and description
        QWidget* widget = new QWidget( m_list );
        {
            QGridLayout* layoutWidget = new QGridLayout();

            QSizePolicy sizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ); // <-- scale it down

            int column = 0;

            // create a checkbox for this module, use the recommends list
            bool isRecommended = ( std::find( m_recommendedModules.begin(), m_recommendedModules.end(),
                                              ( *iter )->getName() ) != m_recommendedModules.end() );
            WQtItemCheckbox* check = new WQtItemCheckbox( isRecommended );
            check->setSizePolicy( sizePolicy );
            layoutWidget->addWidget( check, 0, 0, 2, 1 );

            // we later need to find the checkbox for one module easily:
            m_moduleItemMap[ ( *iter )->getName() ] = check;

            ++column;

            // now, an icon
            QLabel* icon = new QLabel();
            icon->setSizePolicy( sizePolicy );

            // if there is an icon -> show it
            if( ( *iter )->getXPMIcon() )
            {
                // we need to enforce some size
                QPixmap qicon( ( *iter )->getXPMIcon() );
                qicon = qicon.scaled( 32, 32, Qt::KeepAspectRatio );
                icon->setPixmap( qicon );
            }
            else
            {
                icon->setPixmap( noIcon.pixmap( 32, 32 ) );
            }

            layoutWidget->addWidget( icon, 0, column, 2, 1 );
            ++column;

            // Add Name and Description
            layoutWidget->addWidget( new QLabel( "<b>" + QString::fromStdString( ( *iter )->getName() )+ "</b>" ), 0, column );
            // if there is no description -> no widget added to save space
            if( !( *iter )->getDescription().empty() )
            {
                layoutWidget->addWidget(  new QLabel( QString::fromStdString( ( *iter )->getDescription() ) ), 1, column );
            }

            layoutWidget->setSizeConstraint( QLayout::SetMaximumSize );
            widget->setLayout( layoutWidget );
        }

        QListWidgetItem* item = new QListWidgetItem( m_list );
        item->setSizeHint( widget->sizeHint() );

        m_list->addItem( item );
        m_list->setItemWidget( item, widget );
        m_list->setMinimumHeight( 150 );
    }

    // provide some default buttons here
    QDialogButtonBox* defButtons = new QDialogButtonBox( QDialogButtonBox::Save | QDialogButtonBox::Cancel | QDialogButtonBox::RestoreDefaults,
                                                         Qt::Horizontal, this );
    connect( defButtons, SIGNAL( accepted() ), this, SLOT( accept() ) );
    connect( defButtons, SIGNAL( rejected() ), this, SLOT( reject() ) );
    connect( defButtons->button( QDialogButtonBox::RestoreDefaults ), SIGNAL( clicked() ), this, SLOT( reset() ) );
    layout->addWidget( defButtons );

    // initialize the widgets
    loadListsFromSettings();
}

WQtModuleExcluder::~WQtModuleExcluder()
{
    // cleanup
}

void WQtModuleExcluder::loadListsFromSettings( bool recommendsOnly )
{
    // update checkbox too
    bool ignoreAllowedList = WQt4Gui::getSettings().value( "qt4gui/modules/IgnoreAllowedList", false ).toBool();
    m_showThemAll->setCheckState( ignoreAllowedList ? Qt::Checked : Qt::Unchecked );
    bool ignoreRecommendsList = WQt4Gui::getSettings().value( "qt4gui/modules/IgnoreRecommendedList", false ).toBool();
    m_ignoreRecommends->setCheckState( ignoreRecommendsList ? Qt::Checked : Qt::Unchecked );

    // read settings
    std::string allowedModules = WQt4Gui::getSettings().value( "qt4gui/modules/allowedList", "" ).toString().toStdString();
    m_allowedModules = string_utils::tokenize( allowedModules, "," );

    // also set the recommended modules explicitly
    for( AllowedModuleList::const_iterator iter = m_recommendedModules.begin(); iter != m_recommendedModules.end(); ++iter )
    {
        if( m_moduleItemMap.count( *iter ) )
        {
            m_moduleItemMap[ *iter ]->setCheckState( Qt::PartiallyChecked );
        }
    }

    if( !recommendsOnly )
    {
        // set dialog according to the settings
        for( AllowedModuleList::const_iterator iter = m_allowedModules.begin(); iter != m_allowedModules.end(); ++iter )
        {
            if( m_moduleItemMap.count( *iter ) )
            {
                m_moduleItemMap[ *iter ]->setCheckState( Qt::Checked );
            }
        }
    }

    // only if wanted:
    if( !m_ignoreRecommends )
    {
        std::copy( m_recommendedModules.begin(), m_recommendedModules.end(), m_allowedModules.begin() );
    }
}

void WQtModuleExcluder::saveListToSettings()
{
    // rebuild list of allowed modules
    m_allowedModules.clear();
    std::string allowedAsString;    // this is needed since QList cannot be stored in a QSettings in a read-able format right now.

    // iterate the widgets and store in settings:
    for( ModuleItemMapType::const_iterator iter = m_moduleItemMap.begin(); iter != m_moduleItemMap.end(); ++iter )
    {
        // if it is checked, use.
        if( iter->second->checkState() == Qt::Checked ) // save only checked items, since partially checked ones are recommended modules.
        {
            m_allowedModules.push_back( iter->first );
            allowedAsString += iter->first + ",";
        }
    }

    // store this list in settings
    WQt4Gui::getSettings().setValue( "qt4gui/modules/allowedList", QString::fromStdString( allowedAsString ) );
    WQt4Gui::getSettings().setValue( "qt4gui/modules/IgnoreAllowedList", ( m_showThemAll->checkState() == Qt::Checked ) );
    WQt4Gui::getSettings().setValue( "qt4gui/modules/IgnoreRecommendedList", ( m_ignoreRecommends->checkState() == Qt::Checked ) );
}

void WQtModuleExcluder::enforceAllModules()
{
    WQt4Gui::getSettings().setValue( "qt4gui/modules/IgnoreAllowedList", true  );
}

void WQtModuleExcluder::loadRecommends()
{
    m_recommendedModules.clear();

    // where to find the config?
    boost::filesystem::path confFile = WPathHelper::getConfigPath() / "qt4gui" / "RecommendedModules.conf";
    // the preference we search
    std::string prefName = "modules.recommended";

    // before loading the settings, load the recommended modules list
    namespace po = boost::program_options; // since the namespace is far to big we use a shortcut here

    po::options_description configurationDescription( "OpenWalnut Recommended Modules" );

    configurationDescription.add_options()( prefName.c_str(), po::value< std::string >() );

    boost::program_options::variables_map configuration;
    if( boost::filesystem::exists( confFile ) )
    {
        try
        {
            // since overloaded function "const char*" dont work in boost 1.42.0
            std::ifstream ifs( confFile.string().c_str(), std::ifstream::in );
            po::store( po::parse_config_file( ifs, configurationDescription, true ), configuration );

            po::notify( configuration );
            if( configuration.count( prefName ) )
            {
                std::string recommended = configuration[ prefName ].as< std::string >();

                // tokenize the list:
                m_recommendedModules = string_utils::tokenize( recommended, "," );
            }
            else
            {
                wlog::error( "WQtModuleExcluder" ) << "No recommended modules specified in \"" << confFile.string() <<
                                                      "\". Enabling all modules as fall-back.";
                enforceAllModules();
            }
        }
        catch( const po::error &e )
        {
            wlog::error( "WQtModuleExcluder" ) << "Invalid configuration file \"" << confFile.string() <<
                                                   "\". Enabling all modules as fall-back. Error was: " << e.what();
            enforceAllModules();
        }
    }
    else
    {
        wlog::error( "WQtModuleExcluder" ) << "No \"" << confFile.string() << "\" found. Enabling all modules as fall-back.";
        enforceAllModules();
    }
}

bool WQtModuleExcluder::operator()( std::string const& name ) const
{
    return ( m_showThemAll->checkState() != Qt::Checked ) &&
           ( std::find( m_allowedModules.begin(), m_allowedModules.end(), name ) == m_allowedModules.end() );
}

bool WQtModuleExcluder::operator()( WModule::ConstSPtr module ) const
{
    return operator()( module->getName() );
}

void WQtModuleExcluder::configure()
{
    show();
}

QAction* WQtModuleExcluder::getConfigureAction() const
{
    QAction* a = new QAction( "Configure Allowed Modules", parent() );
    a->setToolTip( "Allows you to configure the list of modules, which is used for selecting modules in OpenWalnut (i.e. in the toolbar)." );
    connect( a, SIGNAL( triggered( bool ) ), this, SLOT( configure() ) );
    return a;
}

void WQtModuleExcluder::accept()
{
    saveListToSettings();
    emit updated();
    QDialog::accept();
}

void WQtModuleExcluder::reject()
{
    // reset everything to the current state in the settings:
    loadListsFromSettings();

    QDialog::reject();
}

void WQtModuleExcluder::showThemAllUpdated()
{
    if( m_showThemAll->checkState() == Qt::Checked )
    {
        m_list->setDisabled( true );
    }
    else
    {
        m_list->setDisabled( false );
    }
}

void WQtModuleExcluder::reset()
{
    // reset all checkboxes
    for( std::vector< WModule::ConstSPtr >::const_iterator iter = m_moduleList.begin(); iter != m_moduleList.end(); ++iter )
    {
        // we later need to find the checkbox for one module easily:
        m_moduleItemMap[ ( *iter )->getName() ]->setCheckState( Qt::Unchecked );
    }
    loadListsFromSettings( true );

    m_showThemAll->setCheckState( Qt::Unchecked );
    m_ignoreRecommends->setCheckState( Qt::Unchecked );
    m_list->setDisabled( false );
}