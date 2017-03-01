# Platformer

2D puzzle platformer game prototype with 3 working tutorial-type levels.

The object of the game is to reach an exit on each level (on a given screen), solving logic problems along the way.
The only object currently in the game is a box, which creates a vertical field with a "gravitational pull" that the player can use.
The boxes can be pushed by the player and it is necessary in order to complete the levels.

The game is written in C++ using Visual Studio 2015 and SFML 2.4.2 as the multimedia library.
I built SFML from source and statically linked it with VS runtime libraries, so the game doesn't require any library support except for the OpenAL32.dll (for licence reasons I believe), which is included.
