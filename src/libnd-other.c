/* main.c — nd-other, ported to libxylem.
 *
 * Registers an object type called "other" and picks a random piece of art for
 * anything of that type as it is added to the world. It is the slice's proof
 * of the HD_* indirection (MODS.md §0.2) from a module that uses two
 * different engine tables: HD_OBJ and HD_SKEL, both engine-owned, neither
 * visible to the module as a corm handle.
 *
 * Original: tty-pt/nd-other @ 477 B main.c, from the nd-basics superproject.
 * Ported without behavioural change.
 *
 * This TU XY_IMPLs on_add and so must NOT include nd/hooks.h.
 */

#include <ttypt/xy-mod.h>

#include <nd/xy.h>

/* The type id nd_put(HD_TYPE, NULL, "other") hands back at install. The
 * original stored it in a bare `unsigned type_other` global set by mod_open;
 * SIC ran mod_install for a first load and mod_open for a known one, and both
 * did the same single assignment, so they collapse to one xy_install. */
static unsigned type_other;

XY_MODULE_API void
xy_install(void)
{
	type_other = nd_put(HD_TYPE, NULL, "other");
	WARN("nd-other: xy_install, type \"other\" = %u\n", type_other);
}

XY_IMPL(int, on_add, unsigned, ref, unsigned, type, uint64_t, v)
{
	OBJ obj;
	SKEL skel;

	/* Once-only marker. Deliberately fires on the FIRST call rather than on
	 * a match: the engine creates no object of type "other" by itself -- the
	 * type exists for other modules to use -- so a marker on the match path
	 * would never print and the test would either be vacuous or need a
	 * fixture that manufactures one. What this actually proves is that the
	 * hook is wired AND that the type comparison behaves: the printed type
	 * is the engine's, type_other is what nd_put(HD_TYPE) returned at
	 * install, and a caller can see the two are different. */
	{
		static int marked;
		if (!marked) {
			marked = 1;
			WARN("nd-other: on_add first call, type=%u "
			     "type_other=%u (%s)\n", type, type_other,
			     type == type_other ? "match" : "skip, as expected");
		}
	}

	if (type != type_other)
		return 1;

	nd_get(HD_OBJ, &obj, &ref);
	nd_get(HD_SKEL, &skel, &obj.skid);

	obj.art_id = skel.max_art ? 1 + (v & 0xf) % skel.max_art : 0;
	nd_put(HD_OBJ, &ref, &obj);
	return 0;
}
