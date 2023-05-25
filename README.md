# Growtopia enet proxy by MegaMini

## Don't add me in discord for help with compiling/adding feature/some simple thing with proxy or you will get instantly blocked

### Anyone using or selling "fixed proxy" is a human failure
### If you like don't forget to star the repoistory

# By [MegaMini](https://github.com/MegaMini)

## Compile and build in Visual Studio

- Install Vcpkg openssl step by step;
```
> git clone https://github.com/microsoft/vcpkg
> .\vcpkg\bootstrap-vcpkg.bat
> vcpkg install openssl:x64-windows
```
- Reopen Vs,Done.

## How to use

- Does not need separate http server
- Use HxD or similar to edit "growtopia1.com" and "growtopia2.com" to "localhost" In growtopia binary.
- Alternative: Use hosts file to set growtopia1 and growtopia2 to point to 127.0.0.1 if you want to
- Start proxy -> Run localhost patched file (or normal with hosts) -> Works as supposed
- Build with `Release x64 or x86`
- You need to start proxy.exe with administrator for writen hosts file

## Features

- Print all variantlists and function calls
- Print all text packets
- Supports modifying, ignoring, and making new packets
- Kill players with wrench
- More commands which you can find in the changelog
- Has a PoC /name name command to call OnNameChanged function for local client.
- Has a PoC OnSpawn modifier which appends netid to each players' name, and gives you unlim zoom
- Can both intercept outgoing and incoming packets
- Integrated http server
- Ignore tracking packets and crash log requests
- Ignore autoban packets (NOTE: you can still get autobanned if the check is serversided)
- Works with subserver switching
- Auto Tax, Fast Wheel, Show X, Y Position, Exit World When Mod Joins, Save World, Fast Trash, Fast Drop, Wrench Modes, Warp and etc.

## TODO

- Auto BGL
- Dice Speed
- Config System
- No Shadow Ban
- Auto Page
- Auto Solve Capctha
- Pathfinder
- /tall (Trade All)
- /dropwl [amount]
- /dropdl [amount]
- /dropbgl [amount]

## Changelog

# 1.3

- Add AAP Bypass with no spoof info needed

# 1.2.4

- Major refactor of the whole code
- Add world/player classes, and some helpers for easier adding of new features
- Update version to 4.26

# 1.2

- Add /name {name} - Changes your name to {name} (only visual!!)
- Add /flag {id} - Changes your flag to {id} (only visual!!)
- Add /fd - Enable/Disable Fast Drop
- Add /ft - Enable/Disable Fast Trash
- Add /wrench - Enable/Disable Wrench Mode
- Add /wset {pull, kick, ban} - Sets the wrench mode (default: pull)
- Add /cdrop {count} - Drops {count} wls to ground (if you write 100 drops 1 dl)
- Add /warp {world} - Warps to {world}
- Add /save - Goes to your setted save world
- Add /pall - Pull all players in the world
- Add /kall - Kick all players in the world
- Add /ball - Ban all players in the world
- Add /skin {id} - Changes you skin to {id} skin (only visual!!)
- Add /pwrench {player} - Wrenchs to {player}
- Add /csn - For shows casino commands page, you can set fast wheel in this page and etc.
- Add /settings - For showing options page, you can set save world in this page and etc.
- Add /proxy - For showing proxy commands page
- Major refactor of the whole code
- Add world/player classes, and some helpers for easier adding of new features

# 1.1

- Subserver switching should work, apparently there was no problem in the first place as the implementation worked already
- Edit ping requests always to be non offensive behavior
- Ignore autoban packets sent by client
- Ignore tracking packets
- Ignore crash logs that would be sent to gt
- Clean code
- Gives unlimited zoom

### Video: Soon

![x](https://i.imgur.com/Yq23iCs.png "Proxy pic 3")
![x](https://i.imgur.com/QPphLzg.png "Proxy pic 2")
![x](https://i.imgur.com/wKps0tG.png "Proxy pic 1")
