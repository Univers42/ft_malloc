#!/bin/sh
# ft_malloc leak-audit lane driver. Invoked by the Makefile leakcheck* targets.
# Usage: run.sh {lane12|asan|valgrind|all}
set -u

LANE="${1:-all}"
HERE=$(cd "$(dirname "$0")" && pwd)
ROOT=$(cd "$HERE/../.." && pwd)
BIN="$ROOT/bin"
PRELOAD="$BIN/libsyscount.so"
HARNESS="$BIN/full_audit"
ASAN="$BIN/full_audit_asan"

GREEN='\033[0;32m'
RED='\033[0;31m'
BOLD='\033[1m'
RST='\033[0m'

overall=0

lane12() {
	printf "%b\n" "${BOLD}### Lane 1+2: syscall interposer + call-site leak map ###${RST}"
	LD_LIBRARY_PATH="$BIN" LD_PRELOAD="$PRELOAD" "$HARNESS"
	rc=$?
	[ $rc -eq 0 ] || overall=1
	return $rc
}

asan() {
	printf "%b\n" "${BOLD}### Lane 3: AddressSanitizer + UBSan ###${RST}"
	ASAN_OPTIONS=abort_on_error=1:detect_leaks=0 \
		UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1 "$ASAN"
	rc=$?
	[ $rc -eq 0 ] || overall=1
	return $rc
}

valgrind_lane() {
	if ! command -v valgrind >/dev/null 2>&1; then
		printf "%b\n" "${BOLD}### Lane 4: Valgrind (not installed; skipped) ###${RST}"
		return 0
	fi
	printf "%b\n" "${BOLD}### Lane 4: Valgrind memcheck (UB / invalid access) ###${RST}"
	LD_LIBRARY_PATH="$BIN" valgrind --error-exitcode=99 \
		--errors-for-leak-kinds=none --leak-check=no -q "$HARNESS"
	rc=$?
	[ $rc -eq 0 ] || overall=1
	return $rc
}

demo() {
	printf "%b\n" "${BOLD}### DEMO: call-site leak map catching a real leak ###${RST}"
	LD_LIBRARY_PATH="$BIN" LD_PRELOAD="$PRELOAD" "$HARNESS" --leak-demo \
		>/tmp/ftmalloc_demo.out 2>&1 || true
	sed -n '/DEMO: intentional leak/,$p' /tmp/ftmalloc_demo.out
}

case "$LANE" in
	lane12) lane12 ;;
	asan) asan ;;
	valgrind) valgrind_lane ;;
	all)
		lane12 || true
		printf "\n"
		asan || true
		printf "\n"
		valgrind_lane || true
		printf "\n"
		demo
		;;
	*)
		echo "unknown lane: $LANE" >&2
		exit 2
		;;
esac

printf "\n%b\n" "${BOLD}=== leak-audit overall: $( [ $overall -eq 0 ] && printf "%bPASS%b" "$GREEN" "$RST" || printf "%bFAIL%b" "$RED" "$RST" ) ===${RST}"
exit $overall
