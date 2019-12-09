# AirlineTycoon

This repository aims to complete the partial source code that is provided as a free bonus in the GOG release
of Airline Tycoon Deluxe. To run it you'll need the game assets from the either the First Class, Evolution or
Deluxe edition of the game. You can purchase these assets from GOG.com: https://www.gog.com/game/airline_tycoon_deluxe

## License

The code in the repository is licensed under the terms included in the GOG release. As such the code can
only be used for non-commercial purposes and remains property of BFG.

It is therefore *not* open-source in the free software sense, for more information refer to the License.txt.

## What works?

The singleplayer game is fully playable without issues. Network multiplayer support has been reimplemented using
ENet instead of DirectPlay and might behave differently than the original.

## Building

This repository can be built using Visual Studio 2019 Community. Before building remember to clone the submodules:

```
git submodule update --init
```

More build instructions will follow soon.
