#include "Team.h"

Team::Team()
    : teamID(-1),
      maxLoadCapacity(0),
      currentWorkload(0),
      missionStack(4) {
}

Team::Team(int id, int maxLoad)
    : teamID(id),
      maxLoadCapacity(maxLoad),
      currentWorkload(0),
      missionStack(4) {
}

int Team::getId() const {
    return teamID;
}

int Team::getMaxLoadCapacity() const {
    return maxLoadCapacity;
}

int Team::getCurrentWorkload() const {
    return currentWorkload;
}

void Team::setId(int id) {
    teamID = id;
}

void Team::setMaxLoadCapacity(int maxLoad) {
    maxLoadCapacity = maxLoad;
}

bool Team::hasActiveMission() const {
    return !missionStack.isEmpty();
}

bool Team::tryAssignRequest(const Request& req) {
    //Implement tryAssignRequest function as explained in the PDF.

    //------
    int addedLoad = 0;

    if (req.getType() == "SUPPLY") {
        addedLoad = req.getAmount();
    }
    else if (req.getType() == "RESCUE") {
        int multiplier = 1;
        std::string risk = req.getRisk();

        if (risk == "MEDIUM")
            multiplier = 2;
        else if (risk == "HIGH")
            multiplier = 3;
        // if (LOW) multiplier = 1 

        addedLoad = req.getNumPeople() * multiplier;
    }
    else {
        return false;
    }


    if (currentWorkload + addedLoad > maxLoadCapacity) {
        // mesaj çıkcak mı?
        return false;
    }

    missionStack.push(req);
    currentWorkload += addedLoad;
    return true;
}

void Team::rollbackMission(RequestQueue& supplyQueue, RequestQueue& rescueQueue) {
    //Implement rollbackMission function as explained in the PDF.

    //------
    if (missionStack.isEmpty()) {
        currentWorkload = 0;
        return;
    }

    int n = missionStack.size();
    Request* tmp = new Request[n];

    int idx = 0;
    Request r;
    while (!missionStack.isEmpty()) {
        missionStack.pop(r);
        tmp[idx++] = r;
    }

    currentWorkload = 0;

    int oldSupplySize = supplyQueue.size();
    Request* oldSupply = new Request[oldSupplySize];
    for (int i = 0; i < oldSupplySize; ++i) {
        supplyQueue.dequeue(oldSupply[i]);
    }

    int oldRescueSize = rescueQueue.size();
    Request* oldRescue = new Request[oldRescueSize];
    for (int i = 0; i < oldRescueSize; ++i) {
        rescueQueue.dequeue(oldRescue[i]);
    }

    for (int i = n - 1; i >= 0; --i) {
        const Request& rq = tmp[i];
        if (rq.getType() == "SUPPLY") {
            supplyQueue.enqueue(rq);
        } else if (rq.getType() == "RESCUE") {
            rescueQueue.enqueue(rq);
        }
    }

    for (int i = 0; i < oldSupplySize; ++i) {
        supplyQueue.enqueue(oldSupply[i]);
    }
    for (int i = 0; i < oldRescueSize; ++i) {
        rescueQueue.enqueue(oldRescue[i]);
    }

    delete[] tmp;
    delete[] oldSupply;
    delete[] oldRescue;
}

void Team::clearMission() {
    missionStack.clear();
    currentWorkload = 0;
}

const MissionStack& Team::getMissionStack() const {
    return missionStack;
}
