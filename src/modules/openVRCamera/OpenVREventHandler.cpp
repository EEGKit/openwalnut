/*
 * openvreventhandler.cpp
 *
 *  Created on: Dec 18, 2015
 *      Author: Chris Denham
 */

#include "OpenVREventHandler.h"
#include "OpenVRDevice.h"

bool OpenVREventHandler::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& ad)
{
	switch (ea.getEventType())
	{
		case osgGA::GUIEventAdapter::KEYUP:
		{
			switch (ea.getKey())
			{
				case osgGA::GUIEventAdapter::KEY_R:
					m_openvrDevice->resetSensorOrientation();
					break;
			}
		}
	}

	return osgGA::GUIEventHandler::handle(ea, ad);
}
