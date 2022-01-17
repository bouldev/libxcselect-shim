#!/bin/bash
#TESTDIR="$(topsrcdir)/testsuite"
TESTDIR="testsuite"
ARG1=("/"	"/usr"	"/Applications"			"/Applications/Xcode.app"		"/Library/Developer"	"/Library/Developer"			)
ARG2=("/"	"/"	"/Applications/Xcode.app"	"/Applications/Xcode.app/Contents"	"/Library"		"/Library/Developer/CommandlineTools"	)
RETV=("1"	"0"	"1"				"1"					"0"			"1"					)
COMM="${TESTDIR}/xcselect_developer_dir_matches_path"
PASS=0
FAIL=0
echo "Test $(basename ${COMM}):"
for i in {0..5}; do
	if [[ $(${COMM} ${ARG1[i]} ${ARG2[i]}; echo $?) == ${RETV[i]} ]]; then
		echo "Test $(expr $i + 1) PASS"
		PASS="${PASS} + 1"
	else
		echo "Test $(expr $i + 1) FAIL"
		FAIL="${FAIL} + 1"
	fi
done
echo "$(expr $PASS + $FAIL) in total, $(expr $PASS) PASS, $(expr $FAIL) FAIL"
