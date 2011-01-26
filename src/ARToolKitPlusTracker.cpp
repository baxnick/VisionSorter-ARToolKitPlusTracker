/*
 *	osgART/Tracker/ARToolKit/ARToolKitPlusTracker
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


#include "ARToolKitPlusTracker"

/*
#include <AR/config.h>
#include <AR/video.h>
#include <AR/ar.h>
#include <AR/gsub_lite.h>
#ifndef AR_HAVE_HEADER_VERSION_2_72
#error ARToolKit v2.72 or later is required to build the OSGART ARToolKit tracker.
#endif
*/

//#include <ARToolKitPlus/Tracker.h>

#include "gsub.h"

#include "SingleMarker"
#include "MultiMarker"
#include "SimpleMarker"

#include <osgART/GenericVideo>
#include <osg/Notify>

#include <iostream>
#include <fstream>

#include <ARToolKitPlus/TrackerMultiMarkerImpl.h>

#define PD_LOOP 3

using namespace ARToolKitPlus;

template <typename T> 
int Observer2Ideal(	const T dist_factor[4], 
					const T ox, 
					const T oy,
					T *ix, T *iy )
{
    T  z02, z0, p, q, z, px, py;
    register int i = 0;

    px = ox - dist_factor[0];
    py = oy - dist_factor[1];
    p = dist_factor[2]/100000000.0;
    z02 = px*px+ py*py;
    q = z0 = sqrt(px*px+ py*py);

    for( i = 1; ; i++ ) {
        if( z0 != 0.0 ) {
            z = z0 - ((1.0 - p*z02)*z0 - q) / (1.0 - 3.0*p*z02);
            px = px * z / z0;
            py = py * z / z0;
        }
        else {
            px = 0.0;
            py = 0.0;
            break;
        }
        if( i == PD_LOOP ) break;

        z02 = px*px+ py*py;
        z0 = sqrt(px*px+ py*py);
    }

    *ix = px / dist_factor[3] + dist_factor[0];
    *iy = py / dist_factor[3] + dist_factor[1];

    return(0);
}


namespace osgART {

            void Logger::artLog(const char* nStr) {
                osg::notify() << nStr << std::endl; 
            }


	struct ARToolKitPlusTracker::CameraParameter 
	{
		ARParam cparam;	
	};


	ARToolKitPlusTracker::ARToolKitPlusTracker() : GenericTracker(),
		m_threshold(100),
		m_debugmode(false),
		m_marker_num(0),
                m_logger(new Logger),
		m_cparam(new CameraParameter)
	{		

		// create a new field 
		m_fields["threshold"] = new CallbackField<ARToolKitPlusTracker,int>(this,
			&ARToolKitPlusTracker::getThreshold,
			&ARToolKitPlusTracker::setThreshold);
		
		// attach a new field to the name "debug"
		m_fields["debug"] = new TypedField<bool>(&m_debugmode);

		// for statistics
		m_fields["markercount"] = new TypedField<int>(&m_marker_num);
	}

	ARToolKitPlusTracker::~ARToolKitPlusTracker()
	{
		delete m_cparam;
	}


