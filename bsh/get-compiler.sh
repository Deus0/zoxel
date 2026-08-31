get_compiler() {
    local host_arc="$1"
    local target_arc="$2"
    local platform="$3"

    case "$platform" in
        linux)
            if [[ "$target_arc" == "x64" ]]; then
                if [[ "$host_arc" == "x64" ]]; then
                    echo "gcc"
                else
                    echo "x86_64-linux-gnu-gcc"
                fi
            elif [[ "$target_arc" == "arm" ]]; then
                if [[ "$host_arc" == "arm" ]]; then
                    echo "gcc"
                else
                    echo "aarch64-linux-gnu-gcc"
                fi
            else
                echo "Unsupported Linux architecture: $target_arc" >&2
                return 1
            fi
            ;;

        windows)
            if [[ "$target_arc" == "x64" ]]; then
                echo "x86_64-w64-mingw32-gcc"
            else
                echo "Unsupported Windows architecture: $target_arc" >&2
                return 1
            fi
            ;;

        *)
            echo "Unsupported platform: $platform" >&2
            return 1
            ;;
    esac
}
