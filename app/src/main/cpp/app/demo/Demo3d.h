//
//  demo_3d.h
//  3Demo
//
//  Created by rodo on 02/03/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#pragma once
#include "WorldMeta.hpp"
#include "World.hpp"
#include "StaticObject.hpp"

const Pix::WorldConfig_t WORLDCONFIG = {
		"world",
		{0.9,0.9,0.4},		// background color
		{20000,20000,2000},	// light position
		{0.9,0.0,0.0}		// light color
};

const Pix::TerrainConfig_t TERRAINCONFIG = {
		"floor",		// name (determines texture, obj mesh and heightmap file)
		{0,0},			// terrain placement (for multi-terrain)
		0.1,			// scale height coordinate, 0.1 is a nice factor
		&Pix::PLANE,	// do not load a model
		true			// render grid
};

class Demo3dWorld:public Pix::World {

public:
	Demo3dWorld():World(WORLDCONFIG) {
		add(TERRAINCONFIG);
	}
};

class Demo3d:public Pix::Fu {

	Demo3dWorld *mWorld;

public:

	Demo3d() {

		// Add the world extension on the constructor
		// Extensions added on the contructor are rendered
		// before the UI layer. This makes sense for extensions
		// that take care of the background themseves. World is
		// one of those, as it is designed to be the back layer.

		mWorld = new Demo3dWorld();
		addExtension(mWorld);

	}

	bool onUserCreate(bool restarted) override {
		return true;
	}

	bool onUserUpdate(float fElapsedTime) override {

		// Select mode with ALT/CMD
		CameraKeyControlMode_t mode =
				Keyboard::isHeld(Keys::ALT) ? Pix::ADJUST_ANGLES :
				Keyboard::isHeld(Keys::COMMAND) ? Pix::ADJUST_POSITION : Pix::MOVE;

		camera()->inputKey(
				mode,
				Keyboard::isHeld(Keys::UP),
				Keyboard::isHeld(Keys::DOWN),
				Keyboard::isHeld(Keys::LEFT),
				Keyboard::isHeld(Keys::RIGHT),
				fElapsedTime);

		return true;

	}

};


#endif //PIXFU_ANDROID_DEMOS_DEMO3D_H
