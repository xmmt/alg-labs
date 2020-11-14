#include <quick_sort.hpp>
#include <iostream>

int main() {
    int a[20] = { 3, 2, 10, 8, 5, 1, 8, 9, 7, 6, 13, 12, 20, 18, 15, 11, 18, 19, 17, 16 };
    insertion_sort::sort(a, a + 20);
    return 0;
}