# UltimaLive
Ultima Online Map Streaming Mod

The purpose of UltimaLive is to allow a shard to edit its maps while the game is running, stream those changes to its players in realtime, and use more than six map files. UltimaLive provides freeshard developers the ability to alter maps without the hassle of redistributing map files to their entire player base. 
Maps can be altered in realtime while the game client is running. This gives freeshard developers the possibility of scripting events that make changes to the game maps while the shard is running. 

[Source Code Doxygen Documentation](https://praxiiz.github.io/UltimaLive)

Project Setup

Visual Studio Projects have been moved out of the UltimaLive Project folder so that a separate folder can be maintained for each version of Visual Studio can be supported.

Doxygen 

Doxygen for windows can be installed to generate project documentation. 
1. Doxygen needs to be installed and added to the system path. 
2. Create a folder called html in the UltimaLive root folder.
3. Issue the following command from the UltimaLive root folder to generate documentation:

doxygen docs/Doxyfile
