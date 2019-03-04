all: binary

container: binary
	buildah bud .

vendor:
	dep ensure

binary:
	./hack/build/build.sh

clean:
	rm -f cmd/kvm-info-nfd-plugin/kvm-info-nfd-plugin

.PHONY: all container vendor binary clean

