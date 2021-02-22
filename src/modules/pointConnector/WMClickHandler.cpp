#include "WMClickHandler.h"

WMClickHandler::WMClickHandler(WMPointConnector* connector):
    m_connector( connector )
{
}

bool WMClickHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
    osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*>(&aa);

    if(ea.getEventType() == osgGA::GUIEventAdapter::RELEASE)
    {
        osg::Camera* camera = viewer->getCamera();
        osg::Viewport* viewport = camera->getViewport();
        double halfWidth = viewport->width() * 0.5;
        double halfHeight = viewport->height() * 0.5;

        if(!halfWidth || !halfHeight)
        {
            return false;
        }

        double mouseX = (ea.getX() - halfWidth) / halfWidth;
        double mouseY = (ea.getY() - halfHeight) / halfHeight;
        
        osg::Vec3 mousePoint = mouseToWorldCoordinates( camera, mouseX, mouseY );
        osg::Vec3d cameraPosition = viewer->getCameraManipulator()->getMatrix().getTrans();
        

        std::cout << cameraPosition.x() << " " << cameraPosition.y() << " " << cameraPosition.z() << std::endl;
        std::cout << mousePoint.x() << " " << mousePoint.y() << " " << mousePoint.z() << std::endl;

        osg::Vec3 direction = mousePoint - cameraPosition;
        direction.normalize();

        m_connector->handleClick( cameraPosition, direction );
    }
    return false;
}

osg::Vec3 WMClickHandler::mouseToWorldCoordinates(osg::Camera* camera, double mouseX, double mouseY)
{
    osg::Matrix VP = camera->getViewMatrix() * camera->getProjectionMatrix();

    osg::Matrix inverseVP;
    inverseVP.invert( VP );

    // compute world near far
    osg::Vec3 nearPoint( mouseX, mouseY, -1.0f );
    osg::Vec3 nearPointWorld = nearPoint * inverseVP;

    return nearPointWorld;
}