	bool ARToolKitPlusTracker::init(int xsize, int ysize, 
		const std::string& pattlist_name, 
		const std::string& camera_name)
        {

	    // Set the initial camera parameters.
		cparamName = camera_name;
	    //if(arParamLoad((char*)cparamName.c_str(), 2, &wparam) < 0) {

               
                m_tracker = new ARToolKitPlus::TrackerMultiMarkerImpl<6,6,6,9,9>(xsize,ysize);

                m_tracker->setMarkerMode(MARKER_ID_BCH);
                m_tracker->setBorderWidth(0.125);
                //m_tracker->setMarkerMode(MARKER_TEMPLATE);
                //m_tracker->setBorderWidth(0.25);


                // these are all accuracy enhancements, you can comment them out to return to the defaults
                m_tracker->activateAutoThreshold(true);
                m_tracker->setNumAutoThresholdRetries(5);
                m_tracker->setPoseEstimator(POSE_ESTIMATOR_RPP);
                m_tracker->setImageProcessingMode(IMAGE_FULL_RES);



	    //if(!m_tracker->loadCameraFile((char*)cparamName.c_str(),10.0f,8000.0f)) {
            // we use this "high level" init function because it sets up some things that i can't access
            // with the lower level functions (!!). we pass in an empty marker file because we want to add
            // markers one by one and keep track of them.
            
                float near = 10.0f;
                float far = 8000.0f;

                m_logger->artLog("CHECK 1,2,3... IS THIS THING ON?\n");

            if(!m_tracker->init((char*)cparamName.c_str(),"Data/empty.cfg",near,far,m_logger)) {
			
			// 
			osg::notify(osg::FATAL) 
				<< "osgART::ARToolKitPlusTracker::init(video,patternlist,cameraparam) Can't load camera parameters from '"<<
				camera_name <<"'." << std::endl;
			
			return false;
	    }

            //m_tracker->convertProjectionMatrixToOpenGLStyle(&(m_cparam->cparam), near, far, m_projectionMatrix);


        // slightly less ugly hack
        arglCameraFrustumRH(m_tracker->getCamera(), near, far, m_projectionMatrix);

/*
            printf("PROJECTION MATRIX:\n");
            for(int i=0;i<16;i++) {
                printf(" %f",m_projectionMatrix[i] = m_tracker->getProjectionMatrix()[i]);
                if(i%4==3) printf("\n");
            }
            */

	    //arParamChangeSize(&wparam, xsize, ysize,&(m_cparam->cparam));
            //Tracker::changeCameraSize(xsize,ysize);
	    //arInitCparam(&(m_cparam->cparam));
	    //arParamDisp(&(m_cparam->cparam));

		//arFittingMode = AR_FITTING_TO_IDEAL;
	    //arImageProcMode = AR_IMAGE_PROC_IN_FULL;

		//setProjection(10.0f, 8000.0f);
		setDebugMode(m_debugmode);
		setThreshold(m_threshold);

		if (!setupMarkers(pattlist_name)) {
			std::cerr << "ERROR: Marker setup failed." << std::endl;
			return false;
		}

		// Success
		return true;
	}


	std::string trim(std::string& s,const std::string& drop = " ")
	{
		std::string r=s.erase(s.find_last_not_of(drop)+1);
		return r.erase(0,r.find_first_not_of(drop));
	}


	bool ARToolKitPlusTracker::setupMarkers(const std::string& patternListFile)
	{
		std::ifstream markerFile;

		// Need to check whether the passed file even exists

		markerFile.open(patternListFile.c_str());

		// Need to check for error when opening file
		if (!markerFile.is_open()) return false;

		bool ret = true;

		int patternNum = 0;
		markerFile >> patternNum;

		std::string patternName, patternType;

		// Need EOF checking in here... atm it assumes there are really as many markers as the number says

		for (int i = 0; (i < patternNum) && (!markerFile.eof()); i++)
		{
			// jcl64: Get the whole line for the marker file (will handle spaces in filename)
			patternName = "";
			while (trim(patternName) == "" && !markerFile.eof()) {
				getline(markerFile, patternName);
			}
			
			
			// Check whether markerFile exists?

			markerFile >> patternType;

			if (patternType == "SINGLE")
			{
				
				double width, center[2];
				markerFile >> width >> center[0] >> center[1];
				if (addSingleMarker(patternName, width, center) == -1) {
					std::cerr << "Error adding single pattern: " << patternName << std::endl;
					ret = false;
					break;
				}

			}
			else if (patternType == "MULTI")
			{
				if (addMultiMarker(patternName) == -1) {
					std::cerr << "Error adding multi-marker pattern: " << patternName << std::endl;
					ret = false;
					break;
				}

			} 
			else if (patternType == "SIMPLE")
			{
				double width, center[2];
				markerFile >> width >> center[0] >> center[1];
				if (addSimpleMarker(patternName,width,center) == -1) {
					std::cerr << "Error adding simple-marker pattern: " << patternName << std::endl;
					ret = false;
					break;
				}

			} 
			else 
			{
				std::cerr << "Unrecognized pattern type: " << patternType << std::endl;
				ret = false;
				break;
			}
		}

		markerFile.close();

		return ret;
	}

	int 
	ARToolKitPlusTracker::addSingleMarker(const std::string& pattFile, double width, double center[2]) {

		SingleMarker* singleMarker = new SingleMarker(m_tracker);

		if (!singleMarker->initialise(pattFile, width, center))
		{
			singleMarker->unref();
			return -1;
		}

		m_markerlist.push_back(singleMarker);

		return m_markerlist.size() - 1;
	}

	int 
	ARToolKitPlusTracker::addSimpleMarker(const std::string& pattFile, double width, double center[2]) {

		SimpleMarker* simpleMarker = new SimpleMarker(m_tracker);

		if (!simpleMarker->initialise(atoi(pattFile.c_str()), width, center))
		{
			simpleMarker->unref();
			return -1;
		}

		m_markerlist.push_back(simpleMarker);

		return m_markerlist.size() - 1;
	}

