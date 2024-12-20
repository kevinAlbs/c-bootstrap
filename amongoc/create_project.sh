PROJECT_NAME=${PROJECT_NAME:-}

if [ -z "$PROJECT_NAME" ]; then
    # Try `NAME`
    PROJECT_NAME=${NAME:-}
fi

if [ -z "$PROJECT_NAME" ]; then
    echo "required environment variable PROJECT_NAME / NAME not supplied"
    exit 1
fi

if [[ -d "investigations/$PROJECT_NAME" ]]; then
    echo "investigations/$PROJECT_NAME already exists"
    exit 1
fi

if [[ "$(basename $(pwd))" != "amongoc" ]]; then
    echo "Run from amongoc directory"
    exit 1
fi

cp -r amongoc_template investigations/$PROJECT_NAME
