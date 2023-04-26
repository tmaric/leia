#!/usr/bin/env -S bash 

function foamLatestTime()
{
  case="$1"
  if [[ -z ${case} ]]; then
    echo "Provide a case!" >&2
    return 1
  elif [[ ! -d ${case} ]]; then
    echo "${case} directory does not exist!" >&2
    return 2
  fi

  time=$(foamListTimes -latestTime -processor -case ${case} 2> /dev/null || foamListTimes -latestTime -case ${case}) 
  echo "${time:-0}"
  return 0  
}

foamLatestTime $1


