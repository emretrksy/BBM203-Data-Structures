#include "WagonList.h"

#include <iostream>

WagonList::~WagonList() { clear(); }

void WagonList::clear()
{
    // TODO: Delete all Wagon objects in this list and reset pointers.

    Wagon* current = front;
    while (current)
    {
        Wagon* next = current->getNext();
        delete current;
        current = next;
    }
    front = nullptr;
    rear = nullptr;
    totalWeight = 0;

}

WagonList::WagonList(WagonList &&other) noexcept
{
    // TODO: Implement move constructor.
    
    front = other.front;
    rear = other.rear;
    totalWeight = other.totalWeight;

    other.front = nullptr;
    other.rear = nullptr;
    other.totalWeight = 0;

}

WagonList &WagonList::operator=(WagonList &&other) noexcept
{
    // Operation version of the move constructor.
    // TODO: Implement it.

    if (this != &other)
    {
        clear();
        front = other.front;
        rear = other.rear;
        totalWeight = other.totalWeight;

        other.front = nullptr;
        other.rear = nullptr;
        other.totalWeight = 0;
    }
    return *this;

}

Wagon *WagonList::findById(int id)
{
    // TODO: Find and return the Wagon with given ID.
    // Return nullptr if not found.

    Wagon* current = front;
    while (current)
    {
        if (current->getID() == id)
            return current;
        current = current->getNext();
    }
    return nullptr;

}

void WagonList::addWagonToRear(Wagon *w)
{
    // TODO: Add a Wagon to the rear (end) of the list.
    // This function does not respect the weight order
    // it inserts to end regardless of the weight

    if (!w) return;
    w->setNext(nullptr);
    w->setPrev(nullptr);

    if (isEmpty())
    {
        front = rear = w;
    }
    else
    {
        rear->setNext(w);
        w->setPrev(rear);
        rear = w;
    }

    totalWeight += w->getWeight();

}

int WagonList::getTotalWeight() const { return totalWeight; }

bool WagonList::isEmpty() const
{
    // TODO: Return true if the list has no wagons.

    return front == nullptr;

    //return false;
}

void WagonList::insertSorted(Wagon *wagon)
{

    if (!wagon) return;

    if (isEmpty())
    {
        front = rear = wagon;
        wagon->setNext(nullptr);
        wagon->setPrev(nullptr);
        totalWeight = wagon->getWeight();
        return;
    }

    Wagon* current = front;
    while (current && current->getWeight() > wagon->getWeight())
        current = current->getNext();

    if (!current)
    {
        rear->setNext(wagon);
        wagon->setPrev(rear);
        wagon->setNext(nullptr);
        rear = wagon;
    }
    else if (current == front)
    {
        wagon->setNext(front);
        wagon->setPrev(nullptr);
        front->setPrev(wagon);
        front = wagon;
    }
    else
    {
        Wagon* prev = current->getPrev();
        prev->setNext(wagon);
        wagon->setPrev(prev);
        wagon->setNext(current);
        current->setPrev(wagon);
    }

    totalWeight += wagon->getWeight();

}

void WagonList::appendList(WagonList &&other)
{
   // TODO: Append another WagonList to this one (merge them).
   // Use move semantics to avoid deep copies. (Double && at the parameter already makes it so)
   // 'other' should end up empty after this operation
   // At merge lists (blocks) will be protected 
   // But the one with heavier wagon at the front will be before the other list

   if (other.isEmpty()) return;
   if (isEmpty())
   {
       *this = std::move(other);
       return;
   }

   if (front->getWeight() >= other.front->getWeight())
   {
       rear->setNext(other.front);
       other.front->setPrev(rear);
       rear = other.rear;
   }
   else
   {
       other.rear->setNext(front);
       front->setPrev(other.rear);
       front = other.front;
   }

   totalWeight += other.totalWeight;

   other.front = nullptr;
   other.rear = nullptr;
   other.totalWeight = 0;

}

Wagon *WagonList::detachById(int id)
{
    // TODO: Remove a specific wagon (by ID) from this list and return it.
    // Use: std::cout << "Wagon " << toRemove->id << " detached from Wagon List. " << std::endl;
    // Return nullptr if wagon not found.
    
    Wagon *toRemove = findById(id);
    if (!toRemove)
        return nullptr;

    Wagon *prev = toRemove->getPrev();
    Wagon *next = toRemove->getNext();

    if (prev)
        prev->setNext(next);
    else
        front = next;

    if (next)
        next->setPrev(prev);
    else
        rear = prev;

    toRemove->setNext(nullptr);
    toRemove->setPrev(nullptr);

    totalWeight -= toRemove->getWeight();

    std::cout << "Wagon " << toRemove->getID() << " detached from Wagon List. " << std::endl;
    return toRemove;
    
    //return nullptr;
}


WagonList WagonList::splitAtById(int id)
{
    WagonList newList;

    Wagon *splitStart = findById(id);
    if (!splitStart)
        return newList;

    Wagon *beforeSplit = splitStart->getPrev();

    if (beforeSplit)
    {
        beforeSplit->setNext(nullptr);
        splitStart->setPrev(nullptr);
        rear = beforeSplit;
    }
    else
    {
        front = nullptr;
        rear = nullptr;
    }

    newList.front = splitStart;
    Wagon *temp = splitStart;
    int newTotal = 0;
    while (temp)
    {
        newTotal += temp->getWeight();
        if (!temp->getNext())
            newList.rear = temp;
        temp = temp->getNext();
    }

    newList.totalWeight = newTotal;

    int remainWeight = 0;
    Wagon *c = front;
    while (c)
    {
        remainWeight += c->getWeight();
        c = c->getNext();
    }
    totalWeight = remainWeight;

    return newList;
}

void WagonList::print() const
{

    std::cout << *this << std::endl;
    return;
}

std::ostream &operator<<(std::ostream &os, const WagonList &list)
{
    if (list.isEmpty())
        return os;

    Wagon *current = list.front;

    while (current)
    {
        os << "W" << current->getID() << "(" << current->getWeight() << "ton)";
        if (current->getNext())
            os << " - ";
        current = current->getNext();
    }
    return os;
}
