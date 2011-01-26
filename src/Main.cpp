/*
 *	osgART/Tracker/ARToolKit/Main
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


#include "osgART/VideoPlugin"
#include "osgART/VideoConfig"
#include "osgART/GenericVideo"

#include "ARToolKitPlusTracker"

DLL_API osgART::GenericTracker* osgart_create_tracker()
{
	return new osgART::ARToolKitPlusTracker();
}

OSGART_PLUGIN_ENTRY()
