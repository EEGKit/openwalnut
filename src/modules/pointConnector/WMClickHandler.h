#ifndef WMCLICKHANDLER_H
#define WMCLICKHANDLER_H

#include <iostream>
#include <string>

#include <osg/Geode>
#include <osg/Viewport>
#include <osgGA/GUIEventHandler>
#include <osgViewer/viewer>

#include "WMPointConnector.h"

class WMClickHandler : public osgGA::GUIEventHandler
{
public:
    WMClickHandler(WMPointConnector* connector);

    bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

private:
    osg::Vec3 mouseToWorldCoordinates(osg::Camera* camera, double mouseX, double mouseY);

    WMPointConnector* m_connector;
};

#endif  //WMCLICKHANDLER_H

