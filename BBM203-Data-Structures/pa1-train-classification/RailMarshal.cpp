#include "RailMarshal.h"
#include <iostream>
#include <sstream>
#include <algorithm>

RailMarshal::RailMarshal()
{
    // TODO: Initialize each track in the departure yard.
    // Each TrainTrack corresponds to one Destination.
    
    for (int i = 0; i < static_cast<int>(Destination::NUM_DESTINATIONS); ++i)
    {
        departureYard[i] = TrainTrack(static_cast<Destination>(i));
    }

}

RailMarshal::~RailMarshal()
{
    // TODO: Cleanup remaining trains, prevent memory leaks

    classificationYard.clear();

    for (int i = 0; i < static_cast<int>(Destination::NUM_DESTINATIONS); ++i)
    {
        departureYard[i] = TrainTrack(static_cast<Destination>(i));
    }

}

// Getter (ready)
ClassificationYard &RailMarshal::getClassificationYard()
{
    return classificationYard;
}

// Getter (ready)
TrainTrack &RailMarshal::getDepartureYard(Destination dest)
{
    int idx = static_cast<int>(dest);
    return departureYard[idx];
}

void RailMarshal::processCommand(const std::string &line)
{
    // TODO: Parse user commands from input lines.
    //std::cout << "[DEBUG] processCommand received: " << line << std::endl;

    std::istringstream iss(line);
    std::string cmd;
    iss >> cmd;
    if (cmd.empty()) return;

    // if ADD_WAGON
    // Use: std::cout << "Error: Invalid ADD_WAGON parameters.\n";
    // Use: std::cout << "Wagon " << *w << " added to yard." << std::endl;

    if (cmd == "ADD_WAGON")
    {
        int id, weight, maxCoupler;
        std::string cargoStr, destStr;
        if (!(iss >> id >> cargoStr >> destStr >> weight >> maxCoupler))
        {
            std::cout << "Error: Invalid ADD_WAGON parameters.\n";
            return;
        }

        CargoType cargo;
        Destination dest;
        try {
            cargo = parseCargo(cargoStr);
            dest = parseDestination(destStr);
        } catch (...) {
            std::cout << "Error: Invalid ADD_WAGON parameters.\n";
            return;
        }

        Wagon *w = new Wagon(id, cargo, dest, weight, maxCoupler);
        classificationYard.insertWagon(w);
        std::cout << "Wagon " << *w << " added to yard." << std::endl;
    }

    // if REMOVE_WAGON
    // Use: std::cout << "Error: Invalid REMOVE_WAGON parameters.\n";
    // Use: std::cout << "Wagon " << id << " removed." << std::endl;
    // Use: std::cout << "Error: Wagon " << id << " not found." << std::endl;

    else if (cmd == "REMOVE_WAGON")
    {
        int id;
        if (!(iss >> id)) {
            std::cout << "Error: Invalid REMOVE_WAGON parameters.\n";
            return;
        }

        bool found = false;
        for (int i = 0; !found && i < NUM_DESTINATIONS_INT; ++i) {
            for (int j = 0; !found && j < NUM_CARGOTYPES_INT; ++j) {
                WagonList &wl = classificationYard.getBlockTrain(i, j);
                Wagon *w = wl.detachById(id);
                if (w) {
                    found = true;
                    delete w;
                    std::cout << "Wagon " << id << " removed." << std::endl;
                }
            }
        }
        if (!found)
            std::cout << "Error: Wagon " << id << " not found." << std::endl;
    }

    // if ASSEMBLE_TRAIN
    //  Use: std::cout << "Error: Invalid ASSEMBLE_TRAIN parameters.\n";
    //  Use: std::cout << "No wagons to assemble for " << destStr << std::endl;
    //  verify couplers and possibly split (deterministic)
    //  Keep splitting the *front* train until no more overloaded couplers found
    //  create new train with same destination and name suffix
    //  use std::cout << "Train " << newTrain->getName() << " assembled after split with "
    //  << newTrain->getWagons()<< " wagons." << std::endl;
    // use std::cout << "Train " << t->getName() << " assembled with " << t->getWagons() << " wagons." << std::endl;

    else if (cmd == "ASSEMBLE_TRAIN")
    {
        std::string destStr;
        if (!(iss >> destStr)) {
            std::cout << "Error: Invalid ASSEMBLE_TRAIN parameters.\n";
            return;
        }

        Destination dest;
        try { dest = parseDestination(destStr); }
        catch (...) {
            std::cout << "Error: Invalid ASSEMBLE_TRAIN parameters.\n";
            return;
        }

        const int di = static_cast<int>(dest);
        
        std::string trainName = departureYard[di].generateTrainName();

        Train *t = classificationYard.assembleTrain(dest, trainName);
        if (!t) {
            std::cout << "No wagons to assemble for " << destStr << std::endl;
            return;
        }

        
        int splitCounter = 0;
        while (true) {
            Train *splitTrain = t->verifyCouplersAndSplit(++splitCounter);
            if (!splitTrain) break;

            departureYard[di].addTrain(splitTrain);
            std::cout << "Train " << splitTrain->getName()
                    << " assembled after split with " << splitTrain->getWagons()
                    << " wagons." << std::endl;
        }

        departureYard[di].addTrain(t);
        std::cout << "Train " << t->getName()
                << " assembled with " << t->getWagons() << " wagons." << std::endl;

        
        if (TrainTrack::autoDispatch &&
            departureYard[di].getTotalWeight() > AUTO_DISPATCH_LIMIT) {
            std::cout << "Auto-dispatch: departing " << t->getName() << " to make room.\n";
            dispatchFromTrack(dest);
        }
    }

    // if DISPATCH_TRAIN
    //  use: std::cout << "Error: Invalid DISPATCH parameters.\n";
    //  use: std::cout << "Error: No trains to dispatch from track " << destStr << ".\n";
    //  use:  std::cout << "Dispatching " << train->getName() << " (" << t->getTotalWeight() << " tons)." << std::endl;

    else if (cmd == "DISPATCH_TRAIN")
    {
        std::string destStr;
        if (!(iss >> destStr))
        {
            std::cout << "Error: Invalid DISPATCH parameters.\n";
            return;
        }

        Destination dest;
        try { dest = parseDestination(destStr); }
        catch (...) {
            std::cout << "Error: Invalid DISPATCH parameters.\n";
            return;
        }

        dispatchFromTrack(dest);
    }

    // if PRINT_YARD
    //  use std::cout << "--- classification Yard ---\n";

    else if (cmd == "PRINT_YARD")
    {
        std::cout << "--- classification Yard ---\n";
        classificationYard.print();
    }

    // if PRINT_TRACK
    //  use std::cout << "Error: Invalid PRINT_TRACK parameters.\n";

    else if (cmd == "PRINT_TRACK")
    {
        std::string destStr;
        if (!(iss >> destStr))
        {
            std::cout << "Error: Invalid PRINT_TRACK parameters.\n";
            return;
        }

        Destination dest;
        try { dest = parseDestination(destStr); }
        catch (...) {
            std::cout << "Error: Invalid PRINT_TRACK parameters.\n";
            return;
        }

        departureYard[static_cast<int>(dest)].printTrack();
    }

    // if AUTO_DISPATCH <ON/OFF>
    // Enable or disable automatic dispatch when weight exceeds limits.
    // std::cout << "Error: Invalid AUTO_DISPATCH parameters.\n";
    // print "Auto dispatch "enabled" / "disabled"

    else if (cmd == "AUTO_DISPATCH")
    {
        std::string mode;
        if (!(iss >> mode))
        {
            std::cout << "Error: Invalid AUTO_DISPATCH parameters.\n";
            return;
        }

        std::transform(mode.begin(), mode.end(), mode.begin(), ::tolower);
        if (mode == "on")
        {
            TrainTrack::autoDispatch = true;
            std::cout << "Auto dispatch enabled\n";
        }
        else if (mode == "off")
        {
            TrainTrack::autoDispatch = false;
            std::cout << "Auto dispatch disabled\n";
        }
        else
        {
            std::cout << "Error: Invalid AUTO_DISPATCH parameters.\n";
        }
    }

    // if CLEAR
    // Completely reset the system (yard + departure tracks).
    // std::cout << "System cleared." << std::endl;

    else if (cmd == "CLEAR")
    {
        classificationYard.clear();

        for (int i = 0; i < NUM_DESTINATIONS_INT; ++i) {
            while (true) {
                Train *t = departureYard[i].departTrain();
                if (!t) break;
                delete t;
            }
        }

        std::cout << "System cleared." << std::endl;
    }

    // else std::cout << "Error: Unknown command '" << cmd << "'" << std::endl;

    else
    {
        std::cout << "Error: Unknown command '" << cmd << "'" << std::endl;
    }

}