	int 
	ARToolKitPlusTracker::addMultiMarker(const std::string& multiFile) 
	{
		MultiMarker* multiMarker = new MultiMarker(m_tracker);
		
		if (!multiMarker->initialise(multiFile))
		{
			multiMarker->unref();
			return -1;
		}

		m_markerlist.push_back(multiMarker);

		return m_markerlist.size() - 1;

	}

	void ARToolKitPlusTracker::setThreshold(const int& thresh)	
	{
		m_threshold = osg::clampBetween(thresh,0,255);		
	}

	int ARToolKitPlusTracker::getThreshold() const 
	{
		return m_threshold;
	}

	unsigned char* ARToolKitPlusTracker::getDebugImage() {
		//return arImage;
		return 0;
	}
		
	void ARToolKitPlusTracker::setDebugMode(bool d) 
	{
		m_debugmode = d;
		//arDebug = (m_debugmode) ? 1 : 0;
	}

	bool ARToolKitPlusTracker::getDebugMode() 
	{
		return m_debugmode;
	}

    /*virtual*/ 
	void ARToolKitPlusTracker::setImageRaw(unsigned char * image, PixelFormatType format)
    {
		if (m_imageptr_format != format) {
			// format has changed.
			// Translate the pixel format to an appropriate type for ARToolKit v2.
			switch (format) {
				case VIDEOFORMAT_RGB24:
					m_artoolkit_pixformat = PIXEL_FORMAT_RGB;
					m_artoolkit_pixsize = 3;
					break;
				case VIDEOFORMAT_BGR24:
					m_artoolkit_pixformat = PIXEL_FORMAT_BGR;
					m_artoolkit_pixsize = 3;
					break;
				case VIDEOFORMAT_BGRA32:
					m_artoolkit_pixformat = PIXEL_FORMAT_BGRA;
					m_artoolkit_pixsize = 4;
					break;
				case VIDEOFORMAT_RGBA32:
					m_artoolkit_pixformat = PIXEL_FORMAT_RGBA;
					m_artoolkit_pixsize = 4;
					break;
                                /*
				case VIDEOFORMAT_ARGB32:
					m_artoolkit_pixformat = PIXEL_FORMAT_ARGB;
					m_artoolkit_pixsize = 4;
					break;
                                */
				case VIDEOFORMAT_ABGR32:
					m_artoolkit_pixformat = PIXEL_FORMAT_ABGR;
					m_artoolkit_pixsize = 4;
					break;
                                        /*
                                case VIDEOFORMAT_YUV422:
					m_artoolkit_pixformat = PIXEL_FORMAT_2vuy;
					m_artoolkit_pixsize = 2;
					break;
                                        */
				case VIDEOFORMAT_Y8:
				case VIDEOFORMAT_GREY8:
					m_artoolkit_pixformat = PIXEL_FORMAT_LUM;
					m_artoolkit_pixsize = 1;
					break;
				default:
					break;
			}        
		}
		
        // We are only augmenting method in parent class.
        GenericTracker::setImageRaw(image, format);
    }

