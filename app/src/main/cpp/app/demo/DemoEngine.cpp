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

bool DemoEngine::OnUserCreate() {
	Clear(olc::BLUE);
	return true;
}

bool DemoEngine::OnUserUpdate(float fElapsedTime) {
	DrawString(100,100,"HELLO OLC PGE!", olc::GREEN, 4.0);
	olc::Sprite *s = new olc::Sprite("android.png");				// root of assets folder, subdirs are also ok
	SetPixelMode(olc::Pixel::ALPHA);
	DrawSprite(160,200,s, 1);

	return true;
}
