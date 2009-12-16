#include <stdio.h>
#include <dlfcn.h>

int main(int argc, char* argv[])
{
	int i;
	for (i = 1; i < argc; ++i) {
		void* handle = dlopen(argv[i], RTLD_NOW);
		if (!handle) {
			fprintf(stderr, "%s", dlerror());
			return 1;
		} else {
			dlclose(handle);
		}
	}
	return 0;
}
