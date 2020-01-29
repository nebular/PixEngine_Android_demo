//
// OneLoneCoder Pixel Game Engine Android Template
// Author: Rodolfo Lopez Pintor <rlp@nebular.tv>
//
// License: Creative commons CC-BY
// Created by rodo on 2020-01-24.
//
// On the project tree you have "olcpge_android" as a module with the release version of
// the android/PixelGameEngine library. You may need to update that file for newer versions.
// You can also build the project yourself, just clone https://github.com/nebular/olcPGE_Android
// and build your customized launcher
//
// This first version of the engine launchas PGE using Logical Pixels (so that would be around 640x360)
// and locks landscape mode. The next version will allow you to select a resolution and orientation.


#include "olcPixelGameEngine.h"

#include "DemoEngine.h"
#include "plugins/lonescreenkey.h"


/**
 * HELLO PGE WORLD
 */

bool DemoEngine::OnUserCreate() {
	Clear(olc::BLUE);
	pSprite = new olc::Sprite("android.png");                // root of assets folder, subdirs are also ok
	return true;
}

bool DemoEngine::OnUserUpdate(float fElapsedTime) {
	DrawString(100, 100, "HELLO OLC PGE!", olc::GREEN, 4.0);
	SetPixelMode(olc::Pixel::ALPHA);
	DrawSprite(160, 200, pSprite, 1);

	return true;
}


/**
 * HELLO CONTROLS
 */

bool DemoControls::OnUserCreate() {

	LoneScreenKey::currentInstance->clear();
	LoneScreenKey::currentInstance->reset();
	LoneScreenKey::currentInstance->addCursors(0,120);
	LoneScreenKey::currentInstance->addMouse(ScreenWidth()-100,0);

	pSprite = new olc::Sprite("android.png");                // root of assets folder, subdirs are also ok

	nX = 160;
	nY = 200;

	return true;
}

bool DemoControls::OnUserUpdate(float fElapsedTime) {

	Clear(olc::BLUE);
	LoneScreenKey::currentInstance->DrawSelf(this, olc::WHITE, true);

	SetPixelMode(olc::Pixel::ALPHA);
	DrawSprite(nX, nY, pSprite, 1);


	if (GetMouse(0).bPressed) {
		lastClickX = GetMouseX();
		lastClickY = GetMouseY();
	}


	DrawString(130, 0,
			   "Mouse X " + std::to_string(GetMouseX()) + " Y " + std::to_string(GetMouseY()), olc::WHITE, 2);
	DrawString(130, 20,
			   "B0 Pres " + std::to_string(GetMouse(0).bPressed)
			   + " Held " + std::to_string(GetMouse(0).bHeld)
			   + " Rels " + std::to_string(GetMouse(0).bReleased), olc::WHITE, 2);
	DrawString(130, 40,
			   "B1 Pres " + std::to_string(GetMouse(1).bPressed)
			   + " Held " + std::to_string(GetMouse(1).bHeld)
			   + " Rels " + std::to_string(GetMouse(1).bReleased), olc::WHITE, 2);

	DrawString(130, 60,
			   "B0clk X " + std::to_string(lastClickX)
			   + " Y " + std::to_string(lastClickY), olc::WHITE, 2);


	if (GetKey(olc::Key::LEFT).bHeld) {
		nX--;
		if (nX < 0) nX = 0;
	}
	if (GetKey(olc::Key::RIGHT).bHeld) {
		nX++;
		if (nX > ScreenWidth() - 200) nX = ScreenWidth() - 200;
	}
	if (GetKey(olc::Key::UP).bHeld) {
		nY--;
		if (nY < 0) nY = 0;
	}
	if (GetKey(olc::Key::DOWN).bHeld) {
		nY++;
		if (nY > ScreenHeight() - 100) nY = ScreenHeight() - 100;
	}


	return true;
}


/**
 * HELLO GYROSCOPE
 */

