#!/bin/sh

#--------------------------------------------------------------------------#

die() {
  cecho "${HIDE}test/cnf/run.sh:${NORMAL} ${BAD}error:${NORMAL} $*"
  exit 1
}

msg() {
  cecho "${HIDE}test/cnf/run.sh:${NORMAL} $*"
}

for dir in . .. ../..; do
  [ -f $dir/scripts/colors.sh ] || continue
  . $dir/scripts/colors.sh || exit 1
  break
done

#--------------------------------------------------------------------------#

[ -d ../test -a -d ../test/cnf ] ||
  die "needs to be called from a top-level sub-directory of CourseSAT"

[ x"$COURSESATBUILD" = x ] && COURSESATBUILD="../build"

[ -x "$COURSESATBUILD/coursesat" ] ||
  die "can not find '$COURSESATBUILD/coursesat' (run 'make' first)"

cecho -n "$HILITE"
cecho "---------------------------------------------------------"
cecho "CNF testing in '$COURSESATBUILD'"
cecho "---------------------------------------------------------"
cecho -n "$NORMAL"

make -C $COURSESATBUILD
res=$?
[ $res = 0 ] || exit $res

#--------------------------------------------------------------------------#

coresolver="$COURSESATBUILD/coursesat"
simpsolver="$COURSESATBUILD/../scripts/run-simplifier-and-extend-solution.sh"
dratchecker=$COURSESATBUILD/drat-trim
lratchecker=$COURSESATBUILD/lrat-trim
solutionchecker=$COURSESATBUILD/dimocheck
makefile=$COURSESATBUILD/makefile

if [ ! -f $solutionchecker -o ! -f $dratchecker -o ! -f $lratchecker ]; then

  if [ ! -f $solutionchecker -o ../test/cnf/dimocheck.c -nt $solutionchecker ]; then
    cmd="cc -O -o $solutionchecker ../test/cnf/dimocheck.c -lz"
    cecho "$cmd"
    if $cmd 2>/dev/null; then
      msg "external solution checking with '$solutionchecker'"
    else
      msg "no external solution checking " \
        "(compiling '../test/cnf/preochk.c' failed)"
      solutionchecker=none
    fi
  fi

  if [ ! -f $dratchecker -o ../test/cnf/drat-trim.c -nt $dratchecker ]; then
    cmd="cc -O -o $dratchecker ../test/cnf/drat-trim.c"
    if $cmd 2>/dev/null; then
      msg "external proof checking with '$dratchecker'"
    else
      msg "no external proof checking " \
        "(compiling '../test/cnf/drat-trim.c' failed)"
      dratchecker=none
    fi
  fi

  if [ ! -f $lratchecker -o ../test/cnf/lrat-trim.c -nt $lratchecker ]; then
    cmd="cc -O -o $lratchecker ../test/cnf/lrat-trim.c"
    if $cmd 2>/dev/null; then
      msg "external proof checking with '$lratchecker'"
    else
      msg "no external proof checking " \
        "(compiling '../test/cnf/lrat-trim.c' failed)"
      lratchecker=none
    fi
  fi
else
  msg "external solution checking with '$solutionchecker'"
  msg "external DRAT checking with '$dratchecker'"
  msg "external LRAT checking with '$lratchecker'"
fi

#--------------------------------------------------------------------------#

ok=0
failed=0

