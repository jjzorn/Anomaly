# Anomaly

Anomaly is a simple game engine for effortlessly writing small multiplayer 2D games. It is written
in C++, but games are programmed using the Lua scripting language.

## Architecture

A game is always run by a game server (the AnomalyServer executable). All clients connect to the
game server and send all their player input to it. The server then executes the Lua scripts, which
decide what exactly should happen, and sends back to the client which sprites they should render or
which sounds to play. All this happens completely automatically, without the game developer needing
to care at all about how the network connection works. Even the images, audio files and fonts are
downloaded by the client automatically, and if they or the game scripts are changed, the game can
be reloaded (typically by pressing F5 on a client window) without removing the players from the
game or changing any state.

## Developing a game

For a description on how to design games using the Anomaly engine, take a look at
[Events](Docs/Events.md), [Functions](Docs/Functions.md), and [Content](Docs/Content.md).
