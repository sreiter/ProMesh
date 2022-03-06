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

or e.g. on Ubuntu simply by calling

    sudo apt install qtbase5-dev

You may then proceed with the compilation of ProMesh, execute e.g. in ProMesh's root directory:

    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Release -DUG_ROOT_PATH=pathToUG4 ..

Where `pathToUG4` should point to the `ug4` path which you created in the first step.
Unless cmake finds your Qt installation, please also specify the following option, replacing
`pathToYourQtInstallation` with the path where your Qt installation is located:
  
  cmake -DQT_CMAKE_PATH=pathToYourQtInstallation/5.9/gcc_64/lib/cmake .

The important parameters in a nutshell:
- `CMAKE_BUILD_TYPE`: `Release` or `Debug`. Has to match the CMAKE_BUILD_TYPE you chose when compiling ug4's libgrid (see above)
- `UG_ROOT_PATH`: The path to ug4, i.e., to the directory containing the ugcore and lib subdirectories
- `QT_CMAKE_PATH`: Folder containing cmake-modules for the chosen architecture, e.g., "...pathToQt/5.9/gcc_64/lib/cmake""


GENERATION OF UP-TO-DATE DOCUMENTATION
Once ProMesh has been built (e.g. on Windows), please run the following command in your build directory to generate up-to-date documentation

    ProMesh4.exe -docugen

(`doxygen` is required in your commandline environment for this to work)

This will update the scripting documentation in ProMesh's *docs* folder. ProMesh has to be linked again to reflect the changes (e.g., by calling `make` again).
