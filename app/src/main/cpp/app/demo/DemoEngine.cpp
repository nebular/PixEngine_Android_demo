//
// PixFU Android Template
// Author: Rodolfo Lopez Pintor <rlp@nebular.tv>
//
// License: Creative commons CC-BY
// Created by rodo on 2020-01-24.
//
// On the project tree you have "PixFu" as a module with the release version of
// the android/PixelGameEngine library. You may need to update that file for newer versions.
// You can also build the project yourself, just clone https://github.com/nebular/olcPGE_Android
// and build your customized launcher
//
// This first version of the engine launchas PGE using Logical Pixels (so that would be around 640x360)
// and locks landscape mode. The next version will allow you to select a resolution and orientation.


#include "PixFu.hpp"

#include "DemoEngine.h"

#include "arch/android/platform_android.hpp"
#include "arch/android/plugins/lonescreenkey.h"

#include "items/Font.hpp"
#include "input/Mouse.hpp"
#include "input/Keyboard.hpp"
#include "input/GyroController.hpp"

/**
 * HELLO PGE WORLD
 */

Demo1::Demo1() {};

bool Demo1::onUserCreate() {
	pCanvas = new rgl::Canvas2D(buffer(), new rgl::Font());
	return true;
}

bool Demo1::onUserUpdate(float fElapsedTime) {
	pop += fElapsedTime;

	pCanvas->clear(rgl::Pixel(255 * sinf(pop), 255 * cosf(pop), 255 * sinf(pop), 255));
/*
	pCanvas->fillCircle(screenWidth() / 2, screenHeight() / 2, screenWidth() / 2 * sinf(pop),
						rgl::Pixel(0, 0, 255, 255));
*/
	pCanvas->drawString(10, 250 + 50 * sinf(pop), "PixFu 1.0", rgl::Pixel(0, 255, 0, 255), 7);

	pCanvas->fillCircle(rgl::Mouse::x(), rgl::Mouse::y(), 30,
						rgl::Mouse::isHeld(0) ? rgl::Pixel(255, 0, 0, 255) : rgl::Pixel(255, 255,
																						255, 255));

	float fpsec = 1 / fElapsedTime;
	pCanvas->drawString(10, 10, std::to_string((int) fpsec) + " fps", rgl::Colors::MAGENTA, 2);

	return true;
}


bool DemoEngine::onUserCreate() {


	pCanvas = new rgl::Canvas2D(buffer(), new rgl::Font());
	pCanvas->clear(rgl::Colors::BLUE);

//	pSprite = new olc::Sprite("android.png");                // root of assets folder, subdirs are also ok
	return true;
}

bool DemoEngine::onUserUpdate(float fElapsedTime) {
	pCanvas->drawString(100, 100, "HELLO PIXENGINE!", rgl::Colors::GREEN, 4.0);
//	SetPixelMode(olc::Pixel::ALPHA);
//	DrawSprite(160, 200, pSprite, 1);

	return true;
}

DemoEngine::DemoEngine() {}


/**
 * HELLO CONTROLS
 */

DemoControls::DemoControls() {}

bool DemoControls::onUserCreate() {

	rgl::LoneScreenKey::currentInstance->clear();
	rgl::LoneScreenKey::currentInstance->reset();
	rgl::LoneScreenKey::currentInstance->addCursors(0, 120);
	rgl::LoneScreenKey::currentInstance->addMouse(screenWidth() - 100, 0);

	nX = 160;
	nY = 200;

	pCanvas = new rgl::Canvas2D(buffer(), new rgl::Font());

	return true;
}

bool DemoControls::onUserUpdate(float fElapsedTime) {

	pCanvas->clear(rgl::Colors::BLUE);

	rgl::LoneScreenKey::currentInstance->DrawSelf(pCanvas, rgl::Colors::WHITE, true);

	pCanvas->fillCircle(nX, nY, 14, rgl::Colors::CYAN);

	if (rgl::Mouse::isPressed(0)) {
		lastClickX = rgl::Mouse::x();
		lastClickY = rgl::Mouse::y();
	}


	pCanvas->drawString(130, 0,
						"Mouse X " + std::to_string(rgl::Mouse::x()) + " Y " +
						std::to_string(rgl::Mouse::y()), rgl::Colors::WHITE, 2);
	pCanvas->drawString(130, 20,
						"B0 Pres " + std::to_string(rgl::Mouse::isPressed(0))
						+ " Held " + std::to_string(rgl::Mouse::isHeld(0))
						+ " Rels " + std::to_string(rgl::Mouse::isReleased(0)), rgl::Colors::WHITE,
						2);
	pCanvas->drawString(130, 40,
						"B1 Pres " + std::to_string(rgl::Mouse::isPressed(1))
						+ " Held " + std::to_string(rgl::Mouse::isHeld(1))
						+ " Rels " + std::to_string(rgl::Mouse::isReleased(1)), rgl::Colors::WHITE,
						2);

	pCanvas->drawString(130, 60,
						"B0clk X " + std::to_string(lastClickX)
						+ " Y " + std::to_string(lastClickY), rgl::Colors::WHITE, 2);


	if (rgl::Keyboard::isHeld(rgl::Keys::LEFT)) {
		nX--;
		if (nX < 0) nX = 0;
	}
	if (rgl::Keyboard::isHeld(rgl::Keys::RIGHT)) {
		nX++;
		if (nX > screenWidth() - 200) nX = screenWidth() - 200;
	}
	if (rgl::Keyboard::isHeld(rgl::Keys::UP)) {
		nY--;
		if (nY < 0) nY = 0;
	}
	if (rgl::Keyboard::isHeld(rgl::Keys::DOWN)) {
		nY++;
		if (nY > screenHeight() - 100) nY = screenHeight() - 100;
	}


	return true;
}


