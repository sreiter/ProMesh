Before you can compile promesh, you have to compile and execute docugen.
to do this create a folder in which you execute cmake on the docugen path
(e.g. mkdir docubuild && cd docubuild && cmake ../docugen).
After you specified the required paths, start compilation with 'make' and
execute docugen afterwards. This will create and populate the docs path in
ProMesh's root path and create a resource-file containing all the help-files.

You may then proceed with the compilation of ProMesh.

Qt5 for Linux:
http://download.qt-project.org/official_releases/online_installers/qt-opensource-linux-x64-online.run


to build ProMesh4 for MacOSX, use the following vars in the CMakeCache.txt
file of the associated lib_grid build:
//Build architectures for OSX
CMAKE_OSX_ARCHITECTURES:STRING=i386		# x86_64 would be an alternative option


//Minimum OS X version to target for deployment (at runtime); newer
// APIs weak linked. Set to empty string for default value.
CMAKE_OSX_DEPLOYMENT_TARGET:STRING=10.5

//The product will be built against the headers and libraries located
// inside the indicated SDK.
CMAKE_OSX_SYSROOT:PATH=/Developer/SDKs/MacOSX10.5.sdk
