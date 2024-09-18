# Dungen

This is a library for the Irrlicht-3D-Engine for generating procedural 3D-dungeons.

## Folders

- Folder 'bin' contains the executables and the DunGen.lib file.
- Folder 'documentation' contains the doxygen documentation.
- Folder 'DunGen' contains the code and Visual Studio 2010 project for DunGen.
- Folder 'RandomCaves' contains the code and Visual Studio 2010 project for the tool to create caves from random L-systems.
- Folder 'TutorialXY' contains the code and Visual Studio 2010 project for the DunGen-Tutorial XY.

Note: You probably have to adapt the Irrlicht include and library folder within the projects, if you want to compile them by yourself.

## How-to-use

To use DunGen, you have to:
- to add the 'DunGen.lib' file, so add 'bin' to you library folders
- to add the 'DunGen\interface' folder to your include folders
- have the 'DunGen.dll' in the folder where your project executable is running

## Notes

This is compiled and compatible with the Irrlicht version 1.8.1 .
https://irrlicht.sourceforge.io/

## Pictures

Some pictures of generated dungeons:

From inside:
![Picture of generated dungeon from inside](/dungen01.jpg)

From outside:
![Picture of generated dungeon from outside](/dungen02.jpg)
