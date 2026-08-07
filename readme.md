# Overview

This repository contains all code relevant to my bachelor thesis.
Included are the Updater, the Crawler, the Server/Extractor-Manager and a Docker Compose file for the Database.

# Installation

To install the code and run it yourself, a CMakeLists file is included.
Create a build folder and execute cmake steps:

```sh
mkdir build
cd build
cmake ..
cmake --build .
```

### Warning:

This project is reliant on external libraries. Using the Cmake script will install download and install these. If any of the dependencies are already installed system wide, they should be excluded from the CMakeLists.txt

# How to execute

### OpenSearch

Before anything else, OpenSearch needs to be started. From the base folder do the following:

```sh
cd opensearch
docker compose up -d
cd ..
```

OpenSearch need some time to start up, about 1-2 Minutes.

### Server

After waiting for the database to spin up, start the server next.
The server take a path to a settings file as a parameter:

```sh
executables/server settings/settings.toml
```

### Crawler and Updater

Next, the Crawler or the Updater can be started. Don't execute both at once.

#### Crawler

```sh

```
