FROM python:3.8-slim-buster

# Debian dependencies to install docker and docker compose from docker.com
RUN apt-get update \
&& DEBIAN_FRONTEND=noninteractive apt-get install \
apt-transport-https ca-certificates curl gnupg-agent \
software-properties-common curl gcc libsnmp-dev wireshark-common tshark \
-y && apt-get clean

# Install docker and docker-compose from docker.com repositories
RUN curl -fsSL https://download.docker.com/linux/debian/gpg | apt-key add -
RUN add-apt-repository "deb [arch=amd64] https://download.docker.com/linux/debian $(lsb_release -cs) stable"
RUN apt-get update \
&& apt-get install docker-ce docker-ce-cli containerd.io -y && apt-get clean
RUN curl -L "https://github.com/docker/compose/releases/download/1.26.0/docker-compose-$(uname -s)-$(uname -m)" -o /usr/local/bin/docker-compose
RUN chmod 755 /usr/local/bin/docker-compose

# Installation of boardfarm
RUN git clone https://github.com/mattsm/boardfarm.git \
    && cd boardfarm \
    && git checkout 100521fde1fb67536682cafecc2f91a6e2e8a6f8 \
    && python3 setup.py install

# Installation of the python packages needed by the tests
COPY requirements.txt /app/requirements.txt
WORKDIR app
RUN pip3 install -r requirements.txt

# Installation of Debian dependencies needed by the tests
# RUN DEBIAN_FRONTEND=noninteractive apt-get update && apt-get install wireshark-common -y && apt-get clean
