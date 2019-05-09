#!/bin/bash
if [ ! -x cmd/kvm-version-info-nfd-plugin/kvm-version-info-nfd-plugin ]; then
	echo "*ERR: build the plugin first"
	exit 1
fi

for testcase in $( ls -d tests/version-info/??? ); do
	expected=$(cat $testcase/_stdout)
	if [ -r $testcase/kvm-version-info.json ]; then
		result=$(./cmd/kvm-version-info-nfd-plugin/kvm-version-info-nfd-plugin $testcase/kvm-version-info.json 2> /dev/null)
	else
		result=$(./cmd/kvm-version-info-nfd-plugin/kvm-version-info-nfd-plugin 2> /dev/null)
	fi
	if [[ "$result" != "$expected" ]]; then
		echo "FAIL: $testcase: expected='''$expected''' got='''$result'''"
	else
		echo " OK : $testcase"
	fi
done
