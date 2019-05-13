FROM quay.io/fromani/centos-builder:7 as builder
WORKDIR /go/src/github.com/fromanirh/kvm-info-nfd-plugin
ENV GOPATH=/go
COPY . .
RUN make plugins
RUN cp cmd/kvm-caps-info-nfd-plugin/kvm-caps-info-nfd-plugin /
RUN cp cmd/kvm-version-info-nfd-plugin/kvm-version-info-nfd-plugin /

FROM registry.access.redhat.com/ubi8/ubi-minimal
MAINTAINER "Francesco Romani" <fromani@redhat.com>
ENV container docker

RUN mkdir -p /etc/kvm-version-info-nfd-plugin
COPY conf/kvm-version-info.json /etc/kvm-info-nfd-plugin
COPY --from=builder /kvm-version-info-nfd-plugin /usr/bin/
COPY --from=builder /kvm-caps-info-nfd-plugin /usr/bin/