	void ARToolKitPlusTracker::update()
	{

		ARMarkerInfo    *marker_info;					// Pointer to array holding the details of detected markers.
		
		
	    register int             j, k;

		// Do not update with a null image
		if (m_imageptr == NULL) return;

#if 0
        // Check that the format matches the one passed in.
		if (AR_PIX_SIZE_DEFAULT != m_artoolkit_pixsize || AR_DEFAULT_PIXEL_FORMAT != m_artoolkit_pixformat) {
			std::cerr << "osgart_artoolkit_tracker::update() Incompatible pixelformat!" << std::endl;
			return;
		}
#endif

		// Detect the markers in the video frame.
		if(m_tracker->arDetectMarker(m_imageptr, m_threshold, &marker_info, &m_marker_num) < 0) 
		{
			std::cerr << "Error detecting markers in image." << std::endl;
			return;
		}

		MarkerList::iterator _end = m_markerlist.end();
			
		// Check through the marker_info array for highest confidence
		// visible marker matching our preferred pattern.
		for (MarkerList::iterator iter = m_markerlist.begin(); 
			iter != _end; 
			++iter)		
		{

			SingleMarker* singleMarker = dynamic_cast<SingleMarker*>((*iter).get());
			SimpleMarker* simpleMarker = dynamic_cast<SimpleMarker*>((*iter).get());
			MultiMarker* multiMarker = dynamic_cast<MultiMarker*>((*iter).get());

			if (singleMarker)
			{			

				k = -1;
				for (j = 0; j < m_marker_num; j++)	
				{
					if (singleMarker->getPatternID() == marker_info[j].id) 
					{
						if (k == -1) k = j; // First marker detected.
						else 
						if(marker_info[j].cf > marker_info[k].cf) k = j; // Higher confidence marker detected.

					}
				}
					
				if(k != -1) 
				{
					singleMarker->update(&marker_info[k]); 
				} 
				else 
				{
					singleMarker->update(NULL);
				}
			}
                        else if (simpleMarker)
			{			
                                    //osg::notify() << "testing simpleMarker, name: " << simpleMarker->getName() << std::endl;

				k = -1;
				for (j = 0; j < m_marker_num; j++)	
				{
                                    //osg::notify() << "testing detected marker, id: " << marker_info[j].id << std::endl;

					if (simpleMarker->getPatternID() == marker_info[j].id) 
					{
                                            //osg::notify() << "matched marker, name: " << simpleMarker->getName() << ", id=" << marker_info[j].id << std::endl;
						if (k == -1) k = j; // First marker detected.
						else 
						if(marker_info[j].cf > marker_info[k].cf) k = j; // Higher confidence marker detected.

					}
				}
					
				if(k != -1) 
				{
					simpleMarker->update(&marker_info[k]); 
				} 
				else 
				{
					simpleMarker->update(NULL);
				}
                                osg::Vec3d trans = simpleMarker->getTransform().getTrans();
                                //printf("simple marker %s transform: %f %f %f %s\n",simpleMarker->getName().c_str(),trans.x(),trans.y(),trans.z(),simpleMarker->isValid()?"valid":"invalid");
			}
			else if (multiMarker)
			{
				multiMarker->update(marker_info, m_marker_num);
				
			} else {
				std::cerr << "ARToolKitPlusTracker::update() : Unknown marker type id!" << std::endl;
			}
		}
// #endif

	}

	void ARToolKitPlusTracker::setProjection(const double n, const double f) 
	{
            m_tracker->setCamera(m_tracker->getCamera(),n,f);

        arglCameraFrustumRH(m_tracker->getCamera(), n, f, m_projectionMatrix);
            //convertProjectionMatrixToOpenGLStyle(&(m_cparam->cparam), n, f, m_projectionMatrix);
	}


        Marker* ARToolKitPlusTracker::getMarker(int markerId) {
		MarkerList::iterator _end = m_markerlist.end();

                int i = 0;
                for (MarkerList::iterator iter = m_markerlist.begin();
			iter != _end; 
			++iter)		
                {
                        SingleMarker* singleMarker = dynamic_cast<SingleMarker*>((*iter).get());
                        SimpleMarker* simpleMarker = dynamic_cast<SimpleMarker*>((*iter).get());
                        MultiMarker* multiMarker = dynamic_cast<MultiMarker*>((*iter).get());

                        if (singleMarker && singleMarker->getPatternID()==markerId)
                            return singleMarker;
                        else if (simpleMarker && simpleMarker->getPatternID()==markerId)
                            return simpleMarker;
                        else if (multiMarker && i == markerId || multiMarker->represents(markerId))
                            return multiMarker;

                        i++;
                }

                return NULL;

        }

	void ARToolKitPlusTracker::createUndistortedMesh(
		int width, int height,
		float maxU, float maxV,
		osg::Geometry &geometry)
	{

		osg::Vec3Array *coords = dynamic_cast<osg::Vec3Array*>(geometry.getVertexArray());
		osg::Vec2Array* tcoords = dynamic_cast<osg::Vec2Array*>(geometry.getTexCoordArray(0));
						
		unsigned int rows = 20, cols = 20;
		float rowSize = height / (float)rows;
		float colSize = width / (float)cols;
		ARFloat x, y, px, py, u, v;

		for (unsigned int r = 0; r < rows; r++) {
			for (unsigned int c = 0; c <= cols; c++) {

				x = c * colSize;
				y = r * rowSize;

				m_tracker->getCamera()->observ2Ideal(x, y, &px, &py);
				coords->push_back(osg::Vec3(px, py, 0.0f));

				u = (c / (float)cols) * maxU;
				v = (1.0f - (r / (float)rows)) * maxV;
				tcoords->push_back(osg::Vec2(u, v));

				x = c * colSize;
				y = (r+1) * rowSize;

				m_tracker->getCamera()->observ2Ideal(x, y, &px, &py);
				coords->push_back(osg::Vec3(px, py, 0.0f));

				u = (c / (float)cols) * maxU;
				v = (1.0f - ((r+1) / (float)rows)) * maxV;
				tcoords->push_back(osg::Vec2(u, v));

			}

			geometry.addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUAD_STRIP, 
				r * 2 * (cols+1), 2 * (cols+1)));
		}
	}

}; // namespace osgART
