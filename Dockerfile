# set the base image to Ubuntu 14.04
FROM ubuntu:20.04 AS build

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
RUN bash setup.sh /src 098c16f6349b3c34ef848fc39c27192b802bf9a0
RUN apt-get autoremove

# Copy all needed dynamic libraries so we dont have to install them.
# Static linking would be preferable obviously, but Qt static libs need
# need to be build manually.
WORKDIR /src
RUN mkdir deps
RUN ldd main | tr -s '[:blank:]' '\n' | grep '^/' | xargs -I % sh -c 'cp % deps/$(basename "%");'
# Remove some build files
RUN find builds/ -name '*.o' -delete

FROM ubuntu:20.04
COPY --from=build /src/ /src/
ENV LD_LIBRARY_PATH=/src/:/src/deps/
CMD [ "/src/main", "/src/serverconfig.ini" ]
EXPOSE 8080
EXPOSE 8888
