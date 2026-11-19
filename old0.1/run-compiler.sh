#!/usr/bin/env bash
set -e

# Automatyczne wykrycie narzędzia (podman lub docker)
if command -v podman &> /dev/null; then
    CONTAINER_TOOL="podman"
elif command -v docker &> /dev/null; then
    CONTAINER_TOOL="docker"
else
    echo "[!] Nie znaleziono ani Podmana, ani Dockera!"
    exit 1
fi

IMAGE_NAME="cotanos-dev"

# Sprawdzenie czy obraz istnieje (kompatybilne z Dockerem i Podmanem)
if ! $CONTAINER_TOOL image inspect $IMAGE_NAME &> /dev/null; then
    echo "==> Budowanie obrazu środowiska CoTanOS ($CONTAINER_TOOL)..."
    $CONTAINER_TOOL build -t $IMAGE_NAME -f tls/container/Dockerfile .
fi

echo "==> Uruchamianie polecenia w odizolowanym środowisku..."
$CONTAINER_TOOL run --rm -it \
    --volume "$(pwd):/workspace:z" \
    $IMAGE_NAME \
    "$@"