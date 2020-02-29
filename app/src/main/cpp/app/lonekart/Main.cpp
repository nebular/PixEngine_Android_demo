//
//  Main.cpp
//  PixEngine
//
//  Created by rodo on 16/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#include "Main.hpp"
#include "Mouse.hpp"
#include "Arena.hpp"
#include "Utils.hpp"
#include <cmath>
#include "ext/world/Player.hpp"
#include "input/GyroController.hpp"

using namespace rgl;


DemoWorld::DemoWorld() : rgl::PixFu("Hello World") {
	rgl::GyroController::enable(100, 100);
	addInputDevice(rgl::GyroController::instance());
}

DemoWorld::~DemoWorld() {
	delete pArena;
	pArena = nullptr;
}

bool DemoWorld::onUserCreate(bool restarted)  {
	
	pArena = new Arena(
						"cheeseland",			// level name
						{
							"world",			// shader
							{0.3,0.3,0.9},		// background color
							{0,20000,0},	// light position
							{0.8,0.8,0.9},		// light color
							World::TRANSFORM_NONE, // FLIPZ_ROT, // world TRANSFORM_FLIPZ_ROT,
							World::TRANSFORM_NONE  // terain
						},
					    World::PERSP_FOV60_MID
					);

	addExtension(pArena);
	
	return true;
}

bool DemoWorld::onUserUpdate(float fElapsedTime)  {
	pop += fElapsedTime;
	canvas()->blank();

	canvas()->fillCircle(
						 rgl::Mouse::x(),
						 rgl::Mouse::y(),
						 30,
						 rgl::Mouse::isHeld(0)
						 ? rgl::Pixel(255,0,0,255)
						 : rgl::Pixel(255,255,255,255)
						 );

	return true;
}