core() {
  msg "running CNF test core ${HILITE}'$1'${NORMAL}"
  prefix=$COURSESATBUILD/test-cnf-core
  cnf=../test/cnf/$1.cnf
  log=$prefix-$1.log
  err=$prefix-$1.err
  chk=$prefix-$1.chk
  prf=$prefix-$1.prf
  proofchecker=$3
  if [ -f cnf/$1.sol ]; then
    solopts=" -r ../test/cnf/$1.sol"
  else
    solopts=""
  fi
  case $proofchecker in
  *drat*)
    proofopts=" $prf"
    expectedcheckerstatus=0
    ;;
  *lrat*)
    proofopts=" --lrat $prf"
    expectedcheckerstatus=20
    ;;
  *) proofopts="" ;;
  esac
  opts="$cnf --check$solopts$proofopts"
  cecho "$coresolver \\"
  cecho "$opts"
  cecho -n "# $2 ..."
  "$coresolver" $opts 1>$log 2>$err
  res=$?
  if [ ! $res = $2 ]; then
    cecho " ${BAD}FAILED${NORMAL} (actual exit code $res)"
    failed=$(expr $failed + 1)
  elif [ $res = 10 ]; then
    if [ "$solopts" = "" ]; then
      cecho " ${GOOD}ok${NORMAL} (without solution file)"
    else
      cecho " ${GOOD}ok${NORMAL} (solution file checked after parsing)"
    fi
    if [ x"$solutionchecker" = xnone ]; then
      ok=$(expr $ok + 1)
    else
      cecho "$solutionchecker \\"
      cecho "$cnf $log"
      cecho -n "# 0 ..."
      if $solutionchecker $cnf $log 1>&2 >$chk; then
        cecho " ${GOOD}ok${NORMAL} (solution checked externally too)"
        ok=$(expr $ok + 1)
      else
        cecho " ${BAD}FAILED${NORMAL} (incorrect solution)"
        failed=$(expr $failed + 1)
      fi
    fi
  elif [ $res = 20 ]; then
    cecho " ${GOOD}ok${NORMAL} (exit code as expected)"
    if [ ! x"$proofchecker" = xnone ]; then
      cecho "$proofchecker \\"
      cecho "$cnf $prf"
      cecho -n "# 0 ..."
      $proofchecker $cnf $prf 1>&2 >$chk
      status=$?
      if [ $status = $expectedcheckerstatus ]; then
        cecho " ${GOOD}ok${NORMAL} (proof checked)"
        ok=$(expr $ok + 1)
      else
        cecho " ${BAD}FAILED${NORMAL} (proof check '$proofchecker $cnf $prf' failed)"
        failed=$(expr $failed + 1)
      fi
    fi
  else
    cecho " ${BAD}FAILED${NORMAL} (unsupported exit code $res)"
    failed=$(expr $failed + 1)
  fi
}

simp() {
  msg "running CNF test simp ${HILITE}'$1'${NORMAL}"
  prefix=$COURSESATBUILD/test-cnf-simp
  cnf=../test/cnf/$1.cnf
  log=$prefix-$1.log
  err=$prefix-$1.err
  chk=$prefix-$1.chk
  opts="$cnf"
  cecho "$simpsolver \\"
  cecho "$opts"
  cecho -n "# $2 ..."
  "$simpsolver" $opts 1>$log 2>$err
  res=$?
  if [ ! $res = $2 ]; then
    cecho " ${BAD}FAILED${NORMAL} (actual exit code $res)"
    failed=$(expr $failed + 1)
  elif [ $res = 10 ]; then
    cecho " ${GOOD}ok${NORMAL}"
    if [ x"$solutionchecker" = xnone ]; then
      ok=$(expr $ok + 1)
    else
      cecho "$solutionchecker \\"
      cecho "$cnf $log"
      cecho -n "# 0 ..."
      if $solutionchecker $cnf $log 1>&2 >$chk; then
        cecho " ${GOOD}ok${NORMAL} (solution checked externally)"
        ok=$(expr $ok + 1)
      else
        cecho " ${BAD}FAILED${NORMAL} (incorrect solution)"
        failed=$(expr $failed + 1)
      fi
    fi
  fi
}

run() {
  core $* none
  core $* $dratchecker
  core $* $lratchecker
  simp $*
}

run case-010 10
run case-013 20

run case-078 10
run case-079 10
run case-080 10
run case-081 10
run case-082 20
run case-083 20
run case-084 20
run case-085 20

run case-077 10

run case-044 20
run case-045 10
run case-050 10
run case-051 10
run case-052 10
run case-053 20
run case-054 10
run case-055 10
run case-056 10
run case-057 10
run case-046 10
run case-047 10
run case-048 10
run case-049 10

run case-014 20
run case-015 20
run case-016 20
run case-017 20
run case-018 20
run case-019 20
run case-020 20

run case-043 10
run case-008 20
run case-009 10

run case-007 10

run case-035 10
run case-041 10
run case-032 10
run case-037 10
run case-026 10
run case-029 10
run case-034 10
run case-033 10
run case-038 10
run case-040 10
run case-042 10
run case-027 10
run case-028 10
run case-030 10
run case-031 10

run case-011 10
run case-012 10

run case-066 10
run case-067 10
run case-068 10
run case-069 10
run case-070 10
run case-071 10
run case-072 10
run case-074 10
run case-075 10
run case-076 10
run case-058 10
run case-060 10
run case-061 10
run case-062 10
run case-063 10
run case-064 10
run case-073 10
run case-065 10
run case-059 10

run case-021 20
run case-022 20
run case-023 20
run case-024 20
run case-025 20

run case-004 20
run case-006 20
run case-002 20
run case-003 20
run case-005 20
run case-001 20

run case-039 20

#--------------------------------------------------------------------------#

[ $ok -gt 0 ] && OK="$GOOD"
[ $failed -gt 0 ] && FAILED="$BAD"

msg "${HILITE}CNF testing results:${NORMAL} ${OK}$ok ok${NORMAL}, ${FAILED}$failed failed${NORMAL}"

exit $failed
