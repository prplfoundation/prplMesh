FROM docker:18.09.7-dind

RUN apk add --update --no-cache \
    python3 \
    grep \
    coreutils

COPY entrypoint.sh /usr/local/bin/

ENTRYPOINT ["entrypoint.sh"]
