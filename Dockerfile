FROM centos:7

MAINTAINER "Francesco Romani" <fromani@redhat.com>

ENV container docker

RUN yum -y update

RUN mkdir -p /etc/kvm-version-info-nfd-plugin
COPY conf/kvm-version-info.json /etc/kvm-info-nfd-plugin
COPY cmd/kvm-version-info-nfd-plugin/kvm-version-info-nfd-plugin /usr/bin/

COPY cmd/kvm-caps-info-nfd-plugin/kvm-caps-info-nfd-plugin /usr/bin/
