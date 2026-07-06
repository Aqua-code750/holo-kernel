# HoloShell (user-space prototype)

This is a small user-space shell prototype that detects a Doom WAD (`DOOM.WAD` by default) and attempts to launch a Doom engine (Chocolate Doom or `doom`) if available on PATH.

Build:

```sh
cd user-shell
make
```

Run:

```sh
./holoshell
```

Usage:
- `doom [path]` — launch Doom engine with the WAD (defaults to `DOOM.WAD`)
- `ls`, `clear`, `help`, `echo`, `about`, `exit`

Notes:
- To build the bootable kernel and ISO you should use a Linux toolchain (WSL or native Linux). See parent README for details.
