ARG DEPS
ARG OS
FROM $DEPS AS build

MAINTAINER oliver@tasty.limo

COPY ./depend /3dpicko/source/depend
COPY 3DpickO.pro /3dpicko/source
COPY ./Main /3dpicko/source/Main
COPY ./.git /3dpicko/source/.git
COPY ./.gitmodules /3dpicko/source/.gitmodules
RUN cd /3dpicko/source && git submodule init && git submodule update
WORKDIR /3dpicko/build
RUN qmake -spec linux-clang ../source && \
	make -j$(nproc)
COPY ./check.sh /3dpicko/source
COPY ./Doxyfile /3dpicko/source

# Copy all stuff into a /3dpicko/install directory, to bundle it.
# Includes libraries and binaries.
WORKDIR /3dpicko/install
COPY ./server/ ./server/
# Copy config files and replace template entries.
RUN sed -i "s#%docroot#docroot#g" server/serverconfig.ini \
	&& sed -i "s#%db#/db#g" server/serverconfig.ini
WORKDIR /3dpicko/install/libs
RUN ldd /3dpicko/build/Main/Main | tr -s '[:blank:]' '\n' | grep '^/' | xargs -I % sh -c 'cp % $(basename "%");' \
	&& cp /3dpicko/build/Main/Main /3dpicko/install/Main \
	&& cp /3dpicko/build/depend/quazip/quazip/libquazip.so.1 /3dpicko/install/libs

FROM $DEPS as zip
RUN apt-get -qq update && apt-get install -y -qq zip && rm -rf "/var/lib/apt/lists/*"
COPY --from=build /3dpicko/install/ /3dpicko/install/
WORKDIR /3dpicko/install/
RUN printf "#!/bin/sh\nLD_LIBRARY_PATH=libs/ ./Main server/serverconfig.ini\n" > start.sh && chmod +x start.sh
CMD [ "zip", "-r", "-", "." ]

FROM $OS as image
COPY --from=build /3dpicko/install/ /3dpicko/install/
ENV LD_LIBRARY_PATH=/3dpicko/install/libs/
CMD [ "/3dpicko/install/Main", "/3dpicko/install/server/serverconfig.ini" ]
EXPOSE 8080
EXPOSE 8888