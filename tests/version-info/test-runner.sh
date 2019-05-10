#!/bin/bash
RET=0
for testcase in $( ls -d ??? ); do
	if [ -f "${testcase}/_name" ]; then
		name=$(cat ${testcase}/_name)
	else
		name="testcase#${testcase}"
	fi

	expected=$(cat ${testcase}/_stdout)
	if [ -r $testcase/kvm-version-info.json ]; then
		obtained=$(./kvm-version-info-test ${testcase}/kvm-version-info.json 2> /dev/null)
	else
		obtained=$(./kvm-version-info-test 2> /dev/null)
	fi
	result="???"
	if [ "${obtained}" != "${expected}" ]; then
		result="FAIL"
		RET=1
	else
		result="OK"
	fi
	printf "* %-32s: %s\n" ${name} ${result}
done
exit $RET
