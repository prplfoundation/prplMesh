FROM python:3.8-slim-buster

RUN apt-get update \
    && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
    apt-transport-https \
    ca-certificates curl \
    curl \
    gcc \
    git \
    gnupg \
    gnupg-agent \
    libsnmp-dev \
    netcat \
    software-properties-common \
    wireshark-common \
    && rm -rf /var/lib/apt/lists/*

COPY requirements.txt /app/requirements.txt

WORKDIR app
RUN pip3 install -r requirements.txt

# TODO: what needs this?
#RUN pip3 install jsonschema distro


RUN curl -fsSL https://download.docker.com/linux/debian/gpg | apt-key add -
RUN add-apt-repository "deb [arch=amd64] https://download.docker.com/linux/debian $(lsb_release -cs) stable"
RUN apt-get update \
    && apt-get install -y --no-install-recommends \
    docker-ce \
    docker-ce-cli \
    containerd.io \
    && rm -rf /var/lib/apt/lists/*

RUN curl -L "https://github.com/docker/compose/releases/download/1.26.0/docker-compose-$(uname -s)-$(uname -m)" -o /usr/local/bin/docker-compose
RUN chmod 755 /usr/local/bin/docker-compose

RUN git clone https://github.com/mattsm/boardfarm.git \
    && cd boardfarm \
    && git checkout 100521fde1fb67536682cafecc2f91a6e2e8a6f8 \
    && python3 setup.py install
