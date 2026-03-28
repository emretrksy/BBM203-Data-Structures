#include "QuakeAssistController.h"
#include <iostream>
#include <sstream>

QuakeAssistController::QuakeAssistController()
    : teams(nullptr),
      teamCount(0),
      supplyQueue(4),
      rescueQueue(4) {
}

QuakeAssistController::~QuakeAssistController() {
    delete[] teams;
}

bool QuakeAssistController::parseAndExecute(const std::string& line) {
    //Read the input file line by line and execute realtime.

    //------

    if (line.empty() || line[0] == '#')
        return true;

    std::istringstream iss(line);
    std::string cmd;
    iss >> cmd;

    if (cmd.empty())
        return true;

    // INIT_TEAMS <numTeams> O
    if (cmd == "INIT_TEAMS") {
        int numTeams;
        if (!(iss >> numTeams)) {
            std::cout << "Error: Invalid INIT_TEAMS parameters.\n";
            return true;
        }
        initializeTeams(numTeams);
        return true;
    }

    // SET_TEAM_CAPACITY <teamId> <maxLoadCapacity> O
    if (cmd == "SET_TEAM_CAPACITY") {
        int teamId, capacity;
        if (!(iss >> teamId >> capacity)) {
            std::cout << "Error: Invalid SET_TEAM_CAPACITY parameters.\n";
            return true;
        }
        handleSetTeamCapacity(teamId, capacity);
        return true;
    }

    // ADD_SUPPLY <id> <city> <supplyType> <amount> <emergencyLevel> O
    if (cmd == "ADD_SUPPLY") {
        std::string id, cityStr, supplyTypeStr;
        int amount, emergencyLevel;
        if (!(iss >> id >> cityStr >> supplyTypeStr >> amount >> emergencyLevel)) {
            std::cout << "Error: Invalid ADD_SUPPLY parameters.\n";
            return true;
        }
        handleAddSupply(id, cityStr, supplyTypeStr, amount, emergencyLevel);
        return true;
    }

    // ADD_RESCUE <id> <city> <numPeople> <risk> <emergencyLevel> O
    if (cmd == "ADD_RESCUE") {
        std::string id, cityStr, riskStr;
        int numPeople, emergencyLevel;
        if (!(iss >> id >> cityStr >> numPeople >> riskStr >> emergencyLevel)) {
            std::cout << "Error: Invalid ADD_RESCUE parameters.\n";
            return true;
        }
        handleAddRescue(id, cityStr, numPeople, riskStr, emergencyLevel);
        return true;
    }

    // REMOVE_REQUEST <id> O
    if (cmd == "REMOVE_REQUEST") {
        std::string id;
        if (!(iss >> id)) {
            std::cout << "Error: Invalid REMOVE_REQUEST parameters.\n";
            return true;
        }
        handleRemoveRequest(id);
        return true;
    }

    // HANDLE_EMERGENCY <teamId> <k> O
    if (cmd == "HANDLE_EMERGENCY") {
        int teamId, k;
        if (!(iss >> teamId >> k)) {
            std::cout << "Error: Invalid HANDLE_EMERGENCY parameters.\n";
            return true;
        }
        handleHandleEmergency(teamId, k);
        return true;
    }

    // DISPATCH_TEAM <teamId> O
    if (cmd == "DISPATCH_TEAM") {
        int teamId;
        if (!(iss >> teamId)) {
            std::cout << "Error: Invalid DISPATCH_TEAM parameters.\n";
            return true;
        }
        handleDispatchTeam(teamId);
        return true;
    }

    // PRINT_QUEUES O
    if (cmd == "PRINT_QUEUES") {
        printQueues();
        return true;
    }

    // PRINT_TEAM <teamId> O
    if (cmd == "PRINT_TEAM") {
        int teamId;
        if (!(iss >> teamId)) {
            std::cout << "Error: Invalid PRINT_TEAM parameters.\n";
            return true;
        }
        printTeam(teamId);
        return true;
    }

    // CLEAR O
    if (cmd == "CLEAR") {
        clear();
        return true;
    }

    // UNKNOWN
    std::cout << "Error: Unknown command '" << cmd << "'.\n";
    return true;
}

