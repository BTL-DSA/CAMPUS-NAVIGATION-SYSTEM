#include "MinHeap.h"
#include <stdexcept>
#include <iomanip>

MinHeap::MinHeap(int initialCapacity)
    : heapArray(nullptr), capacity(0), size(0) {
    if (initialCapacity < 4) initialCapacity = 4;
    capacity = initialCapacity;
    heapArray = new HeapNode[capacity];
}

MinHeap::~MinHeap() {
    freeMemory();
}

MinHeap::MinHeap(const MinHeap& other)
    : heapArray(nullptr), capacity(0), size(0) {
    copyFrom(other);
}

MinHeap& MinHeap::operator=(const MinHeap& other) {
    if (this != &other) {
        freeMemory();
        copyFrom(other);
    }
    return *this;
}

void MinHeap::freeMemory() {
    if (heapArray != nullptr) {
        delete[] heapArray;
        heapArray = nullptr;
    }
    capacity = 0;
    size = 0;
}

void MinHeap::copyFrom(const MinHeap& other) {
    capacity = (other.capacity > 0) ? other.capacity : 32;
    size = other.size;
    heapArray = new HeapNode[capacity];
    for (int i = 0; i < size; ++i) {
        heapArray[i] = other.heapArray[i];
    }
}

void MinHeap::resize(int newCapacity) {
    HeapNode* newArray = new HeapNode[newCapacity];
    for (int i = 0; i < size; ++i) {
        newArray[i] = heapArray[i];
    }
    delete[] heapArray;
    heapArray = newArray;
    capacity = newCapacity;
}

void MinHeap::swapNodes(int i, int j) {
    HeapNode temp = heapArray[i];
    heapArray[i] = heapArray[j];
    heapArray[j] = temp;
}

void MinHeap::siftUp(int index) {
    while (index > 0 && heapArray[parent(index)].distance > heapArray[index].distance) {
        swapNodes(index, parent(index));
        index = parent(index);
    }
}

void MinHeap::siftDown(int index) {
    int smallest = index;
    int left = leftChild(index);
    int right = rightChild(index);

    if (left < size && heapArray[left].distance < heapArray[smallest].distance) {
        smallest = left;
    }

    if (right < size && heapArray[right].distance < heapArray[smallest].distance) {
        smallest = right;
    }

    if (smallest != index) {
        swapNodes(index, smallest);
        siftDown(smallest);
    }
}

void MinHeap::insert(const std::string& name, double distance) {
    if (size >= capacity) {
        int newCap = (capacity == 0) ? 8 : capacity * 2;
        resize(newCap);
    }

    heapArray[size] = HeapNode(name, distance);
    siftUp(size);
    ++size;
}

HeapNode MinHeap::extractMin() {
    if (isEmpty()) {
        throw std::underflow_error("[MinHeap Error] Hàng đợi ưu tiên đang rỗng!");
    }

    HeapNode minNode = heapArray[0];
    heapArray[0] = heapArray[size - 1];
    --size;

    if (size > 0) {
        siftDown(0);
    }

    return minNode;
}

HeapNode MinHeap::peek() const {
    if (isEmpty()) {
        throw std::underflow_error("[MinHeap Error] Hàng đợi ưu tiên đang rỗng!");
    }
    return heapArray[0];
}

bool MinHeap::decreaseKey(const std::string& name, double newDistance) {
    for (int i = 0; i < size; ++i) {
        if (heapArray[i].name == name) {
            if (newDistance < heapArray[i].distance) {
                heapArray[i].distance = newDistance;
                siftUp(i);
                return true;
            }
            return false;
        }
    }
    return false;
}

bool MinHeap::contains(const std::string& name) const {
    for (int i = 0; i < size; ++i) {
        if (heapArray[i].name == name) {
            return true;
        }
    }
    return false;
}

double MinHeap::getDistance(const std::string& name) const {
    for (int i = 0; i < size; ++i) {
        if (heapArray[i].name == name) {
            return heapArray[i].distance;
        }
    }
    return -1.0;
}

bool MinHeap::isEmpty() const {
    return size == 0;
}

int MinHeap::getSize() const {
    return size;
}

int MinHeap::getCapacity() const {
    return capacity;
}

void MinHeap::clear() {
    size = 0;
}

void MinHeap::printHeap() const {
    std::cout << "[MinHeap] Số phần tử: " << size << "/" << capacity << "\n";
    for (int i = 0; i < size; ++i) {
        std::cout << "  [" << i << "] " << heapArray[i].name 
                  << " (khoảng cách: " << heapArray[i].distance << ")\n";
    }
}

