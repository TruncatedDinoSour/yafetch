# This repository has been migrated to the self-hosted ari-web Forgejo instance: <https://git.ari.lt/ari/yafetch>
<p align="center"> <img src="/media/yafetch.jpg"> </p>

**This project is a fork of [yrwq/yafetch](https://github.com/yrwq/yafetch) with a different build system, more improvements and active porting to newer lua versions**

<h4 align="center">Yafetch is a minimal command line system information tool written in C and configured in Lua. </h4>

# Dependencies

-   a Linux distribution
-   a C compiler
-   Lua 5.4
-   (For NixOS) [lorri](https://github.com/target/lorri)

# Documentation (Installation, Upgrading, etc.)

[Docs](/doc)

# Usage

`yafetch` or `yafetch <config.lua>`

# Configuration

Yafetch is extensible in lua, the default location for the config is `~/.config/yafetch/init.lua`
See [Docs](/doc/FUNC.md).

# Testing

## Requirements

-   Bash
-   Valgrind
-   Coreutils
-   Clang
-   GCC
-   Binutils
-   Lua 5.4

## Testing

```sh
$ ./scripts/tests.sh
```

## Warning

These tests are extremely slow
and painful to do, so beware if you're
running them in a CI environment or something
of sort
