# Blackjack

Developed with Unreal Engine 4

In a usuarl UE4 project, many things (specially in the prototype phase, and I consider this a prototype) are done in blueprints, but since this was a programming test I spent some time moving that code into C++. Most of the important code is inside ABlackjackGameMode, in UE4 the GameMode class is the one that controls the flow of a game/match. Even with that, there are some parts of the code, mostly the ones that send events to the UI (that is also a blueprint) that are inside a game mode blueprint, which is more or less in Unity's language, a prefab that inherits from the C++ ABlackjackGameMode class. You can save your current game (money that you have up until that point) after a game/round ends.

ABlackjackCard is the class that represents a card with two sprites. I wrote the code so that many players could play locally but the UI right now only supports one player because programatically setting up the UI to acommodate multiple players was gonna take me maybe 2 or 3 hours more.

There are some things that can be improved for example, right now there is the option to quit or save and quit, but for example if you have a previous save, continue with that and lose some games you can quit without saving, that can be exploited of course but just wanted to have the options.

A Win64 executable is inside the Build folder