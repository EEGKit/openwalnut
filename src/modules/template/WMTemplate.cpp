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




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// How to create your own module in OpenWalnut? Here are the steps to take:
//   * copy the template module directory
//   * think about a name for your module
//   * rename the files from WMTemplate.cpp and WMTemplate.h to WMYourModuleName.cpp and WMYourModuleName.h
//   * rename the class inside these files to WMYourModuleName
//   * change WMYourModuleName::getName() to a unique name, like "Your Module Name"
//   * add a new prototype of your module to src/kernel/WModuleFactory.cpp -> search for m_prototypes.insert
//     * analogously to the other modules, add yours
//     * Note: this step will be automated in some time
//   * run CMake and compile
//   * read the documentation in this module and modify it to your needs
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




// Some rules to the inclusion of headers:
//  * Ordering:
//    * C Headers
//    * C++ Standard headers
//    * External Lib headers (like OSG or Boost headers)
//    * headers of other classes inside OpenWalnut
//    * your own header file

#include <string>

#include <osg/ShapeDrawable>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Material>
#include <osg/StateAttribute>

#include "../../kernel/WKernel.h"
#include "../../common/WColor.h"
#include "../../common/WPropertyHelper.h"

#include "template.xpm"
#include "WMTemplate.h"

WMTemplate::WMTemplate():
    WModule()
{
    // In the constructor, you can initialize your members and all this stuff. You must not initialize connectors or properties here! You also
    // should avoid doing computationally expensive stuff, since every module has its own thread which is intended to be used for such calculations.
    // Please keep in mind, that every member initialized here is also initialized in the prototype, which may be a problem if the member is large,
    // and therefore, wasting a lot of memory in your module's prototype instance.
}

WMTemplate::~WMTemplate()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMTemplate::factory() const
{
    // To properly understand what this is, we need to have a look at how module instances get created. At first, if you are not familiar with the
    // design patterns "Prototype", "Abstract Factory" and "Factory Method" you should probably read about them first. For short: while the kernel
    // is starting up, it also creates an instance of WModuleFactory, which creates a prototype instance of every module that can be loaded.
    // These prototypes are then used to create new instances of modules, check compatibility of modules and identify the type of modules.
    // If someone, in most cases the module container, wants a new instance of a module with a given prototype, it asks the factory class for it,
    // which uses the prototype's factory() method. Since the method is virtual, it returns a module instance, created with the correct type.
    // A prototype itself is an instance of your module, with the constructor run, as well as connectors() and properties(). What does this mean
    // to your module? Unlike the real "Prototype"- Design pattern, the module prototypes do not get cloned to retrieve a new instance,
    // they get constructed using "new" and this factory method.
    //
    // Here is a short overview of the lifetime of a module instance:
    //
    //    * constructor
    //    * connectors()
    //    * properties()
    //    * now isInitialized() will return true
    //    * the module will be associated with a container
    //    * now isAssociated() will return true
    //          o isUsable() will return true
    //    * after it got added, moduleMain() will be called
    //    * run, run, run, run
    //    * notifyStop gets called
    //    * moduleMain() should end
    //    * destructor
    //
    // So you always have to write this method and always return a valid pointer to an object of your module class.
    // Never initialize something else in here!
    return boost::shared_ptr< WModule >( new WMTemplate() );
}

const char** WMTemplate::getXPMIcon() const
{
    // The template_xpm char array comes from the template.xpm file as included above.
    // Such char arrays, i.e. files, can be easily created using an image manipulation program
    // like GIMP. Be aware that the xpm file is a simple header file. Thus it contains real
    // code. This code can be manipulated by hand. Unfortunately, you really have to fix the
    // xpm files produced by gimp. You need to make the char array const in order to prevent
    // compiler warnings or even errors.
    return template_xpm;
}

const std::string WMTemplate::getName() const
{
    // Specify your module name here. This name must be UNIQUE!
    return "Template";
}

