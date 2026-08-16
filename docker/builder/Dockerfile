# base image
FROM ubuntu:24.04

# Prevent interactive prompts during apt-get
ENV DEBIAN_FRONTEND=noninteractive

# Copy package installation scripts
COPY .build-commons/docker/install-deps.sh /tmp
COPY docker/builder/* /tmp

# Install the Build Commons dependencies
RUN /tmp/install-deps.sh && rm /tmp/install-deps.sh

# Create the pulsar user, group and /pulsar directory
ARG UID=1313
ARG GID=1313
ARG UNAME=pulsar

RUN groupadd -f -g $GID $UNAME
RUN useradd -m -u $UID -g $GID -s /bin/bash $UNAME
RUN mkdir -p /pulsar

# Configure the environment
ENV PGPLOT_DIR="/usr/lib/pgplot5"
ENV PGPLOT_FONT="${PGPLOT_DIR}/grfont.dat"
ENV TEMPO2="/pulsar/tempo2"
ENV PSRCAT_FILE="/pulsar/psrcat/psrcat.db"
ENV PATH="/pulsar/tempo2/bin:/pulsar/bin:${PATH}"
ENV LD_LIBRARY_PATH="/pulsar/tempo2/lib:/pulsar/lib"
ENV PYTHONPATH="/pulsar/lib/python3.12/site-packages"
ENV PKG_CONFIG_PATH="/pulsar/lib/pkgconfig"

# Install the package-specific dependencies
RUN cd /tmp && ./install_dependencies.sh

RUN rm -rf /tmp/*
RUN chown -R $UID:$GID /pulsar

