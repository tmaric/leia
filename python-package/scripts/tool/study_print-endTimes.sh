#!/usr/bin/env -S bash -u

# Script lists endTimes of all cases inside study directories which match the provided globbing pattern.

if [[ -z $1 ]]; then
  echo "Provide a path to a studydir."
  exit 1
fi

cd "$1" && find -type d -name "*[0-9][0-9][0-9][0-9][0-9]*" -prune | sort | xargs -I[] bash -c "
echo -n \"[] : \" && foamListTimes -latestTime -processor -case [] 2> /dev/null || foamListTimes -latestTime -case []" && echo ""
