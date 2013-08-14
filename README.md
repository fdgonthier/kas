# Teambox Collaboration Server (kas)

The Teambox Collaboration server is the main daemon of the Teambox collaboration system. The project include the daemon and the accompanying web administration and client interface. 

## Teambox Collaboration web interfaces

The web interface are built on an outdated version of the Pylon's Python framework. The framework is now named Pyramid. The interfaces are quite complicated to setup as the old age of the code means they require version of Python packages which are no longer provided on most distribution. The setup is done through a Python virtual environment.

All the interfaces should be considered unmaintained.

## `web/kwmo` Teambox web interface

## `web/freemium` (end of life)

This is exclusively a web API. This is used by the Windows Teambox client to create user remotely on the server.

## `web/kascfg` (end of life)
