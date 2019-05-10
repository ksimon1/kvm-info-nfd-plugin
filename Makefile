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

unittests: capsinfo verinfo
	cd tests/caps-info && ./test-runner.sh
	cd tests/version-info && ./test-runner.sh

tests: unittests

.PHONY: all container vendor binary clean unittests tests

