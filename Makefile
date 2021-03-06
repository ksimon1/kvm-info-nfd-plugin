all: binaries

binaries: verinfo capsinfo

container: binaries
	buildah bud .

vendor:
	dep ensure

verinfo:
	./hack/build/build-verinfo.sh
	./hack/build/build-verinfo-test.sh

capsinfo:
	./hack/build/build-capsinfo-lib.sh
	./hack/build/build-capsinfo.sh
	./hack/build/build-capsinfo-test.sh

clean:
	rm -f cmd/kvm-version-info-nfd-plugin/kvm-version-info-nfd-plugin
	rm -f cmd/kvm-caps-info-nfd-plugin/kvm-caps-info-nfd-plugin
	rm -f lib/*.o
	rm -f lib/*.a

unittests: plugins
	cd tests/caps-info && ./test-runner.sh
	cd tests/version-info && ./test-runner.sh

plugins: capsinfo verinfo

tests: unittests

release: plugins
	mkdir -p _out
	cp cmd/kvm-version-info-nfd-plugin/kvm-version-info-nfd-plugin _out/kvm-version-info-nfd-plugin-${VERSION}-linux-amd64
	cp cmd/kvm-caps-info-nfd-plugin/kvm-caps-info-nfd-plugin _out/kvm-caps-info-nfd-plugin-${VERSION}-linux-amd64
	hack/container/docker-push.sh ${VERSION}

check-fmt: check-whitespaces

check-whitespaces:
	./hack/check-whitespaces.sh

.PHONY: all vendor binaries clean unittests tests plugins release release-plugins release-clean check-fmt check-whitespaces
