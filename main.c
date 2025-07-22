#include <nd/nd.h>

unsigned type_other;

int on_add(unsigned ref, uint64_t v)
{
	OBJ obj;
	SKEL skel;

	nd_get(HD_OBJ, &obj, &ref);
	if (obj.type != type_other)
		return 1;

	nd_get(HD_SKEL, &skel, &obj.skid);

	obj.art_id = skel.max_art ? 1 + (v & 0xf) % skel.max_art : 0;
	nd_put(HD_OBJ, &ref, &obj);
	return 0;
}

void mod_open(void *arg __attribute__((unused))) {
	type_other = nd_put(HD_TYPE, NULL, "other");
}

void mod_install(void *arg) {
	mod_open(arg);
}