const std::string WMTemplate::getDescription() const
{
    // Specify your module description here. Be detailed. This text is read by the user.
    return "This module is intended to be a module template and an example for writing modules.";
}

void WMTemplate::connectors()
{
    // How will your module know on which data it should work? Through its input connector(s). How will other modules get to know about your
    // calculated output data? Through your output connector(s). Simple isn't it? You may assume your module as some kind of function, as in
    // common programming languages, where your connectors denote its function signature. The method "connectors()" is for initializing your
    // connectors, your function signature. Now, a short excursion on how the module container and kernel knows which connector can be connected
    // to which. Generally, there are only two types of connectors available for your usage: WModuleInputData and WModuleOutputData and they can
    // only be connected to each other. So, it is not possible to connect an input with an input, nor an output with an output. Both of them are
    // template classes and therefore are associated with a type. This type determines if an input connector is compatible with an output connector.
    // A simple example: assume you have a class hierarchy:
    // Initialize your connectors here. Give them proper names and use the type your module will create or rely on. Do not use types unnecessarily
    // high in class hierarchy. The list of your connectors is fixed after connectors() got called. As in common imperative programming languages
    // the function signature can not be changed during runtime (which, in our case, means after connectors() got called).

    // Here is an example of how to create connectors. This module wants to have an input connector. This connector is defined by the type of
    // data that should be transferred, an module-wide unique name and a proper description:
    m_input = boost::shared_ptr< WModuleInputData < WDataSetSingle  > >(
        new WModuleInputData< WDataSetSingle >( shared_from_this(),
                                                               "in", "The dataset to display" )
        );

    // This creates an input connector which can receive WDataSetSingle. It will never be able to connect to output connectors providing just a
    // WDataSet (which is the father class of WDataSetSingle), but it will be able to be connected to an output connector with a type derived
    // from WDataSetSingle.

    // As properties, every connector needs to be added to the list of connectors.
    addConnector( m_input );

    // Now, lets add an output connector. We want to provide data calculated here to other modules. The output connector is initialized the same
    // way as input connectors. You need the type, the module-wide unique name and the description. The type you specify here also determines
    // which input connectors can be connected to this output connector: only connectors with a type equal or lower in class hierarchy.
    m_output = boost::shared_ptr< WModuleOutputData < WDataSetSingle  > >(
        new WModuleOutputData< WDataSetSingle >( shared_from_this(),
                                                               "out", "The calculated dataset" )
        );

    // As above: make it known.
    addConnector( m_output );

    // call WModules initialization
    WModule::connectors();
}

