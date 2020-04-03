FROM docker:18.09.7-dind

RUN apk add --update --no-cache \
    bash \
    python3 \
    grep \
    coreutils \
    tcpdump

COPY entrypoint.sh /usr/local/bin/

ENTRYPOINT ["entrypoint.sh"]
