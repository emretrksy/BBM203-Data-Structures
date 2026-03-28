#include "ClassificationYard.h"
#include <iostream>

ClassificationYard::ClassificationYard() {}
ClassificationYard::~ClassificationYard() { clear(); }

WagonList &ClassificationYard::getBlockTrain(int destination, int cargoType)
{
    return blockTrains[destination][cargoType];
}

WagonList *ClassificationYard::getBlocksFor(Destination dest)
{
    return blockTrains[static_cast<int>(dest)];
}

// Inserts vagon to the corract place at the yard
void ClassificationYard::insertWagon(Wagon *w)
{
    if (!w)
        return;
    int dest = static_cast<int>(w->getDestination());
    int cargo = static_cast<int>(w->getCargoType());
    blockTrains[dest][cargo].insertSorted(w);
}

// Merges multiple blocks into a train while keeping blocks grouped
Train *ClassificationYard::assembleTrain(Destination dest, const std::string &trainName)
{
    const int row = static_cast<int>(dest);


    bool any = false;
    for (int j = 0; j < NUM_CARGOTYPES_INT; ++j)
        if (!blockTrains[row][j].isEmpty()) { any = true; break; }
    if (!any) return nullptr;


    const int HAZ = static_cast<int>(CargoType::HAZARDOUS);
    struct Item { int w; int j; };
    Item arr[NUM_CARGOTYPES_INT]; int n = 0;

    for (int j = 0; j < NUM_CARGOTYPES_INT; ++j) {
        if (j == HAZ) continue;                    
        if (!blockTrains[row][j].isEmpty()) {
            Wagon *front = blockTrains[row][j].getFront();    
            int w = front ? front->getWeight() : 0;
            arr[n++] = { w, j };
        }
    }


    for (int a = 0; a < n; ++a) {
        int best = a;
        for (int b = a + 1; b < n; ++b)
            if (arr[b].w > arr[best].w) best = b;
        if (best != a) { Item tmp = arr[a]; arr[a] = arr[best]; arr[best] = tmp; }
    }


    Train *train = new Train(trainName, dest);


    for (int k = 0; k < n; ++k) {
        int j = arr[k].j;
        train->appendWagonList(blockTrains[row][j]);
    }


    WagonList &hz = blockTrains[row][HAZ];
    if (!hz.isEmpty()) {
        Wagon *frontHaz = hz.getFront();
        if (frontHaz) {
            int hid = frontHaz->getID();
            Wagon* take = hz.detachById(hid);
            if (take) {
                train->addWagonToRear(take);
            }
        }
    }

    return train;
}


bool ClassificationYard::isEmpty() const
{
    /* TODO: Check if the entire classification yard is empty.
     *
     * The yard is empty if every blockTrain list for all destination-cargo pairs is empty.
     */

         for (int i = 0; i < static_cast<int>(Destination::NUM_DESTINATIONS); ++i)
    {
        for (int j = 0; j < static_cast<int>(CargoType::NUM_CARGOTYPES); ++j)
        {
            if (!blockTrains[i][j].isEmpty())
                return false;
        }
    }
    return true;

}

void ClassificationYard::clear()
{
    /* TODO: Clear all wagons from the classification yard.
     *
     * Used when resetting or ending the simulation.
     */

    for (int i = 0; i < static_cast<int>(Destination::NUM_DESTINATIONS); ++i)
    {
        for (int j = 0; j < static_cast<int>(CargoType::NUM_CARGOTYPES); ++j)
        {
            blockTrains[i][j].clear();
        }
    }

}

// Print function is already implemented to keep output uniform
void ClassificationYard::print() const
{
    for (int i = 0; i < static_cast<int>(Destination::NUM_DESTINATIONS); ++i)
    {
        auto dest = destinationToString(static_cast<Destination>(i));
        std::cout << "Destination " << dest << ":\n";
        for (int j = 0; j < static_cast<int>(CargoType::NUM_CARGOTYPES); ++j)
        {
            if (!blockTrains[i][j].isEmpty())
            {
                auto type = cargoTypeToString(static_cast<CargoType>(j));
                std::cout << "  CargoType " << type << ": ";
                blockTrains[i][j].print();
            }
        }
    }
}