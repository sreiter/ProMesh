to build ProMesh4 for MacOSX, use the following vars in the CMakeCache.txt
file of the associated lib_grid build:
//Build architectures for OSX
CMAKE_OSX_ARCHITECTURES:STRING=i386

//Minimum OS X version to target for deployment (at runtime); newer
// APIs weak linked. Set to empty string for default value.
CMAKE_OSX_DEPLOYMENT_TARGET:STRING=10.5

//The product will be built against the headers and libraries located
// inside the indicated SDK.
CMAKE_OSX_SYSROOT:PATH=/Developer/SDKs/MacOSX10.5.sdk