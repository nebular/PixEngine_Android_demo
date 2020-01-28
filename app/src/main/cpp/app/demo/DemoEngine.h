//
// Created by rodo on 2020-01-24.
//

#ifndef DEMOENGINE_H
#define DEMOENGINE_H

#include "olcPixelGameEngine.h"

class DemoEngine : public olc::PixelGameEngine {

	bool OnUserCreate();
	bool OnUserUpdate(float fElapsedTime);
};


#endif //GLES3JNI_DEMOENGINE_H
