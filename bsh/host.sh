#!/usr/bin/env bash
# the machine the scripts run on, and helpers that work the same on each
#   source bsh/host.sh
# host_os: linux or windows (git bash / msys). git bash has no zip, and its tar has no zstd, so the helpers use
#   windows' own tar there (bsdtar: reads zstd, writes zips)

is_windows_host() {
    case "$(uname -s)" in
        MINGW*|MSYS*|CYGWIN*) return 0 ;;
        *) return 1 ;;
    esac
}

host_os="linux"
is_windows_host && host_os="windows"

windows_tar() {
    "$(cygpath -u "${SYSTEMROOT}")/System32/tar.exe" "$@"
}

# extract_zst <archive> <into> [paths in it]
extract_zst() {
    local archive="$1" into="$2"
    shift 2
    if [[ ${host_os} == "windows" ]]; then
        windows_tar -xf "${archive}" -C "${into}" "$@"
    else
        tar --zstd -xf "${archive}" -C "${into}" "$@"
    fi
}

# zip_folders <zip> <folder> <what in it> [<folder> <what in it>]...
#   each pair is zipped from inside its folder, so paths in the zip start at <what in it>
zip_folders() {
    local zip="$1"
    shift
    if [[ ${host_os} == "windows" ]]; then
        # bsdtar can't append to a zip, so one call. each -C is absolute: bsdtar takes a relative one from the last
        local args=()
        while [[ $# -ge 2 ]]; do
            args+=(-C "$(cygpath -w "$(realpath "$1")")" "$2")
            shift 2
        done
        windows_tar -a -cf "$(cygpath -w "$(realpath -m "${zip}")")" "${args[@]}"
    else
        local zip_path
        zip_path="$(realpath -m "${zip}")"
        while [[ $# -ge 2 ]]; do
            (cd "$1" && zip -q -r "${zip_path}" "$2")
            shift 2
        done
    fi
}
