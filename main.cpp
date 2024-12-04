#include <iostream>
#include "bucket_storage.hpp"

int main() {
    std::cout << "=== Создание BucketStorage и добавление элементов ===\n";
    BucketStorage<int> stl;
    for (int i = 0; i < 10; i++) {
        stl.insert(i);
    }
    std::cout << "Элементы в stl после вставки: ";
    for (const auto& elem : stl) {
        std::cout << elem << " ";
    }
    std::cout << "\nРазмер: " << stl.size() << ", Вместимость: " << stl.capacity() << "\n\n";

    std::cout << "=== Удаление элемента ===\n";
    auto it = stl.begin();
    ++it; // Удаляем второй элемент
    stl.erase(it);
    std::cout << "Элементы в stl после удаления: ";
    for (const auto& elem : stl) {
        std::cout << elem << " ";
    }
    std::cout << "\nРазмер: " << stl.size() << ", Вместимость: " << stl.capacity() << "\n\n";

    std::cout << "=== Замена содержимого через swap ===\n";
    BucketStorage<int> stl1;
    stl1.insert(2938);
    stl.swap(stl1);
    std::cout << "Элементы в stl после swap: ";
    for (const auto& elem : stl) {
        std::cout << elem << " ";
    }
    std::cout << "\nЭлементы в stl1 после swap: ";
    for (const auto& elem : stl1) {
        std::cout << elem << " ";
    }
    std::cout << "\n\n";

    std::cout << "=== Вставка нового элемента ===\n";
    stl.insert(23);
    std::cout << "Элементы в stl после вставки 23: ";
    for (const auto& elem : stl) {
        std::cout << elem << " ";
    }
    std::cout << "\nРазмер: " << stl.size() << ", Вместимость: " << stl.capacity() << "\n\n";

    std::cout << "=== Оптимизация через shrink_to_fit ===\n";
    std::cout << "Вместимость до shrink_to_fit: " << stl.capacity() << "\n";
    stl.shrink_to_fit();
    std::cout << "Вместимость после shrink_to_fit: " << stl.capacity() << "\n\n";

    std::cout << "=== Очистка BucketStorage ===\n";
    stl.clear();
    stl1.clear();
    std::cout << "Размер stl после очистки: " << stl.size() << "\n";
    std::cout << "Размер stl1 после очистки: " << stl1.size() << "\n";

    return 0;
}
