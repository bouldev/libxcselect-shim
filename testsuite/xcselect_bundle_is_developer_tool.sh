#!/bin/bash
#TESTDIR="$(topsrcdir)/testsuite"
TESTDIR="testsuite"
ARG1=(dt.Xcode dt.SourceEdit iphonesimulator dt.Instruments FileMerge itunes.connect.ApplicationLoader AccessibilityInspector RealityComposer)
RETV=("1" "1" "1" "1" "1" "1" "1" "1")
COMM="${TESTDIR}/xcselect_bundle_is_developer_tool"
PASS=0
FAIL=0
echo "Test $(basename ${COMM}):"
for i in {0..7}; do
	if [[ $(${COMM} com.apple.${ARG1[i]}; echo $?) == ${RETV[i]} ]]; then
		echo "Test $(expr $i + 1) PASS"
		PASS="${PASS} + 1"
	else
		echo "Test $(expr $i + 1) FAIL"
		FAIL="${FAIL} + 1"
	fi
done
echo "$(expr $PASS + $FAIL) in total, $(expr $PASS) PASS, $(expr $FAIL) FAIL"
