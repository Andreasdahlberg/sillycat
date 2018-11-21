# Docker

Docker is used to get a consistent development environment when using a CI-service
such as [Travis CI](https://travis-ci.org/).

## Build image
```bash
$ cd docker
$ docker build -t andreasdahlberg/docker-avr .
```

## Push to repository
```bash
$ docker push andreasdahlberg/docker-avr
```
