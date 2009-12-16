#include <stdio.h>
#include <dlfcn.h>
#include <string.h>

int main(int argc, char* argv[])
{
	int i;
	for (i = 1; i < argc; ++i) {
		const char* arg = argv[i];
		size_t l = strlen(arg);
		if (l >= 3) {
			// skip static libs (ie ending with '.a')
			if (arg[l - 2] == '.' && arg[l - 1] == 'a') {
				continue;
			}
		}
		void* handle = dlopen(arg, RTLD_NOW);
		if (!handle) {
			fprintf(stderr, "%s", dlerror());
			return 1;
		} else {
			dlclose(handle);
		}
	}
	return 0;
}
