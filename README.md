## Abstract
3DPicko converts 3D printers into colony pickers.

## Installation
There are three ways of setting it up:  
  - Docker container (recommended)
  - Binary release (Ubuntu 18.04 & 20.04)
  - Compile from source

#### Docker
For quick tryout:
```
docker run -v$(pwd)/:/db -v$(pwd)/uploads:/docroot/uploads -v$(pwd)/reports:/docroot/reports --rm -i ggwpez/3dpicko:dev
```
Open *172.17.0.2:8080* in your browser. If you have more than one docker container running the ip is different.  

For server deployment:
```
docker run -v$(pwd)/:/db -v$(pwd)/uploads:/docroot/uploads -v$(pwd)/reports:/docroot/reports --net=host --restart unless-stopped -d --name 3dpicko ggwpez/3dpicko:dev
```
Open *0.0.0.0:8080* in your browser; this openes ports 8080 and 8888.

On the first run, it will create a default database with the preset for the TU Darmstadt laboratory in it.  
The directories *uploads/* and *reports/* will contain a copy of all uploaded images and created reports.

#### Binary release
Available binaries:  
- Ubuntu 18.04/AMD64
- Ubuntu 20.04/AMD64

Links not available yet.

#### Compile from source
(Debian/Ubuntu):
```sh
git clone https://gitlab.com/ggwpez/3cpicko
cd 3cpicko && git submodule init && git submodule update
# Installs qt5-default libqt5websockets5-dev g++ build-essential pkg-config zlib1g-dev
./dependencies.sh
# Installs OpenCV 3.4.10
./opencv.sh
# Pass it the directory where you want to install 3cpicko
./setup.sh installation/
```

## Known BUGs
- Auto-updater: Currently disabled since it is not used anyway.
- Round plates: The image detection for round source plates does not work.

## Updating
#### Docker
```
docker pull ggwpez/3dpicko:dev
```

#### From Source
Once upon a time there was a small *auto-updater* deeply buried in the source code…… it was not seen for a long time and nobody cared about it. It was able to update the software by downloading the new code and compiling it. The configuration options in the *serverconfig.ini* are the only remnants of its former glory.  
__TLDR__: The *auto-updater* does currently not work.

## License
This project is licensed under GPLv3, see [LICENSE](LICENSE).
<br/>
<br/>
`depend/QtWebApp` contains a modified version of `QtWebApp`, for these files applies:
```
  License: LGPLv3
  Author and Copyright owner: Stefan Frings
  stefan@stefanfrings.de
  http://www.stefanfrings.de
```
`depend/quazip` references [QuaZip v0.9.1](https://github.com/stachenov/quazip). Thanks to [Sergey A. Tachenov](https://github.com/stachenov) for the *cute* wrapper.