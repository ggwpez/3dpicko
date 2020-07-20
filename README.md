## Installation

There are three ways of setting it up:  
  - Docker container (recommended)
  - Binary release (Ubuntu 18.04 & 20.04)
  - Compile from source

### Docker
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

### Binary release

### Compile

## Known BUGs
- Auto-updater: Currently disabled since it is not used anyway.
- Round plates: The image detection for round source plates does not work.

## Updating
### Docker
```
docker pull ggwpez/3dpicko:dev
```