bool DemoGyro::OnUserCreate() {
/*
	LoneScreenKey::currentInstance->add({olc::Key::UP, 0, 0, 100, 50});
	LoneScreenKey::currentInstance->add({olc::Key::LEFT, 0, 50, 50, 100});
	LoneScreenKey::currentInstance->add({olc::Key::RIGHT, 50, 50, 100, 100});
	LoneScreenKey::currentInstance->add({olc::Key::DOWN, 0, 100, 100, 150});

	LoneScreenKey::currentInstance->add({0, ScreenWidth()-100, 0, ScreenWidth()-50, 50, true});
	LoneScreenKey::currentInstance->add({1, ScreenWidth()-50, 0, ScreenWidth(), 50, true});

	pSprite = new olc::Sprite(
			"android.png");                // root of assets folder, subdirs are also ok

	nX = 160;
	nY = 200;
*/
	return true;
}

void DemoGyro::DrawWireFrameModel( const std::vector<std::pair<float, float>> &vecModelCoordinates, float x, float y, float r, float s, std::vector<olc::Pixel> col)
{
	// pair.first = x coordinate
	// pair.second = y coordinate

	// Create translated model vector of coordinate pairs
	std::vector<std::pair<float, float>> vecTransformedCoordinates;
	long verts = vecModelCoordinates.size();
	vecTransformedCoordinates.resize(verts);

	// Rotate
	for (int i = 0; i < verts; i++)
	{
		vecTransformedCoordinates[i].first = vecModelCoordinates[i].first * cosf(r) - vecModelCoordinates[i].second * sinf(r);
		vecTransformedCoordinates[i].second = vecModelCoordinates[i].first * sinf(r) + vecModelCoordinates[i].second * cosf(r);

		// Scale
		vecTransformedCoordinates[i].first = vecTransformedCoordinates[i].first * s;
		vecTransformedCoordinates[i].second = vecTransformedCoordinates[i].second * s;

		// Translate
		vecTransformedCoordinates[i].first = vecTransformedCoordinates[i].first + x;
		vecTransformedCoordinates[i].second = vecTransformedCoordinates[i].second + y;
	}

	// Draw Closed Polygon
	for (int i = 0; i < verts + 1; i++)
	{
		int j = (i + 1);
		DrawLine((int)vecTransformedCoordinates[i % verts].first, (int)vecTransformedCoordinates[i % verts].second,
				 (int)vecTransformedCoordinates[j % verts].first, (int)vecTransformedCoordinates[j % verts].second, col[i%col.size()]);
	}
}


bool DemoGyro::OnUserUpdate(float fElapsedTime) {

	auto todeg = [](float rad) { return rad * 180 / 3.14159265; };

	Clear(olc::BLUE);
//	LoneScreenKey::currentInstance->DrawSelf(this, olc::WHITE);

	float roll = tCurrentEvent.vector.roll;
	float azimuth = tCurrentEvent.vector.azimuth;
	float pich = tCurrentEvent.vector.pitch;

	DrawString(50, 270,"ROLL", olc::RED, 2.0);
	DrawString(250,270,"AZIMUTH", olc::RED, 2.0);
	DrawString(450,270,"PITCH", olc::RED, 2.0);
	DrawString(50, 300,std::to_string(todeg(roll)), olc::YELLOW, 2.0);
	DrawString(250,300,std::to_string(todeg(azimuth)), olc::YELLOW, 2.0);
	DrawString(450,300,std::to_string(todeg(pich)), olc::YELLOW, 2.0);

	DrawLine(500,50,500,200,olc::WHITE);
	FillCircle(500, 50 + 75 + 75 * sin(-pich), 5.0, olc::YELLOW);

	DrawLine(250, 125, 400, 125,olc::WHITE);
	FillCircle(250 + 75 + 75 * sin(azimuth), 125, 5.0, olc::YELLOW);

	DrawWireFrameModel(stVecModelCar,100, 125, -roll, 20.0, {olc::YELLOW});

	//	SetPixelMode(olc::Pixel::ALPHA);
//	DrawSprite(nX, nY, pSprite, 1);

return true;
}