void RailMarshal::dispatchFromTrack(Destination track)
{
    // TODO: Dispatch the next train (frontmost) from the specified track.
    // std::cout << "Error: No trains to dispatch from Track " << destIndex << ".\n";
    /*std::cout << "Train " << t->getName()
              << " departed from Track " << destIndex
              << " (" << destinationToString(static_cast<Destination>(destIndex)) << ").\n";
     */

    int destIndex = static_cast<int>(track);
    Train *t = departureYard[destIndex].departTrain();

    if (!t)
    {
        std::cout << "Error: No trains to dispatch from track "
                  << destinationToString(track) << ".\n";
        return;
    }

    std::cout << "Dispatching " << t->getName()
              << " (" << t->getTotalWeight() << " tons)." << std::endl;

    delete t;

}

void RailMarshal::printDepartureYard() const
{
    for (int i = 0; i < NUM_DESTINATIONS_INT; ++i)
    {
        std::cout << "Track " << i << " ("
                  << destinationToString(static_cast<Destination>(i)) << "):\n";
        departureYard[i].printTrack();
    }
}

// Debug helper functions
void RailMarshal::printStatus() const
{
    std::cout << "--- classification Yard ---\n";
    classificationYard.print();

    std::cout << "--- Departure Yard ---\n";
    for (int i = 0; i < static_cast<int>(Destination::NUM_DESTINATIONS); ++i)
    {
        departureYard[i].printTrack();
    }
}
