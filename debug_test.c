#include "debug.h"

int main() {
	Assert(12 == 12, "This can pass");
	Assert(12 != 12, "This cannot pass, because %d != %d", 12, 12);
	panic("Never gets here, because %d != %d", 12, 12);
}