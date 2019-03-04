FROM centos:7

MAINTAINER "Francesco Romani" <fromani@redhat.com>

ENV container docker

COPY cmd/kvm-info-nfd-plugin/kvm-info-nfd-plugin /usr/bin/
