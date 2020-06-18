FROM python:3.8-slim-buster

RUN apt-get update \
&& apt-get install gcc libsnmp-dev -y \
&& apt-get clean

COPY requirements.txt /app/requirements.txt

WORKDIR app
RUN pip3 install -r requirements.txt

RUN apt-get install apt-transport-https ca-certificates curl gnupg-agent software-properties-common -y

# VOLUME "/home/pablo/assia/prpl/git"
# ENTRYPOINT ["/usr/local/bin/entrypoint.sh"]
