#include <common.h>

void main_loop();
void hal_init();

int
main(void) {
  hal_init();
	main_loop();

	return 0;
}
