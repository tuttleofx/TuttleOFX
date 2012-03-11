
# Define some colors first:
red='\e[0;31m'
RED='\e[1;31m'
blue='\e[0;34m'
BLUE='\e[1;34m'
cyan='\e[0;36m'
CYAN='\e[1;36m'
NC='\e[0m'              # No Color

OPTIONS="-o nospace" # complete without spaces

function _sam_check {
echo -e "\n$BLUE""TODO$NC"
}
function _sam_cp {
echo -e "\n$BLUE""TODO$NC"
}

function _sam_do {
  if [[ ! -f ~/.tuttle/tuttlePluginCacheSerialize.xml ]] ; then
    echo -e "\n$RED""loading plugins...$NC"
  fi
  if [[ $prev == "//" || $argc == 2 ]] ; then
    opts=`sam do --nodes-list`
    COMPREPLY=( $(compgen -W "$opts" -- $cur ) )
  else
    local count numberOfCases word currentNode paramType param
    currentNode="${COMP_WORDS[2]}"
    for numberOfCases in `seq 0 $argc` ; do
      word="${COMP_WORDS[$argc-$numberOfCases]}"
      if [[ "$word" == "//" ]] ; then
	currentNode="${COMP_WORDS[$argc-$numberOfCases+1]}"
      fi
    done

    if [[ "$currentNode" == "$prev" || "${COMP_WORDS[argc-2]}" ==  "=" ]] ; then
      opts=`sam do $currentNode --parameters-list`
      #echo "**"$cur"**"
      COMPREPLY=( $(compgen -W "$opts" -S"="  -- $cur ) $(compgen -W " // " -- $cur ) )
    else
      param="${COMP_WORDS[argc-1]}"
      #echo -e "$param"
      if [[ $param == "=" ]] ; then
	param="${COMP_WORDS[argc-2]}"
      fi

      prefix=""
      if [[ $cur != "=" ]] ; then
	prefix=$cur
      fi

      paramType=`sam do $currentNode --parameter-type $param`
      #echo $paramType
      if [[ "$paramType" == "OfxParamTypeString" ]] ; then
	opts=`sam ls -df --enable-color=false $prefix | cut -d" " -f1`
	echo -e "$BLUE$prefix$NC"
	COMPREPLY=( $(compgen -W "$opts" -- $prefix) )
      fi
      if [[ "$paramType" == "OfxParamTypeChoice" ]] ; then
	opts=`sam do $currentNode --parameter-values $param | cut -d' ' -f1`

	COMPREPLY=( $(compgen -W "$opts" -- $prefix ) )
      fi
      if [[ "$paramType" == "OfxParamTypeBoolean" ]] ; then
	opts="'1 ' '0 '"
	COMPREPLY=( $(compgen -W "$opts" ) )
      fi
    fi
  fi
}

function _sam_info {
echo -e "\n$BLUE""TODO$NC"
}

function _sam_ls {
startLine=`sam ls -h | sed -n '/TuttleOFX/='`
endLine=`sam ls -h | sed -n '/OPTIONS/='`
opts=`sam ls -h | sed $startLine','$endLine'd' | sed '$d' | cut -d" " -f3`
opts+=`sam ls -h | sed $startLine','$endLine'd' | sed '$d' | grep "\[" | cut -d" " -f 5`
opts+=`sam ls -df --enable-color=false | cut -d" " -f1 `
COMPREPLY=( $(compgen -W "$opts" ) )
}

function _sam_mv {
startLine=`sam mv -h | sed -n '/TuttleOFX/='`
endLine=`sam mv -h | sed -n '/OPTIONS/='`
opts=`sam mv -h | sed $startLine','$endLine'd' | sed '$d' | cut -d" " -f3`
opts+=`sam mv -h | sed $startLine','$endLine'd' | sed '$d' | grep "\[" | cut -d" " -f 5`
opts+=`sam mv -df --enable-color=false | cut -d" " -f1 `
COMPREPLY=( $(compgen -W "$opts" ) )
}

function _sam_plugins {
echo -e "\n$BLUE""TODO$NC"
}

function _sam_rm {
echo -e "\n$BLUE""TODO$NC"
}

# sam auto-complete function
_sam(){
# local variables
local args cur opts prev subcmd

#COMPREPLY actual completion
COMPREPLY=()

# argc : numbers of arguments in command ligne
argc=${COMP_CWORD}

# cur  : last word on the command line
cur="${COMP_WORDS[argc]}"
prev="${COMP_WORDS[argc-1]}" # previous word


# possible auto-completion process
if [[ $argc == 1 ]] ; then
opts=`sam --commands-list`
COMPREPLY=( $(compgen -W "$opts" -- $cur ) )
else
  if [[ $argc > 1 ]] ; then
    subcmd="${COMP_WORDS[1]}"
    if [[ "$subcmd" == "check" ]] ; then
      _sam_check
    fi
    if [[ "$subcmd" == "cp" ]] ; then
      _sam_cp
    fi
    if [[ "$subcmd" == "do" ]] ; then
      _sam_do
    fi
    if [[ "$subcmd" == "info" ]] ; then
      _sam_info
    fi
    if [[ "$subcmd" == "ls" ]] ; then
      _sam_ls
    fi
    if [[ "$subcmd" == "mv" ]] ; then
      _sam_mv
    fi
    if [[ "$subcmd" == "plugins" ]] ; then
      _sam_plugins
    fi
    if [[ "$subcmd" == "rm" ]] ; then
      _sam_ls
    fi
  fi
fi
}

# On active l'auto-completion de la commande grenlibre en relation avec la fonction _grenlibre
complete $OPTIONS -F _sam sam

complete $OPTIONS -F _sam_do sam-do

