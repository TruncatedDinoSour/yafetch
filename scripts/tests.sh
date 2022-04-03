#!/usr/bin/env bash

set -e

compile() {
    flags=($(./configure 2>&1 |
        grep -- '--' |
        awk '{print $1}' |
        grep -v -e install -e config -e help))

    for ((length = 1; length <= "${#flags[@]}"; ++length)); do
        for ((start = 0; start + length <= "${#flags[@]}"; ++start)); do
            _flags=(${flags[@]:start:length})

            log "Configuring with ${_flags[*]}..."
            ./configure ${_flags[@]}

            log "Compiling with $CFLAGS..."
            make clean
            make

            log "Running $1..."
            eval "$1"
        done
    done
}

log() {
    echo -e "\n * $1\n" >&2
}

main() {
    log_file="${LOGFILE:-valgrind.log}"
    tools=(memcheck cachegrind callgrind helgrind drd massif dhat lackey none exp-bbv)

    base_cmd='valgrind --trace-children=yes --log-file=valgrind.log --error-exitcode=1'
    end_cmd="head -n 1 -- '$log_file' && exit 127"

    for tool in "${tools[@]}"; do
        log "Trying valgrind tool $tool"

        case "$tool" in
        memcheck) opt='--leak-check=full --show-leak-kinds=all  --track-origins=yes --expensive-definedness-checks=yes' ;;
        cachegrind) opt='--branch-sim=yes' ;;
        drd) opt='--check-stack-var=yes --free-is-write=yes' ;;
        massif) opt='--stacks=yes' ;;
        *) opt='' ;;
        esac

        compile "$base_cmd $opt --tool='$tool' -s ./yafetch || ($end_cmd)"

        log 'Cleaning up'

        rm -rf ./*.out.*
        make clean
    done
}

main "$@"