bool QuakeAssistController::initializeTeams(int numTeams) {

    //------
    delete[] teams;

    supplyQueue.clear();
    rescueQueue.clear();
    
    teams = new Team[numTeams];
    teamCount = numTeams;
    for (int i = 0; i < numTeams; ++i) {
        teams[i].setId(i);
        teams[i].clearMission();
    }
    std::cout << "Initialized " << numTeams << " teams.\n";
    return true;
}

int QuakeAssistController::findTeamIndexById(int teamId) const {

    if (teams == nullptr || teamCount == 0)
        return -1;

    for (int i = 0; i < teamCount; i++) {
        if (teams[i].getId() == teamId) {
            return i;
        }
    }

    return -1;
}

bool QuakeAssistController::handleSetTeamCapacity(int teamId, int capacity) {
    //Find the index of team in the array, update the capacity value of the team.

    //------
    int idx = findTeamIndexById(teamId);
    if (idx == -1) {
        std::cout << "Error: Team " << teamId << " not found." << std::endl;
        return true;
    }
    teams[idx].setMaxLoadCapacity(capacity);
    std::cout << "Team " << teamId << " capacity set to " << capacity << "." << std::endl;
    return true;
}

bool QuakeAssistController::handleAddSupply(const std::string& id,
                                            const std::string& cityStr,
                                            const std::string& supplyTypeStr,
                                            int amount,
                                            int emergencyLevel) {

    //------
    Request req(id, cityStr, supplyTypeStr, amount, emergencyLevel);
    supplyQueue.enqueue(req);
    std::cout << "Request " << id << " added to SUPPLY queue." << std::endl;
    return true;
}

bool QuakeAssistController::handleAddRescue(const std::string& id,
                                            const std::string& cityStr,
                                            int numPeople,
                                            const std::string& riskStr,
                                            int emergencyLevel) {

    //------
    Request req(id, cityStr, numPeople, riskStr, emergencyLevel);
    rescueQueue.enqueue(req);
    std::cout << "Request " << id << " added to RESCUE queue." << std::endl;
    return true;
}

bool QuakeAssistController::handleRemoveRequest(const std::string& id) {
    //Remove request using request ID from request(SUPPLY, RESCUE) queue.

    //------
    bool removedSupply = supplyQueue.removeById(id);
    bool removedRescue = rescueQueue.removeById(id);

    if (!removedSupply && !removedRescue) {
        std::cout << "Error: Request " << id << " not found." << std::endl;
        return false;
    }

    std::cout << "Request " << id << " removed." << std::endl;
    return true;
}

bool QuakeAssistController::handleHandleEmergency(int teamId, int k) {
    // TODO: Implement:
    // 1) Find team by id.
    // 2) For up to k steps:
    //    - Look at front of supplyQueue and rescueQueue using peek().
    //    - Use Request::computeEmergencyScore() to decide which to take.
    //    - If both empty -> break.
    //    - Try teams[teamIdx].tryAssignRequest(chosenRequest).
    //       * If this returns false, print overload message and
    //         call teams[teamIdx].rollbackMission(supplyQueue, rescueQueue),
    //         then break.
    //       * Else, dequeue chosen request from its queue and continue.

    //------
    int idx = findTeamIndexById(teamId);
    if (idx == -1) {
        std::cout << "Error: Team " << teamId << " not found." << std::endl;
        return true;
    }

    Team& team = teams[idx];

    int assignedSupply = 0;
    int assignedRescue = 0;
    bool overloaded = false;

    for (int step = 0; step < k; ++step) {
        Request frontSupply, frontRescue;
        bool hasSupply = supplyQueue.peek(frontSupply);
        bool hasRescue = rescueQueue.peek(frontRescue);

        if (!hasSupply && !hasRescue)
            break;

        bool chooseSupply = false;

        if (hasSupply && hasRescue) {
            int sScore = frontSupply.computeEmergencyScore();
            int rScore = frontRescue.computeEmergencyScore();

            if (sScore > rScore)
                chooseSupply = true;
            else if (rScore > sScore)
                chooseSupply = false;
            else
                chooseSupply = false;
        } else if (hasSupply) {
            chooseSupply = true;
        } else {
            chooseSupply = false;
        }

        Request chosen = chooseSupply ? frontSupply : frontRescue;

        if (!team.tryAssignRequest(chosen)) {
            std::cout << "Overload on Team " << teamId
                      << ": rolling back mission." << std::endl;
            team.rollbackMission(supplyQueue, rescueQueue);
            overloaded = true;
            break;
        }

        Request dummy;
        if (chooseSupply) {
            supplyQueue.dequeue(dummy);
            ++assignedSupply;
        } else {
            rescueQueue.dequeue(dummy);
            ++assignedRescue;
        }
    }

    int totalAssigned = assignedSupply + assignedRescue;

    if (!overloaded && totalAssigned > 0) {
        std::cout << "Team " << teamId << " assigned "
                  << totalAssigned << " requests ("
                  << assignedSupply << " SUPPLY, "
                  << assignedRescue << " RESCUE), total workload "
                  << team.getCurrentWorkload() << "." << std::endl;
    }

    return true;
}

