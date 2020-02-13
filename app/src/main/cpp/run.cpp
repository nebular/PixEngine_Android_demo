// #include "olcPixelGameEngine.h"

#include "demo/DemoEngine.h"
#include "arch/android/platform_android.hpp"


// trick to make this code executed as Android loads the library, before anything
// else. We inject in the static field ROOTINSTANCE of the olcPixelEngine shared library
// our instantiated PixelGameEngine class.

// Thanks to this dirty trick, the JNI launcher, and the OLC  libs,
// are independent shared libraries that have their own CMAKE, so it is really
// easy to link them into new projects, package, distribute, etc ... vs. manually
// having to craft a big CMAKE everytime.

struct PixFuDemos {

	PixFuDemos() {

		rgl::PixFu *engine = new DemoEngine();

//		rgl::PixFu *engine = new Demo1();
//		rgl::PixFu *engine = new DemoGyro2();
//		rgl::PixFu *engine = new DemoGyro();
//		rgl::PixFu *engine = new DemoControls();

		rgl::PixFuPlatform::init(new rgl::PixFuPlatformAndroid(engine));

	}

};

PixFuDemos rules;

