#include "RequestQueue.h"
#include <new> // for std::nothrow

RequestQueue::RequestQueue()
    : data(nullptr),
      capacity(0),
      front(0),
      rear(0),
      count(0) {
    // start with a small default capacity
    resize(4);
}

RequestQueue::RequestQueue(int initialCapacity)
    : data(nullptr),
      capacity(0),
      front(0),
      rear(0),
      count(0) {
    if (initialCapacity < 1) {
        initialCapacity = 4;
    }
    resize(initialCapacity);
}

RequestQueue::~RequestQueue() {
    delete[] data;
}

bool RequestQueue::isEmpty() const {
    return count == 0;
}

bool RequestQueue::isFull() const {
    return count == capacity;
}

int RequestQueue::size() const {
    return count;
}

int RequestQueue::nextIndex(int idx) const {
    if (capacity == 0) return 0;
    return (idx + 1) % capacity;
}

bool RequestQueue::enqueue(const Request& req) {
    //Implement enqueue function as explained in the PDF.

    //------
    if (isFull()) {
        int newCap = (capacity > 0) ? capacity * 2 : 4;
        resize(newCap);
    }
    data[rear] = req;
    rear = nextIndex(rear);
    ++count;
    return true;
}

bool RequestQueue::dequeue(Request& outReq) {
    //Implement dequeue function as explained in the PDF.

    //------
    if (isEmpty()) {
        return false;
    }
    outReq = data[front];
    front = nextIndex(front);
    --count;
    return true;
}

bool RequestQueue::peek(Request& outReq) const {
    //Implement peek function as explained in the PDF.

    //------
    if (isEmpty()) {
        return false;
    }
    outReq = data[front];
    return true;
}

void RequestQueue::clear() {
    front = 0;
    rear = 0;
    count = 0;
}

bool RequestQueue::removeById(const std::string& id) {
    //------

    if (isEmpty()) {
        return false;
    }

    Request* newData = new (std::nothrow) Request[capacity];

    int newCount = 0;
    bool removed = false;

    int idx = front;
    for (int i = 0; i < count; ++i) {
        if (!removed && data[idx].getId() == id) {
            removed = true;
        } else {
            newData[newCount++] = data[idx];
        }
        idx = (idx + 1) % capacity;
    }

    if (!removed) {
        delete[] newData;
        return false;
    }

    delete[] data;
    data = newData;
    count = newCount;
    front = 0;
    rear = (newCount == 0) ? 0 : newCount % capacity;

    return true;
}

bool RequestQueue::resize(int newCapacity) {
    //Implement resize function as explained in the PDF.

    //------

    if (newCapacity <= 0) {
        newCapacity = 1;
    }

    Request* newData = new (std::nothrow) Request[newCapacity];

    for (int i = 0; i < count; ++i) {
        int oldIdx = (front + i) % (capacity == 0 ? 1 : capacity);
        newData[i] = data[oldIdx];
    }

    delete[] data;
    data = newData;
    capacity = newCapacity;
    front = 0;
    rear = (count == 0) ? 0 : count % capacity;

    return true;
}
