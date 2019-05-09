all: binaries

binaries: verinfo capsinfo

container: binaries
	buildah bud .

vendor:
	dep ensure

verinfo:
	./hack/build/build-verinfo.sh

capsinfo:
	./hack/build/build-libcapsinfo.sh
	./hack/build/build-capsinfo.sh

clean:
	rm -f cmd/kvm-version-info-nfd-plugin/kvm-version-info-nfd-plugin
	rm -f cmd/kvm-caps-info-nfd-plugin/kvm-caps-info-nfd-plugin

.PHONY: all container vendor binary clean

