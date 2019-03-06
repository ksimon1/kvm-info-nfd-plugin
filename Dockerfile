FROM centos:7

MAINTAINER "Francesco Romani" <fromani@redhat.com>

ENV container docker

RUN yum -y update
RUN mkdir -p /etc/kvm-info-nfd-plugin
COPY conf/kvm-info.json /etc/kvm-info-nfd-plugin
COPY cmd/kvm-info-nfd-plugin/kvm-info-nfd-plugin /usr/bin/
