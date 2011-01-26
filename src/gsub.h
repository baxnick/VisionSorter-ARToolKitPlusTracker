#ifndef _GSUB_HEADER_
#define _GSUB_HEADER_

#include <ARToolKitPlus/ar.h>

using namespace ARToolKitPlus;

// this is pretty ugly but just dumping it here...
void arglCameraViewRH(const ARFloat para[3][4], GLdouble m_modelview[16], const double scale);

void arglCameraFrustumRH(const ARParam *cparam, const double focalmin, const double focalmax, GLdouble m_projection[16]);

#endif
