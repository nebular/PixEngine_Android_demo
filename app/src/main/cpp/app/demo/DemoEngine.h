//
// Created by rodo on 2020-01-24.
//

#ifndef DEMOENGINE_H
#define DEMOENGINE_H

#include "olcPixelGameEngine.h"
#include "plugins/lonescreenkey.h"

class DemoEngine : public olc::PixelGameEngine {

	olc::Sprite *pSprite;
	bool OnUserCreate();
	bool OnUserUpdate(float fElapsedTime);
};

class DemoControls : public olc::PixelGameEngine {
	olc::Sprite *pSprite;
	int nX, nY, lastClickX, lastClickY;

	bool OnUserCreate();
	bool OnUserUpdate(float fElapsedTime);
};
class DemoGyro : public olc::PixelGameEngine {
	olc::Sprite *pSprite;
	int nX, nY;

	bool OnUserCreate();
	bool OnUserUpdate(float fElapsedTime);
};

#endif //GLES3JNI_DEMOENGINE_H