void WMTemplate::properties()
{
    // Every module can provide properties to the outside world. These properties can be changed by the user in the GUI or simply by other
    // modules using yours. Properties NEED to be created and added here. Doing this outside this function will lead to severe problems.
    //
    // Theoretically, you can specify properties of every type possible in C++. Therefore, see WPropertyVariable. But in most cases, the
    // predefined properties (WPropertyTypes.h) are enough, besides being the only properties shown and supported by the GUI.
    //
    // To create and add a new property, every module has a member m_properties. It is a set of properties this module provides to the outer
    // world. As with connectors, a property which not has been added to m_properties is not visible for others. Now, how to add a new property?

    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );
    m_aTrigger         = m_properties->addProperty( "Do It Now!",               "Trigger Button Text.", WPVBaseTypes::PV_TRIGGER_READY,
                                                    m_propCondition );
    m_enableFeature    = m_properties->addProperty( "Enable Feature",           "Description.", true );
    m_anInteger        = m_properties->addProperty( "Number of Shape Rows",     "Number of shape rows.", 10, m_propCondition );
    m_anIntegerClone   = m_properties->addProperty( "CLONE!Number of Shape Rows",
                                                    "A property which gets modified if \"Number of shape rows\" gets modified.", 10 );
    m_aDouble          = m_properties->addProperty( "Shape Radii",              "Shape radii.", 20.0, m_propCondition );
    m_aString          = m_properties->addProperty( "A String",                 "Something.", std::string( "hello" ), m_propCondition );
    m_aFile            = m_properties->addProperty( "A Filenname",              "Description.", WKernel::getAppPathObject(), m_propCondition );
    m_aColor           = m_properties->addProperty( "A Color",                  "Description.", WColor( 1.0, 0.0, 0.0, 1.0 ) );

    // These lines create some new properties and add them to the property list of this module. The specific type to create is determined by the
    // initial value specified in the third argument. The first argument is the name of the property, which needs to be unique among all
    // properties of this group and must not contain any slashes (/). The second argument is a description. A nice feature is the possibility
    // to specify an own condition, which gets fired when the property gets modified. This is especially useful to wake up the module's thread
    // on property changes. So, the property m_anInteger will wake the module thread on changes. m_enableFeature and m_aColor should not wake up
    // the module thread. They get read by the update callback of this modules OSG node, to update the color. m_aTrigger is a property which can
    // be used to trigger costly operations. The GUI shows them as buttons with the description as button text. The user can then press them and
    // the WPropTrigger will change its state to PV_TRIGGER_TRIGGERED. In the moduleMain documentation, you'll find a more detailed description
    // of how to use trigger properties. Be aware, that these kind of properties should be used carefully. They somehow inhibit the update flow
    // through the module graph.
    //
    // m_anIntegerClone has a special purpose in this example. It shows that you can simply update properties from within your module whilst the
    // GUI updates itself. You can, for example, set constraints or simply modify values depending on input data, most probably useful to set
    // nice default values or min/max constraints.

    // All these above properties are not that usable for selections. Assume the following situation. Your module allows two different kinds of
    // algorithms to run on some data and you want the user to select which one should do the work. This might be done integer properties but it
    // is simply ugly. Therefore, properties of type WPropSelection are available. First you need to define a list of alternatives:
    m_possibleSelections = boost::shared_ptr< WItemSelection >( new WItemSelection() );
    m_possibleSelections->addItem( "Select", "Description" );
    m_possibleSelections->addItem( "Me", "Description" );
    m_possibleSelections->addItem( "Please", "Description" );

    // This list of alternatives is NOT the actual property value. It is the list on which so called "WItemSelector" instances work. These
    // selectors are the actual property. After you created the first selector instance from the list, it can't be modified anymore. This ensures
    // that it is consistent among multiple threads and selection instances. The following two lines create two selectors as initial value and
    // create the property:
    m_aSingleSelection = m_properties->addProperty( "Select one",  "Description.", m_possibleSelections->getSelectorFirst(), m_propCondition );
    m_aMultiSelection  = m_properties->addProperty( "Select some", "Description.", m_possibleSelections->getSelectorAll(), m_propCondition );

    // Adding a lot of properties might confuse the user. Using WPropGroup, you have the possibility to group your properties together. A
    // WPropGroup needs a name and can provide a description. As with properties, the name should not contain any "/" and must be unique.

    m_group1        = m_properties->addPropertyGroup( "Group 1",  "A nice group for grouping stuff." );
    m_group1a       = m_group1->addPropertyGroup(     "Group 1a", "A group nested into \"Group 1\"." );
    m_group2        = m_properties->addPropertyGroup( "Group 2",  "Another nice group for grouping stuff." );

    // To understand how the groups can be used, you should consider that m_properties itself is a WPropGroup! This means, you can use your newly
    // created groups exactly in the same way as you would use m_properties.
    m_group1Bool    = m_group1->addProperty( "Funny stuff", "A grouped property", true );

    // You even can add one property multiple times to different groups:
    m_group2->addProperty( m_aColor );
    m_group1a->addProperty( m_aDouble );
    m_group1a->addProperty( m_enableFeature );

    // How can the values of the properties be changed? You can take a look at moduleMain where this is shown. For short: m_anInteger->set( 2 )
    // and m_anInteger->get().

    // The properties offer another nice feature: property constraints. You can enforce your properties to be in a special range, to not be
    // empty, to contain a valid directory name and so on. This is done with the class WPropertyVariable< T >::WPropertyConstraint. There are
    // several predefined you can use directly: WPropertyConstraintTypes.h. The constants defined there can be used as namespace in
    // WPropertyHelper. As an example, we want the property m_aFile to only contain existing directories:
    WPropertyHelper::PC_PATHEXISTS::addTo( m_aFile );
    WPropertyHelper::PC_ISDIRECTORY::addTo( m_aFile );

    // Thats it. To set minimum and maximum value for a property the convenience methods setMin and setMax are defined. setMin and setMax are
    // allowed for all property types with defined <= and >= operator.
    m_anInteger->setMin( 1 );
    m_anInteger->setMax( 15 );
    m_aDouble->setMin( 5.0 );
    m_aDouble->setMax( 50.0 );

    // we also want to constraint the both selection properties. One should not allow selecting multiple elements. But both require at least one
    // element to be selected:
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_aSingleSelection );
    WPropertyHelper::PC_NOTEMPTY::addTo( m_aSingleSelection );
    WPropertyHelper::PC_NOTEMPTY::addTo( m_aMultiSelection );

    // The most amazing feature is: custom constraints. Similar to OSG update callbacks, you just need to write your own PropertyConstraint class
    // to define the allowed values for your constraint. Take a look at the StringLength class in this module's code on how to do it.
    m_aString->addConstraint( boost::shared_ptr< StringLength >( new StringLength ) );
    WPropertyHelper::PC_NOTEMPTY::addTo( m_aString );

    // One last thing to mention is the active property. This property is available in all modules and represents the activation state of the
    // module. Int the GUI this is simply a checkbox beneath the module. The active property should be taken into account in ALL modules.
    // Visualization modules should turn off their graphics. There are basically three ways to react on changes in m_active, which is the member
    // variable for this property.
    // 1: overwrite WModule::activate() in your module
    // 2: register your own handler: m_active->getCondition()->subscribeSignal( boost::bind( &WMTemplate::myCustomActiveHandler, this ) );
    // 3: react during your module main loop using the moduleState: m_moduleState.add( m_active->getCondition );
    // Additionally, your can also use the m_active variable directly in your update callbacks to en-/disable some OSG nodes.
    // This template module uses method number 1. This might be the easiest and most commonly used way.

    // Sometimes it is desirable to provide some values, statistics, counts, times, ... to the user. This would be possible by using a property
    // and set the value to the value you want to show the user. Nice, but the user can change this value. PropertyConstraints can't help here,
    // as they would forbid writing any value to the property (regardless if the module or the user wants to set it). In other words, these
    // special properties serve another purpose. They are used for information output. Your module already provides another property list only
    // for these kind of properties. m_infoProperties can be used in the same way as m_properties. The only difference is that each property and
    // property group added here can't be modified from the outside world. Here is an example:
    m_aIntegerOutput = m_infoProperties->addProperty( "Run Count", "Number of run cycles the module made so far.", 0 );
    // Later on, we will use this property to provide the number of run cycles to the user.
    // In more detail, the purpose type of the property gets set to PV_PURPOSE_INFORMATION automatically by m_infoProperties. You can, of course,
    // add information properties to your custom groups or m_properties too. There, you need to set the purpose flag of the property manually:
    std::string message = std::string( "Hey you! Besides all these parameters, you also can print values, html formatted strings, colors and " ) +
                          std::string( "so on using properties! Isn't it <b>amazing</b>?" );
    m_aStringOutput = m_group1a->addProperty( "A Message", "A message to the user.", message );
    m_aStringOutput->setPurpose( PV_PURPOSE_INFORMATION );
    // This adds the property m_aStringOutput to your group and sets its purpose. The default purpose for all properties is always
    // "PV_PURPOSE_PARAMETER". It simply denotes the meaning of the property - its meant to be used as modifier for the module's behaviour; a
    // parameter.
}

