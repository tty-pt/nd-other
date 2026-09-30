# axil-nd-other

`nd-other` for [axil-nd](../axil-nd), ported from SIC to libxylem.

Registers an object type called `other` and picks a random piece of art for
anything of that type as it is added to the world. It is the slice's proof of
the `HD_*` indirection (MODS.md §0.2) from a module that uses two different
engine tables: `HD_OBJ` and `HD_SKEL`, both engine-owned, neither visible to the
module as a corm handle.

## Install

```sh
make install
```

Installs one file:

```
lib/libnd-other.so
```

There is deliberately no `lib/nd-other.so` symlink. `xy_load()` appends `.so`
itself and does not retry with a `lib` prefix, so the engine's `mods.load` names
this module `libnd-other` and `dlopen`s `libnd-other.so`. A soname symlink would
also have been silently dropped from the OpenBSD package: `tty-pt/ci` builds the
packing list from `find usr -type f`, which never lists a symlink, so the package
would have shipped the library under one name and asked the loader for another.

Also packaged for deb, apk, rpm, brew and openbsd from a `v*` tag.

It installs no header because it exports no API — `on_add` is an event hook the
engine declares itself in `nd/hooks.h`, not something another module calls.

## Build from source

```sh
make
```

Needs [libxylem](https://github.com/tty-pt/libxylem) (headers already in
`$(PREFIX)/include`) and the engine's game API, `<nd/xy.h>`, from either an
`axil-nd` checkout beside this repo or an installed `axil-nd`:

```sh
git clone https://github.com/tty-pt/nd-other && cd nd-other
git clone https://github.com/tty-pt/axil-nd ../axil-nd
make
```

`<nd/xy.h>` installs to `$(PREFIX)/include/nd/`, the same directory that already
carries `<ttypt/xy.h>`, so an **installed** engine needs no `-I` of its own here.
Against a checkout beside this repo it is `-I../axil-nd/include`; both paths are
on `CFLAGS` at once and a missing `-I` is ignored, so the same command works
either way.

## What it does

* `xy_install()` registers the object type: `nd_put(HD_TYPE, NULL, "other")`,
  keeping the returned id in a file-static `type_other`.
* `on_add(ref, type, v)` returns immediately unless `type == type_other`. On a
  match it reads the object and its skeleton, sets `obj.art_id` to a random
  variant within `skel.max_art`, and writes the object back.

## Testing

There is no `test.sh` here. Behaviour is asserted by the engine's own suite,
which builds every module in its `mods.load`, boots, and greps stderr:

```sh
cd ../axil-nd
make && ./test.sh
```

The suite asserts `nd-other: on_add first call`, which proves the hook is wired
and that the printed `type` is the engine's while `type_other` is what
`nd_put(HD_TYPE)` returned at install.

That marker deliberately fires on the **first** call rather than on a match. The
engine creates no object of type `other` by itself — the type exists for other
modules to use — so a marker on the match path would never print and the test
would either be vacuous or need a fixture that manufactures one. What it proves
is that the hook is wired *and* that the type comparison behaves: the two are
different numbers and the module correctly skips.

## Notes from the port

* **The include changed spelling.** It was `"papi/nd-xy.h"`, a file that no
  longer exists in any checkout: the engine moved its module-facing tree from
  `papi/` to `nd/`, and `papi/` now holds only `nd.h`. The module could not
  compile until this was fixed.
* `mod_install` and `mod_open` collapsed into one `xy_install`. The original
  stored the type id in a bare `unsigned type_other` global set by `mod_open`;
  SIC ran `mod_install` for a first load and `mod_open` for a known one, and both
  did the same single assignment, so they collapse.
* Ported without behavioural change otherwise.

## License

BSD 2-Clause, carried over from `tty-pt/nd-other`. See `LICENSE`.
