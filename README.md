
# Space Shooter Game
> :warning: ***Project is incomplete and is UNDERDEVELOPMENT***


## Requirements
- [XMAKE](https://xmake.io/)
- CLANG compiler

## Installation

* XMAKE:
    > Bash Script, Works for general: `$ bash <(curl -fsSL https://xmake.io/shget.text)`
    Other(Individual):
    - [Windows](https://xmake.io/#/guide/installation?id=windows)
    - [Ubuntu](https://xmake.io/#/guide/installation?id=ubuntu)
    - [MacOS](https://xmake.io/#/guide/installation?id=macos)
    - [Others](https://xmake.io/#/guide/installation)


## Building and running application

### Cloning repo:
```$ git clone https://github.com/sarthak-pokharel/space-shooter-sdl.git```

### Switch to project directory
```$ cd space-shooter-sdl```

### Installing libsdl on system
`
git clone https://github.com/libsdl-org/SDL
cd SDL
mkdir build
cd build
../configure
make
sudo make install

`

### Install/configure libsdl on xmake
- : ```$ xrepo install libsdl```
- ```$ xrepo install libsdl_image```

### Build and run
```$ ./run```