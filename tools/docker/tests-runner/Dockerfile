# docker:18.09.7-dind on Thu  7 May 07:38:06 2020
FROM docker.io/library/docker@sha256:a490c83561c1cef49b6fe12aba2c31f908391ec3efe4eb173225809c981e50c3

RUN apk add --update --no-cache \
    bash \
    python3 \
    grep \
    coreutils \
    tshark

COPY entrypoint.sh /usr/local/bin/

ENTRYPOINT ["entrypoint.sh"]