bool QuakeAssistController::handleDispatchTeam(int teamId) {
    int idx = findTeamIndexById(teamId);
    if (idx == -1) {
        std::cout << "Error: Team " << teamId << " not found." << std::endl;
        return true;
    }
    Team& t = teams[idx];
    if (!t.hasActiveMission()) {
        std::cout << "Error: Team " << teamId << " has no active mission." << std::endl;
        return true;
    }
    int workload = t.getCurrentWorkload();
    std::cout << "Team " << teamId << " dispatched with workload " << workload << "." << std::endl;
    t.clearMission();
    return true;
}

void QuakeAssistController::printQueues() const {
    //Print queues.

    //------

    std::cout << "SUPPLY QUEUE:\n";
    for (int i = 0, idx = supplyQueue.getFrontIndex();
         i < supplyQueue.size();
         ++i, idx = (idx + 1) % supplyQueue.getCapacity())
    {
        const Request& r = supplyQueue.getData()[idx];
        std::cout << r.getId()      << " "
                << r.getCity()    << " "
                << r.getSupplyType() << " "
                << r.getAmount()  << " "
                << r.getEmergencyLevel();
        std::cout << '\n';
    }

    std::cout << "RESCUE QUEUE:\n";
    for (int i = 0, idx = rescueQueue.getFrontIndex();
         i < rescueQueue.size();
         ++i, idx = (idx + 1) % rescueQueue.getCapacity())
    {
        const Request& r = rescueQueue.getData()[idx];
        std::cout << r.getId()      << " "
                << r.getCity()    << " "
                << r.getNumPeople() << " "
                << r.getRisk() << " "
                << r.getEmergencyLevel();
        std::cout << '\n';
    }
}

void QuakeAssistController::printTeam(int teamId) const {
    //Print team data using teamId.

    //------
    int idx = findTeamIndexById(teamId);
    if (idx == -1) {
        std::cout << "Error: Team " << teamId << " not found." << std::endl;
        return;
    }

    const Team& t = teams[idx];
    std::cout << "TEAM " << teamId << " MISSION STACK:\n";

    const MissionStack& ms = t.getMissionStack();

    for (int i = ms.getTopIndex(); i >= 0; --i) {
        const Request& r = ms.getData()[i];
        if (r.getType() == "SUPPLY") {
            std::cout << r.getId()      << " "
                    << r.getCity()    << " "
                    << r.getSupplyType() << " "
                    << r.getAmount()  << " "
                    << r.getEmergencyLevel();
        } else if (r.getType() == "RESCUE") {
            std::cout << r.getId()      << " "
                    << r.getCity()    << " "
                    << r.getNumPeople() << " "
                    << r.getRisk() << " "
                    << r.getEmergencyLevel();
        }
        std::cout << '\n';
    }

}

void QuakeAssistController::clear() {
    //Clear data.

    //------
    supplyQueue.clear();
    rescueQueue.clear();

    if (teams != nullptr) {
        for (int i = 0; i < teamCount; i++) {
            teams[i].clearMission();
            teams[i].setMaxLoadCapacity(0);
        }
    }

    std::cout << "System cleared.\n";
}
