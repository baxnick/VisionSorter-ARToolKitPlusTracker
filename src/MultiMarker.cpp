/*
 *	osgART/Tracker/ARToolKit/MultiMarker
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


#include "MultiMarker"

/*#include <AR/gsub_lite.h>*/
#include "gsub.h"

//#include <assert.h>

namespace osgART {

	MultiMarker::MultiMarker(ARToolKitPlus::Tracker *tracker) : m_tracker(tracker), Marker() {
	}

	MultiMarker::~MultiMarker() {   
		// jcl64: Free the multimarker
		if (m_multi) m_tracker->arMultiFreeConfig(m_multi);
	}

	/* virtual */
	Marker::MarkerType MultiMarker::getType() const {
		return Marker::ART_MULTI;
	}

	bool 
	MultiMarker::initialise(const std::string& multiFile) {

		// Check if multifile exists!!!
		m_multi = m_tracker->arMultiReadConfigFile(multiFile.c_str());
		if (m_multi == NULL) return false;
		
		setName(multiFile);
		setActive(false);

		return true;
	}

	void
	MultiMarker::setActive(bool a) {
		m_active = a;

                // not implemented
                //assert(0);
	
                /*        
		if (m_active) arMultiActivate(m_multi);
		else arMultiDeactivate(m_multi);
                */
	}

	void 
	MultiMarker::update(ARToolKitPlus::ARMarkerInfo* markerInfo, int markerCount) 
	{
		m_valid = (m_tracker->arMultiGetTransMat(markerInfo, markerCount, m_multi) >= 0);
		if (m_valid) {
			double modelView[16];
			arglCameraViewRH(m_multi->trans, modelView, 1.0); // scale = 1.0.
			osg::Matrix tmp(modelView);
			updateTransform(tmp);
		} else {
			m_seen = false;
		}
	}
	
        bool
        MultiMarker::represents(int markerID)
        {
            for (int i = 0; i < m_multi->marker_num; i++)
            {
                if (m_multi->marker[i].patt_id == markerID) return true;
            }

            return false;
        }
};
