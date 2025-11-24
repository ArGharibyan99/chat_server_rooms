# Docker helper scripts

This repository includes two small helper scripts in the `scripts/` folder to make building and running a development Docker container quick and reproducible:

- `scripts/build_docker` — builds the `u24` Docker image using the bundled `Dockerfile`.
- `scripts/du24` — starts an interactive container from the `u24` image, mounts the current working directory, and opens a shell.

## Prerequisites

- Docker installed and working on your machine (docker daemon running).
- Scripts expect to be executed from the repository root or any location you want mounted into the container.
- The scripts use `sudo` in their default form. If your user is in the `docker` group and you can run Docker without `sudo`, you can run the commands without it.

## Usage examples

From the repository root:

1) Build the image (may require `sudo`):

   sudo ./scripts/build_docker

2) Start an interactive container with your repo mounted:

   sudo ./scripts/du24

Inside the container you'll be dropped into a shell as a non-root user (`devuser` in the provided `Dockerfile`) at the same working directory as on the host. From there you can run cmake/make, tests, editors, etc.