/**
 * HELLO GYROSCOPE
 */

DemoGyro::DemoGyro() {
	rgl::GyroController::enable(100, 100);
	addInputDevice(rgl::GyroController::currentInstance());
}


bool DemoGyro::onUserCreate() {

	pCanvas = new rgl::Canvas2D(buffer(), new rgl::Font());

	nX = 160;
	nY = 200;

	return true;
}

bool DemoGyro::onUserUpdate(float fElapsedTime) {

	auto todeg = [](float rad) { return rad * 180 / 3.14159265; };

	pCanvas->clear(rgl::Colors::BLUE);
	rgl::LoneScreenKey::currentInstance->DrawSelf(pCanvas, rgl::Colors::WHITE);

	ASensorEvent tCurrentEvent = rgl::GyroController::currentInstance()->raw();
	float roll = tCurrentEvent.vector.roll;
	float azimuth = tCurrentEvent.vector.azimuth;
	float pich = tCurrentEvent.vector.pitch;

	pCanvas->drawString(50, 270, "ROLL", rgl::Colors::RED, 2.0);
	pCanvas->drawString(250, 270, "AZIMUTH", rgl::Colors::RED, 2.0);
	pCanvas->drawString(530, 270, "PITCH", rgl::Colors::RED, 2.0);
	pCanvas->drawString(50, 300, std::to_string(todeg(roll)), rgl::Colors::YELLOW, 2.0);
	pCanvas->drawString(250, 300, std::to_string(todeg(azimuth)), rgl::Colors::YELLOW, 2.0);
	pCanvas->drawString(450, 300, std::to_string(todeg(pich)), rgl::Colors::YELLOW, 2.0);

	pCanvas->drawWireFrameModel(stVecModelCar, 100, 125, -roll, 20.0, {rgl::Colors::YELLOW});

	rgl::GyroController::currentInstance()->drawSelf(pCanvas, rgl::Colors::CYAN);

	return true;
}


DemoGyro2::DemoGyro2() {
	rgl::GyroController::enable(100, 100);
	addInputDevice(rgl::GyroController::currentInstance());
}


bool DemoGyro2::onUserCreate() {

	pCanvas = new rgl::Canvas2D(buffer(), new rgl::Font());

	return true;
}

bool DemoGyro2::onUserUpdate(float fElapsedTime) {

	auto todeg = [](float rad) { return rad * 180 / 3.14159265; };

	pCanvas->clear(rgl::Colors::BLUE);


	float xn = rgl::GyroController::currentInstance()->xNorm();
	float yn = rgl::GyroController::currentInstance()->yNorm();

	fAngle += xn * fElapsedTime * 10;

	fPosX -= yn * cosf(fAngle) * fElapsedTime * 100;
	fPosY -= yn * sinf(fAngle) * fElapsedTime * 100;

	fPosX = fmod(fPosX, screenWidth());
	fPosY = fmod(fPosY, screenHeight());

	pCanvas->drawString(50, 270, "xn", rgl::Colors::RED, 2.0);
	pCanvas->drawString(250, 270, "yn", rgl::Colors::RED, 2.0);
	pCanvas->drawString(50, 300, std::to_string(xn), rgl::Colors::YELLOW, 2.0);
	pCanvas->drawString(250, 300, std::to_string(yn), rgl::Colors::YELLOW, 2.0);

	pCanvas->drawWireFrameModel(stVecModelCar, fPosX, fPosY, fAngle, 20.0, {rgl::Colors::YELLOW});

	rgl::GyroController::currentInstance()->drawSelf(pCanvas, rgl::Colors::CYAN);

	return true;
}
