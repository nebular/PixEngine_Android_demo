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
	const std::vector<std::pair<float, float>> stVecModelCar = { { 1,1 },{ 1,3 },{ 3,0 },{ 0,-3 },{ -3,0 },{ -1, 3 },{ -1,1 } };

	bool OnUserCreate();
	bool OnUserUpdate(float fElapsedTime);
	void DrawWireFrameModel( const std::vector<std::pair<float, float>> &vecModelCoordinates, float x, float y, float r, float s, std::vector<olc::Pixel> col);

};

#endif //GLES3JNI_DEMOENGINE_H