void WMTemplate::moduleMain()
{
    // This is the modules working thread. Its the most important part of your module.
    // When you enter this method, all connectors and properties the module provides are fixed. They get initialized in connectors() and
    // properties(). You always can assume the kernel, the GUI, the graphics engine and the data handler to be initialized and ready. Please keep
    // in mind, that this method is running in its own thread.

    // You can output log messages everywhere and everytime in your module. The WModule base class therefore provides debugLog, infoLog, warnLog
    // and errorLog. You can use them very similar to the common std::cout streams.
    debugLog() << "Entering moduleMain()";

    // Your module can notify everybody that it is ready to be used. The member function ready() does this for you. The ready state is especially
    // useful whenever your module needs to do long operations to initialize. No other module can connect to your module before it signals its
    // ready state. You can assume the code before ready() to be some kind of initialization code.
    debugLog() << "Doing time consuming operations";
    sleep( 5 );

    // Your module can use an moduleState variable to wait for certain events. Most commonly, these events are new data on input connectors or
    // changed properties. You can decide which events the moduleState should handle. Therefore, use m_moduleState.add( ... ) to insert every
    // condition you want to wait on. As every input connector provides an changeCondition, we now add this condition to the moduleState:
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );
    // Remember the condition provided to some properties in properties()? The condition can now be used with this condition set.
    m_moduleState.add( m_propCondition );
    // One note about "setResetable": It might happen, that a condition fires and your thread does not currently waits on it. This would mean,
    // that your thread misses the event. The resetable flag for those condition sets can help here. Whenever a condition, managed by the
    // condition set, fires, the moduleState variable remembers it. So, the next call to m_moduleState.wait() will immediately return and reset
    // the "memory" of the moduleState. For more details, see: http://berkeley.informatik.uni-leipzig.de/trac/ow-public/wiki/HowtoWaitCorrectly

    // Signal ready state. Now your module can be connected by the container, which owns the module.
    ready();
    debugLog() << "Module is now ready.";

    // Normally, you will have a loop which runs as long as the module should not shutdown. In this loop you can react on changing data on input
    // connectors or on changed in your properties.
    debugLog() << "Entering main loop";
    while ( !m_shutdownFlag() )
    {
        // Now, the moduleState variable comes into play. The module can wait for the condition, which gets fired whenever the input receives data
        // or an property changes. The main loop now waits until something happens.
        debugLog() << "Waiting ...";
        m_moduleState.wait();

        // As you might remember, this property is an information property to provide the number of run cycles to the outside world. It won't be
        // modified but the module can modify it. This is useful to provide statistics, counts, times or even a "hello world" string to the user
        // as an information or status report. Please do not abuse these information properties as progress indicators. A short overview on how
        // to make progress indicators is provided some lines below. Here, we simply increase the value.
        m_aIntegerOutput->set( m_aIntegerOutput->get() +1 );

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // After waking up, the module has to check whether the shutdownFlag fired. If yes, simply quit the module.

        // woke up since the module is requested to finish
        if ( m_shutdownFlag() )
        {
            break;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // The next part is the collection part. We collect the information we need and check whether they changed.
        // Do not recalculate everything in every loop. Always check whether the data changed or some property and handle those cases properly.
        // After collection, the calculation work can be done.


        // Now, we can check the input, whether it changed the data. Therefore, we try to grab some data and check whether it is different from
        // the currently used data. Of course, you do not need to check whether the data really is different, but you should do it as you do not
        // want permanent recalculation which is actually not needed.
        //
        // Note: do not call m_input->getData() twice; one for checking if it is different and on for copying the pointer, since the result of
        // getData might be different among both calls.
        boost::shared_ptr< WDataSetSingle > newDataSet = m_input->getData();
        bool dataChanged = ( m_dataSet != newDataSet );
        bool dataValid   = ( newDataSet );

        // To check validity of multiple inputs at once, you can also use dataChanged and dataValid:
        // bool dataChanged = ( m_dataSet != newDataSet ) || ( m_dataSet2 != newDataSet2 ) || ( m_dataSet3 != newDataSet3 );
        // bool dataValid   = newDataSet && newDataSet2 && newDataSet3;
        // This way, you can easily ensure that ALL your inputs are set and the module can do its job

        // now, copy the new data to out local member variables
        if ( dataChanged && dataValid )
        // this condition will become true whenever the new data is different from the current one or our actual data is NULL. This handles all
        // cases.
        {
            // The data is different. Copy it to our internal data variable:
            debugLog() << "Received Data.";
            m_dataSet = newDataSet;

            // For multiple inputs:
            // m_dataSet2 = newDataSet2;
            // m_dataSet3 = newDataSet3;
        }

        // Here we collect our properties. You, as with input connectors, always check if a property really has changed. You most probably do not
        // want to check properties which are used exclusively inside the update callback of your OSG node. As the properties are thread-safe, the
        // update callback can check them and apply it correctly to your visualization.
        //
        // To check whether a property changed, WPropertyVariable provides a changed() method which is true whenever the property has changed.
        // Please note: creating the property with addProperty( ... ) will set changed to true.
        if ( m_aFile->changed() )
        {
            // To reset the changed flag, supply a "true" to the get method. This resets the changed-flag and next loop you can again check
            // whether it has been changed externally.

            // This is a simple example for doing an operation which is not depending on any other property.
            debugLog() << "Doing an operation on the file \"" << m_aFile->get( true ).file_string() << "\".";

            // NOTE: be careful if you want to use m_dataSet here, as it might be unset. Verify data validity using dataChanged && dataValid.
        }

        // m_aFile got handled above. Now, handle two properties which together are used as parameters for an operation.
        if ( m_aString->changed() )
        {
            // This is a simple example for doing an operation which is depends on all, but m_anFile,  properties.
            debugLog() << "Doing an operation basing on m_aString ... ";
            debugLog() << "m_aString: " << m_aString->get( true );

            // NOTE: be careful if you want to use m_dataSet here, as it might be unset. Verify data validity using dataChanged && dataValid.
        }

        // This example code now shows how to modify your OSG nodes basing on changes in your dataset or properties.
        // The if statement also checks for data validity as it uses the data! You should also always do that.
        if ( ( m_anInteger->changed() || m_aDouble->changed() || dataChanged  ) && dataValid )
        {
            debugLog() << "Creating new OSG node";

            // You should grab your values at the beginning of such calculation blocks, since the property might change at any time!
            int rows = m_anInteger->get( true );
            double radii = m_aDouble->get( true );

            // You can set other properties here. This example simply sets the value of m_anIntegerClone. The set command allows an additional
            // parameter. If it is true, the specified property condition does not fire if it is set. This is useful if your module main loop
            // waits for the condition of the property you want to set. Setting the property without suppressing the notification would cause
            // another loop in your module.
            m_anIntegerClone->set( m_anInteger->get(), true );

            debugLog() << "Number of Rows: " << rows;
            debugLog() << "Radii: " << radii;
            debugLog() << "Current dataset: " << m_dataSet->getFileName() << " with name: " << m_dataSet->getName();

            // This block will be executed whenever we have a new dataset or the m_anInteger property has changed. This example codes produces
            // some shapes and replaces the existing root node by a new (updated) one. Therefore, a new root node is needed:
            osg::ref_ptr< osg::Geode > newRootNode = new osg::Geode();
            // When working with the OpenSceneGraph, always use ref_ptr to store pointers to OSG objects. This allows OpenSceneGraph to manage
            // its resources automatically.
            for ( int32_t i = 0; i < rows; ++i )
            {
                newRootNode->addDrawable(
                        new osg::ShapeDrawable( new osg::Box(             osg::Vec3(  25, 128, i * 15 ), radii ) ) );
                newRootNode->addDrawable(
                        new osg::ShapeDrawable( new osg::Sphere(          osg::Vec3(  75, 128, i * 15 ), radii ) ) );
                newRootNode->addDrawable(
                        new osg::ShapeDrawable( new osg::Cone(            osg::Vec3( 125, 128, i * 15 ), radii, radii ) ) );
                newRootNode->addDrawable(
                        new osg::ShapeDrawable( new osg::Cylinder(        osg::Vec3( 175, 128, i * 15 ), radii, radii ) ) );
                newRootNode->addDrawable(
                        new osg::ShapeDrawable( new osg::Capsule(         osg::Vec3( 225, 128, i * 15 ), radii, radii ) ) );
            }

            // The old root node needs to be removed safely. The OpenSceneGraph traverses the graph at every frame. This traversion is done in a
            // separate thread. Therefore, adding a Node directly may cause the OpenSceneGraph to crash. Thats why the Group node provided by
            // getScene offers safe remove and insert methods. Use them to manipulate the scene node.
            WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootNode );
            m_rootNode = newRootNode;

            // please also ensure that, according to m_active, the node is active or not. Setting it here allows the user to deactivate modules
            // in project files for example.
            m_rootNode->setNodeMask( m_active->get() ? 0xFFFFFFFF : 0x0 );

            // OSG allows you to add custom callbacks. These callbacks get executed on each update traversal. They can be used to modify several
            // attributes and modes of existing nodes. You do not want to remove the node and recreate another one to simply change some color,
            // right? Setting the color can be done in such an update callback. See in the header file, how this class is defined.
            m_rootNode->addUpdateCallback( new SafeUpdateCallback( this ) );

            WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_rootNode );
        }

        // Now we updated the visualization after the dataset has changed. Your module might also calculate some other datasets basing on the
        // input data.
        // To ensure that all datasets are valid, check dataChanged and dataValid. If both are true, you can safely use the data.
        if ( dataChanged && dataValid )
        {
            debugLog() << "Data changed. Recalculating output.";

            // Calculate your new data here. This example just forwards the input to the output ;-).
            boost::shared_ptr< WDataSetSingle > newData = m_dataSet;

            // Doing a lot of work without notifying the user visually is not a good idea. So how is it possible to report progress? Therefore,
            // the WModule class provides a member m_progress which is of type WPropgressCombiner. You can create own progress objects and count
            // them individually. The m_progress combiner provides this information to the GUI and the user.
            // Here is a simple example:
            int steps = 10;
            boost::shared_ptr< WProgress > progress1 = boost::shared_ptr< WProgress >( new WProgress( "Doing work 1", steps ) );
            m_progress->addSubProgress( progress1 );
            for ( int i = 0; i < steps; ++i )
            {
                ++*progress1;
                sleep( 1 );
            }
            progress1->finish();
            // This creates a progress object with a name and a given number of steps. Your work loop can now increment the progress object. The
            // progress combiner m_progress collects the progress and provides it to the GUI. When finished, the progress MUST be marked as
            // finished using finish(). It is no problem to have several progress objects at the same time!

            // Sometimes, the number of steps is not known. WProgress can also handle this. Simply leave away the last parameter (the number of
            // steps. As with the other progress, you need to add it to the modules progress combiner and you need to mark it as finished with
            // finish() if you are done with your work.
            boost::shared_ptr< WProgress > progress2 = boost::shared_ptr< WProgress >( new WProgress( "Doing work 2" ) );
            m_progress->addSubProgress( progress2 );
            sleep( 2 );
            progress2->finish();

            // How to set the data to the output and how to notify other modules about it?
            m_output->updateData( newData );
            // This sets the new data to the output connector and automatically notifies all modules connected to your output.
        }

        // As we provided our condition to m_aTrigger too, the main thread will wake up if it changes. The GUI can change the trigger only to the
        // state "PV_TRIGGER_TRIGGERED" (this is the case if the user presses the button).
        if ( m_aTrigger->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
        {
            // Now that the trigger has the state "triggered", a time consuming operation can be done here.
            debugLog() << "User triggered an important and time consuming operation.";

            // Do something here. As above, do not forget to inform the user about your progress.
            int steps = 10;
            boost::shared_ptr< WProgress > progress1 = boost::shared_ptr< WProgress >( new WProgress( "Doing something important", steps ) );
            m_progress->addSubProgress( progress1 );
            for ( int i = 0; i < steps; ++i )
            {
                ++*progress1;
                sleep( 1 );
            }
            progress1->finish();

            // As long as the module does not reset the trigger to "ready", the GUI disables the trigger button. This is very useful to avoid
            // that a user presses the button multiple times during an operation. When setting the property back to "ready", the GUI re-enables
            // the button and the user can press it again.
            // To avoid the moduleMain- loop to awake every time we reset the trigger, provide a second parameter to the set() method. It denotes
            // whether the change notification should be fired or not. In our case, we avoid this by providing false to the second parameter.
            m_aTrigger->set( WPVBaseTypes::PV_TRIGGER_READY, false );
        }
    }

    // At this point, the container managing this module signalled to shutdown. The main loop has ended and you should clean up. Always remove
    // allocated memory and remove all OSG nodes.
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootNode );
}

