/*
 *	osgART/Tracker/ARToolKit/SimpleMarker
 *	osgART: AR ToolKit for OpenSceneGraph
 *
 *	Copyright (c) 2005-2007 ARToolworks, Inc. All rights reserved.
 *	
 *	Rev		Date		Who		Changes
 *  1.0   	2006-12-08  ---     Version 1.0 release.
 *
 */
/*
 * This file is part of osgART - AR Toolkit for OpenSceneGraph
 *
 * Copyright (c) 2005-2007 ARToolworks, Inc. All rights reserved.
 *
 * (See the AUTHORS file in the root of this distribution.)
 *
 *
 * OSGART is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * OSGART is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OSGART; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */


#include "SimpleMarker"

#include <ARToolKitPlus/Tracker.h>
/*#include <AR/gsub_lite.h>*/
#include "gsub.h"

namespace osgART {

	SimpleMarker::SimpleMarker(ARToolKitPlus::Tracker *tracker) : m_tracker(tracker), Marker(),
		patt_id(-1)
	{
			m_fields["confidence"] = new TypedField<double>(&m_confidence);
	}

	SimpleMarker::~SimpleMarker()
	{
		// jcl64: Free the pattern
		//if (patt_id >= 0) m_tracker->arFreePatt(patt_id);
		//patt_id = -1;
	}

	Marker::MarkerType SimpleMarker::getType() const
	{
		return Marker::ART_SINGLE;
	}

	bool SimpleMarker::initialise(int id, double width, double center[2])
	{
            char name[1024];


            //printf("1: patt_id=%d\n",patt_id);
		if (patt_id >= 0) return (false);
		patt_id = id;
            //printf("2: patt_id=%d\n",patt_id);
		if (patt_id < 0) return false;
		patt_width = width;
		patt_center[0] = center[0];
		patt_center[1] = center[1];
                snprintf(name,1024,"simple%d",patt_id);
		setName(name);
		setActive(false);
            //printf("3: patt_id=%d\n",patt_id);
		return true;
	}

	void SimpleMarker::update(ARToolKitPlus::ARMarkerInfo* markerInfo)
	{
		if (markerInfo == 0L) {
			m_valid = false;
			m_seen = false;
		} else {
			m_valid = true;
			//arGetTransMatCont(markerInfo, patt_trans, patt_center, patt_width, patt_trans);
			m_tracker->arGetTransMat(markerInfo, patt_center, patt_width, patt_trans);
			m_confidence = markerInfo->cf;
			double modelView[16];
			arglCameraViewRH(patt_trans, modelView, 1.0); // scale = 1.0.
			osg::Matrix tmp(modelView);
			updateTransform(tmp);
		}
	}

	void SimpleMarker::setActive(bool a)
	{
		m_active = a;
	
        /*        
		if (m_active) arActivatePatt(patt_id);
		else arDeactivatePatt(patt_id);
                */

	}

	int SimpleMarker::getPatternID()
	{
		return patt_id;
	}

	double SimpleMarker::getPatternWidth()
	{
		return patt_width;
	}
		
	ARFloat* SimpleMarker::getPatternCenter()
	{
		return patt_center;
	}
};
