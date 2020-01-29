// #include "olcPixelGameEngine.h"

#include "demo/DemoEngine.h"
#include "videos/OneLoneCoder_PGE_Balls2.h"
#include "videos/OneLoneCoder_PGE_PathFinding_WaveProp.cpp"
#include "videos/OneLoneCoder_PGE_PolygonCollisions1.cpp"
#include "olcPixelGameEngine.h"


// trick to make this code executed as Android loads the library, before anything
// else. We inject in the static field ROOTINSTANCE of the olcPixelEngine shared library
// our instantiated PixelGameEngine class.

// Thanks to this dirty trick, the JNI launcher, and the OLC  libs,
// are independent shared libraries that have their own CMAKE, so it is really
// easy to link them into new projects, package, distribute, etc ... vs. manually
// having to craft a big CMAKE everytime.

struct OneLoneDemo {
	OneLoneDemo() {
		olc::PixelGameEngine::BOOTINSTANCE = new DemoEngine();
//		olc::PixelGameEngine::BOOTINSTANCE = new DemoControls();
//		olc::PixelGameEngine::BOOTINSTANCE = new DemoGyro();
//		olc::PixelGameEngine::BOOTINSTANCE = new CirclePhysics();
//		olc::PixelGameEngine::BOOTINSTANCE = new PolygonCollisions();
//		olc::PixelGameEngine::BOOTINSTANCE = new PathFinding_FlowFields();
	}

};

OneLoneDemo rules;

