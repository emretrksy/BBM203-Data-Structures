#include "MissionStack.h"
#include <new>     // for std::nothrow

MissionStack::MissionStack()
    : data(nullptr),
      capacity(0),
      top(-1) {
    resize(4);
}

MissionStack::MissionStack(int initialCapacity)
    : data(nullptr),
      capacity(0),
      top(-1) {
    if (initialCapacity < 1) {
        initialCapacity = 4;
    }
    resize(initialCapacity);
}

MissionStack::~MissionStack() {
    delete[] data;
}

bool MissionStack::isEmpty() const {
    return top == -1;
}

int MissionStack::size() const {
    return top + 1;
}

bool MissionStack::push(const Request& req) {
    //Implement push function as explained in the PDF.

    //------
    if (size() == capacity) {
        int newCap = (capacity > 0) ? capacity * 2 : 4;
        if (!resize(newCap)) {
            return false;
        }
    }
    data[++top] = req;
    return true;
}

bool MissionStack::pop(Request& outReq) {
    //Implement pop function as explained in the PDF.

    //------
    if (isEmpty()) {
        return false;
    }
    outReq = data[top--];
    return true;
}

bool MissionStack::peek(Request& outReq) const {
    //Implement peek function as explained in the PDF.

    //------
    if (isEmpty()) {
        return false;
    }
    outReq = data[top];
    return true;
}

void MissionStack::clear() {
    top = -1;
}

bool MissionStack::resize(int newCapacity) {

    //------

    Request* newData = new (std::nothrow) Request[newCapacity];
    if (!newData) {
        return false;
    }

    for (int i = 0; i <= top; ++i) {
        newData[i] = data[i];
    }

    delete[] data;
    data     = newData;
    capacity = newCapacity;

    return true;
}
