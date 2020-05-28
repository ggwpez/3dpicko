# set the base image to Ubuntu 14.04
FROM ubuntu:20.04

MAINTAINER oliver.tale@web.de

RUN apt-get update && apt-get install -y sudo
RUN useradd -ms /bin/bash docker
RUN su docker

# Install tzdata so it does not annoy us later
ENV TZ=Europe/Berlin
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone
RUN apt-get update && apt-get upgrade -y && sudo apt-get install tzdata git -y

ENV CPICKO_ENV=docker
RUN git clone https://gitlab.com/ggwpez/3cpicko.git
WORKDIR 3cpicko/
RUN bash dependencies.sh
RUN bash setup.sh ~/3dpicko-installation 098c16f6349b3c34ef848fc39c27192b802bf9a0

RUN apt-get autoremove

ENV LD_LIBRARY_PATH=/root/3dpicko-installation/
CMD [ "/root/3dpicko-installation/main", "/root/3dpicko-installation/serverconfig.ini" ]
EXPOSE 8080
EXPOSE 8888
