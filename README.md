## Install
The following steps were tested on a fresh Ubunt 18.04 installation.  
You need access to the repository and a [SSH key](https://docs.gitlab.com/ee/ssh/) so that the script can clone the repository.  
You can test your SSH setup with ```ssh -T git@gitlab.com```.

Steps:  
```
sudo apt update && sudo apt install git
git clone git@gitlab.com:ggwpez/3cpicko.git && cd 3cpicko
bash dependencies.sh
bash setup.sh ~/3dpicko-installation 0f9a6cc664ff0a23d4ff7e2a7d6c02f6c068211a 
```
That long hex number is the version that should be installed.  
The compilation of opencv could take a while.  
If everything completes, 3DpickO will be installed to ~/3dpicko-installation.  
Start it with the <b>start.sh</b> script in that folder, a service will not be created yet, so it wont restart on reboot. (todo)  
  
## Usage
Go to the installation directory ~/3dpicko-installation and execute ```./start.sh```.   
You can now navigate to 127.0.0.1:8080 in you'r browser to access the GUI :tada:.