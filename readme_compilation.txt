GENERATION OF UP-TO-DATE DOCUMENTATION
Before compiling promesh, you may want to compile and execute docugen. This will
generate up-to-date documentation. If you omit this step, a slightly outdated
documentation may be used.
In order to build docugen please create a folder in which you execute cmake on
the docugen path (e.g. mkdir docubuild && cd docubuild && cmake ../docugen).
After you specified the required paths, start compilation with 'make' and
execute docugen afterwards. This will create and populate the docs path in
ProMesh's root path and create a resource-file containing all the help-files.
Note that docugen requires that 'doxygen' is executable from the command line.


COMPILATION OF PROMESH
ProMesh requires UG4, the UG4-ProMesh plugin and the UG4-LuaShell plugin.
Please install UG4 through its package manager [ughub](https://github/UG4/ughub), e.g. like this:

    mkdir ug4
    cd ug4
    ughub init
    ughub install ProMesh LuaShell
    mkdir build
    cd build
    cmake -DTARGET=libgrid -DSTATIC_BUILD=ON -DCMAKE_BUILD_TYPE=Release -DProMesh=ON -DLuaShell=ON -DPARALLEL=OFF ..

ProMesh and UG4 support [Tetgen](http://tetgen.org/). After downloading and (locally) installing tetgen you
should enable Tetgen support for UG4. Execute the following command in ug4's build-folder:

    cmake -DTETGEN=.../PathToTetgen ..

Please have a look at Tetgen's licensing.


Please install Qt5, e.g. for Linux from:
    http://download.qt-project.org/official_releases/online_installers/qt-opensource-linux-x64-online.run


You may then proceed with the compilation of ProMesh, execute e.g. in ProMesh's root directory:
    mkdir build && cd build && cmake ..

Please specify all the required paths. If you experience any problems please
have a look at ProMesh/CMakeLists.txt and parse the comments.


If you'd like to deploy ProMesh4 to older MacOSX systems, you may want to adjust the
following vars in the build/CMakeCache.txt file of the associated lib_grid and ProMesh build (also Tetgen, if used)
Please first check which SDKs are actually installed.
	//Build architectures for OSX
	CMAKE_OSX_ARCHITECTURES:STRING=i386		// x86_64 would be an alternative option


	//Minimum OS X version to target for deployment (at runtime); newer
	// APIs weak linked. Set to empty string for default value.
	CMAKE_OSX_DEPLOYMENT_TARGET:STRING=10.5

	//The product will be built against the headers and libraries located
	// inside the indicated SDK.
	CMAKE_OSX_SYSROOT:PATH=/Developer/SDKs/MacOSX10.5.sdk