void WMTemplate::SafeUpdateCallback::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
    // One note about m_aColor: As you might have notices, changing one of the properties, which recreate the OSG node, causes the material to be
    // gray again. This is simply caused by m_aColor->changed() is still false! To resolve this problem, use some m_osgNeedsUpdate boolean which
    // gets set in your thread main and checked here or, as done in this module, by checking whether the callback is called the first time.
    if ( m_module->m_aColor->changed() || m_initialUpdate )
    {
        // Grab the color
        WColor c = m_module->m_aColor->get( true );

        // Set the diffuse color and material:
        osg::ref_ptr< osg::Material > mat = new osg::Material();
        mat->setDiffuse( osg::Material::FRONT, osg::Vec4( c.getRed(), c.getGreen(), c.getBlue(), c.getAlpha() ) );
        node->getOrCreateStateSet()->setAttribute( mat, osg::StateAttribute::ON );
    }
    traverse( node, nv );
}

bool WMTemplate::StringLength::accept( boost::shared_ptr< WPropertyVariable< WPVBaseTypes::PV_STRING > > /* property */,
                                       WPVBaseTypes::PV_STRING value )
{
    // This method gets called everytime the m_aString property is going to be changed. It can decide whether the new value is valid or not. If
    // the method returns true, the new value is set. If it returns false, the value is rejected.
    //
    // Note: always use WPVBaseTypes when specializing the WPropertyVariable template.

    // simple example: just accept string which are at least 5 chars long and at most 10.
    return ( value.length() <= 10 ) && ( value.length() >= 5 );
}

void WMTemplate::activate()
{
    // This method gets called, whenever the m_active property changes. Your module should always handle this. For more details, see the
    // documentation in properties(). The most simple way is to activate or deactivate your OSG root node in this function according to
    // m_active's value. At the moment, we are not 100% sure whether deactivating a node, which is currently used, is thread-safe and complies to
    // OSG's requirements. Activating an inactive node is not the problem, as OSG does not traverse these nodes (and therefore could possibly
    // produce issues), but deactivating an active node, which might be traversed at the same time, COULD cause problems. We'll see in the future
    // whether this is problematic or not.

    if ( m_rootNode )   // always ensure the root node exists
    {
        if ( m_active->get() )
        {
            m_rootNode->setNodeMask( 0xFFFFFFFF );
        }
        else
        {
            m_rootNode->setNodeMask( 0x0 );
        }
    }

    // Always call WModule's activate!
    WModule::activate();
}

