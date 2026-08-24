# Overview

This repository contains all code relevant to my bachelor thesis - Effiziente, inkrementelle Metadatenerfassung für heterogene, wissenschaftliche Daten
Included are the Updater, Crawler, Server/Extractor-Manager, a Docker Compose file for the Database as well as the front- and backend.

# Installation

_Important_: Vcpkg is needed to install Crawler, Updater, Extraktor-Manager (file is called server). npm is needed to run the front- and backend.
To install the code and run it yourself, use vcpkg:

```sh
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=[path/to/vcpkg]/scripts/buildsystems/vcpkg.cmake
cmake --build build
```

### Warning:

This project is reliant on external libraries. Using the Vcpkg/Cmake script will download and install these. If any of the dependencies are already installed system wide, they should be excluded.

# How to execute

### OpenSearch

Before everything else, OpenSearch needs to be started. From the projects root folder do the following:

```sh
cd opensearch
docker compose up -d
cd ..
```

OpenSearch need some time to start up, about 1-2 Minutes.

### Extraktor-Manager

After waiting for the database to spin up, start the Extraktor-Manager next.
It take a path to a settings file as a parameter (note that the executable is called server):

```sh
executables/server settings/settings.toml
```

### Crawler and Updater

Next, the Crawler or the Updater can be started. Don't execute both at once.

#### Crawler

The Crawler takes a path to the same settings file as the server (-s) as well as a relative or total path to where the crawl starts (-t).

```sh
executables/crawler -s settings/settings.toml -t testing/files/
```

#### Updater

The Updater needs the settings file (-s), the file listing the old files (-o), and a file listing the current files (-c)

```sh
executables/updater -s settings/settings.toml -o filesOld -c filesCurrent
```

### Webinterface

#### Backend

To start the backend, change into its folder, install the dependencies and run: npm run dev

```sh
cd web_interface/backend
npm install
npm run dev
```

#### Frontend

To start the frontend, change into its folder, install the dependencies and run: npm run dev

```sh
cd web_interface/frontend
npm install
npm run dev
```
