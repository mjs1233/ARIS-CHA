#include <cstdint>

extern "C" int func(int b) {
    volatile int a = 10;
    for (int i = 0; i < 10; i++) {
        a += 10;
    }
    return a + b;
}

extern "C" int main() {
    int c = func(10);
    int d = func(100);
    if (c + d > 10) {
        return 0;
    }


    return c;
    while (true) {
        asm volatile("wfi");
    }
}
