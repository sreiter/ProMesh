COMPILATION OF PROMESH
ProMesh requires UG4, the UG4-ProMesh plugin and the UG4-LuaShell plugin.
Please install UG4 through its package manager [ughub](https://github/UG4/ughub), e.g. like this:

    mkdir ug4
    cd ug4
    ughub init
    ughub install ProMesh LuaShell tetgen
    mkdir build
    cd build
    cmake -DTARGET=libgrid -DSTATIC_BUILD=ON -DCMAKE_BUILD_TYPE=Release -DProMesh=ON -DLuaShell=ON -DPARALLEL=OFF -Dtetgen=ON ..


Installing 'tetgen' (as proposed above) is optional.
Please have a look at Tetgen's licensing (www.tetgen.org).


Please install Qt5, e.g. for Linux from:
    http://download.qt-project.org/official_releases/online_installers/qt-opensource-linux-x64-online.run


You may then proceed with the compilation of ProMesh, execute e.g. in ProMesh's root directory:
    mkdir build && cd build && cmake ..

Please specify all the required paths. If you experience any problems please
have a look at ProMesh/CMakeLists.txt and parse the comments. Required are at least the following options:

    UG_ROOT_PATH: "The path to ug4. Has to contain the ugbase and lib directories."
    QT_CMAKE_PATH: "Folder containing cmake-modules for the chosen architecture."


GENERATION OF UP-TO-DATE DOCUMENTATION
Once ProMesh has been built (e.g. on Windows), please run the following command in your build directory to generate up-to-date documentation

    ProMesh4.exe -docugen

ProMesh has to be linked again with the new documentation. To force linking, please remove the old executable before calling make (or corresponding build tools).


DEPLOY PROMESH ON OSX
If you'd like to deploy ProMesh4 to older MacOSX systems, you may want to adjust the
following vars in the build/CMakeCache.txt file of the associated lib_grid and ProMesh build (also Tetgen, if used)
Please first check which SDKs are actually installed.
    //Build architectures for OSX
    CMAKE_OSX_ARCHITECTURES:STRING=i386     // x86_64 would be an alternative option


    //Minimum OS X version to target for deployment (at runtime); newer
    // APIs weak linked. Set to empty string for default value.
    CMAKE_OSX_DEPLOYMENT_TARGET:STRING=10.5

    //The product will be built against the headers and libraries located
    // inside the indicated SDK.
    CMAKE_OSX_SYSROOT:PATH=/Developer/SDKs/MacOSX10.5.sdk
