case "$(uname -s | tr '[:upper:]' '[:lower:]')" in
    cygwin*)
        OS="WINDOWS"
        ;;
    darwin)
        OS="MACOS"
        ;;
    *)
        OS="LINUX"
        ;;
esac