//
// Created by rodo on 2020-01-24.
//

#ifndef DEMOENGINE_H
#define DEMOENGINE_H

#include "PixEngine.hpp"
#include "arch/android/plugins/lonescreenkey.h"
#include "items/Canvas2D.hpp"
#include "Font.hpp"
#include "input/GyroController.hpp"

class DemoEngine : public rgl::PixEngine {

//	olc::Sprite *pSprite;
	rgl::Canvas2D *pCanvas;

public:
	DemoEngine();
	bool onUserCreate();
	bool onUserUpdate(float fElapsedTime);
};

class DemoControls : public rgl::PixEngine {

//	olc::Sprite *pSprite;
	int nX, nY, lastClickX, lastClickY;
	rgl::Canvas2D *pCanvas;

public:
	DemoControls();

	bool onUserCreate();
	bool onUserUpdate(float fElapsedTime);
};

class DemoGyro : public rgl::PixEngine {

public:
	DemoGyro();
	rgl::Canvas2D *pCanvas;

	//	olc::Sprite *pSprite;
	int nX, nY;
	const std::vector<std::pair<float, float>> stVecModelCar = { { 1,1 },{ 1,3 },{ 3,0 },{ 0,-3 },{ -3,0 },{ -1, 3 },{ -1,1 } };

	bool onUserCreate();
	bool onUserUpdate(float fElapsedTime);

};

class DemoGyro2 : public rgl::PixEngine {

public:
	DemoGyro2();
	rgl::Canvas2D *pCanvas;

	float fAngle = 0, fPosX=100, fPosY=100;

	const std::vector<std::pair<float, float>> stVecModelCar = { { 1,1 },{ 1,3 },{ 3,0 },{ 0,-3 },{ -3,0 },{ -1, 3 },{ -1,1 } };

	bool onUserCreate();
	bool onUserUpdate(float fElapsedTime);

};



class Demo1 : public rgl::PixEngine {

	rgl::Canvas2D *pCanvas;
	float pop = 0;

public:

	Demo1();

	bool onUserCreate();
	bool onUserUpdate(float fElapsedTime);

};

#endif //GLES3JNI_DEMOENGINE_H
