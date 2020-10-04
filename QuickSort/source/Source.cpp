#include <quick_sort.hpp>
#include <iostream>

int main() {
    int a[10] = { 3, 2, 10, 8, 5, 1, 8, 9, 7, 6 };
    quick_sort::sort(a + 4, a + 3);
    return 0;
}