#!/bin/bash
if [ ! -x cmd/kvm-info-nfd-plugin/kvm-info-nfd-plugin ]; then
	echo "*ERR: build the plugin first"
	exit 1
fi

for testcase in $( ls -d tests/??? ); do
	result=$(./cmd/kvm-info-nfd-plugin/kvm-info-nfd-plugin $testcase/kvm-info.json 2> /dev/null)
	expected=$(cat $testcase/_stdout)
	if [[ "$result" != "$expected" ]]; then
		echo "FAIL: $testcase: expected='''$expected''' got='''$result'''"
	else
		echo " OK : $testcase"
	fi
done
