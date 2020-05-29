## Install
The following steps were tested on a fresh Ubunt 18.04 installation.  
You need access to the repository and a [SSH key](https://docs.gitlab.com/ee/ssh/) so that the script can clone the repository.  

Steps:  
```
sudo apt update && sudo apt install git
git clone https://gitlab.com/ggwpez/3cpicko.git && cd 3cpicko
bash dependencies.sh
bash setup.sh ~/3dpicko-installation 098c16f6349b3c34ef848fc39c27192b802bf9a0 
```
That long hex number is the version that should be installed.  
The compilation of opencv could take a while.  
If everything completes, 3DpickO will be installed to ~/3dpicko-installation.  
A service will be created and started.
You can now navigate to 127.0.0.1:8080 in you'r browser to access the GUI :tada:.  

## Docker
You can also use docker do build/run it:  
```
curl https://gitlab.com/ggwpez/3cpicko/-/raw/dev/Dockerfile | docker build --tag dpicko:0.1 -
docker run dpicko:0.1
```

## Updating
_If_ everything works correctly, a click to the <i>Update</i> button in the Admin GUI should be enought.  
On the next cold-restart it should use the new version. (Dont try the <i>Restart</i> button in the GUI, it does a warm-restart).  
### Auto updater
It does the same thing as a click to the _Update_ button and can be configured in the _serverconfig.ini_ section _updater_.