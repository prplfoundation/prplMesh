FROM python:3.8-slim-buster
# FROM tiangolo/docker-with-compose

RUN apt-get update \
&& apt-get install gcc libsnmp-dev -y \
&& apt-get clean

COPY requirements.txt /app/requirements.txt

WORKDIR app
RUN pip3 install -r requirements.txt

RUN apt-get install apt-transport-https ca-certificates curl gnupg-agent software-properties-common -y

RUN apt-get install curl -y

RUN curl -fsSL https://download.docker.com/linux/debian/gpg | apt-key add -
RUN add-apt-repository "deb [arch=amd64] https://download.docker.com/linux/debian $(lsb_release -cs) stable"
RUN apt-get update \
&& apt-get install docker-ce docker-ce-cli containerd.io -y
RUN curl -L "https://github.com/docker/compose/releases/download/1.26.0/docker-compose-$(uname -s)-$(uname -m)" -o /usr/local/bin/docker-compose
RUN chmod 755 /usr/local/bin/docker-compose

RUN DEBIAN_FRONTEND=noninteractive apt-get install wireshark-common -y
# VOLUME "/home/pablo/assia/prpl/git"
# ENTRYPOINT ["/usr/local/bin/entrypoint.sh"]
RUN git clone https://github.com/mattsm/boardfarm.git \
    && cd boardfarm \
    && git checkout 100521fde1fb67536682cafecc2f91a6e2e8a6f8 \
    && python3 setup.py install
