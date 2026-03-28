
#include "ArtifactManager.h"
#include <iostream>
#include <sstream>

ArtifactManager::ArtifactManager()
{
}

ArtifactManager::~ArtifactManager()
{
}

int ArtifactManager::tokenize(const std::string &line, std::string tokens[], int maxTokens) const
{
    std::istringstream iss(line);
    std::string tok;
    int count = 0;
    while (iss >> tok && count < maxTokens)
    {
        tokens[count++] = tok;
    }
    return count;
}

void ArtifactManager::parseAndExecute(const std::string &line)
{
    // TODO: read lines and execuıte each command
    // Print "Error: Unknown command" if command is not known

    if (line.empty()) return;
    
    std::string tokens[10]; 
    int count = tokenize(line, tokens, 10);
    if (count == 0) return;

    std::string cmd = tokens[0];

    if (cmd == "ADD_ARTIFACT") handleAddArtifact(tokens, count);
    else if (cmd == "REMOVE_ARTIFACT") handleRemoveArtifact(tokens, count);
    else if (cmd == "HIRE_RESEARCHER") handleHireResearcher(tokens, count);
    else if (cmd == "FIRE_RESEARCHER") handleFireResearcher(tokens, count);
    else if (cmd == "REQUEST") handleRequest(tokens, count);
    else if (cmd == "RETURN") handleReturn(tokens, count);
    else if (cmd == "RETURN_ALL") handleReturnAll(tokens, count);
    else if (cmd == "RESEARCHER_LOAD") handleResearcherLoad(tokens, count);
    else if (cmd == "MATCH_RARITY") handleMatchRarity(tokens, count);
    else if (cmd == "PRINT_UNASSIGNED") handlePrintUnassigned(tokens, count);
    else if (cmd == "PRINT_STATS") handlePrintStats(tokens, count);
    else if (cmd == "CLEAR") handleClear(tokens, count);
    else std::cout << "Error: Unknown command '" << cmd << "'." << std::endl;

}

// =================== COMMAND HANDLERS ===================

void ArtifactManager::handleAddArtifact(const std::string tokens[], int count)
{
    // Expected: ADD_ARTIFACT <id> <name> <rarity> <value>
    // TODO:
    // 1) Check parameter count.
    // 2) Convert <id>, <rarity>, <value> to integers.
    // 3) Create Artifact and attempt to insert into AVL tree.
    // 4) On success: print "Artifact <id> added."
    // 5) On duplicate: print appropriate error (as in the PDF).

    if (count != 5) return; 
    int id = std::stoi(tokens[1]);
    std::string name = tokens[2];
    int rarity = std::stoi(tokens[3]);
    int value = std::stoi(tokens[4]);

    Artifact a(id, name, rarity, value);
    if (artifactTree.insertArtifact(a)) {
        std::cout << "Artifact " << id << " added." << std::endl;
    } else {
        std::cout << "Error: Artifact " << id << " already exists." << std::endl;
    }
}

void ArtifactManager::handleRemoveArtifact(const std::string tokens[], int count)
{
    // Expected: REMOVE_ARTIFACT <id>
    // TODO:
    // 1) Parse id.
    // 2) Find artifact in AVL; if not found, print error.
    // 3) If artifact is assigned , find researcher and
    //    remove artifact from their list.
    // 4) Remove artifact from AVL; print success or error message.

    int id = std::stoi(tokens[1]);
    ArtifactNode* node = artifactTree.findArtifact(id);
    if (!node) {
        std::cout << "Error: Artifact " << id << " not found." << std::endl;
        return;
    }
    
    if (!node->data.assignedToName.empty()) {
        ResearcherNode* rNode = researcherTree.findResearcher(node->data.assignedToName);
        if (rNode) {
            rNode->data.removeArtifact(id);
        }
    }

    if (artifactTree.removeArtifact(id)) {
        std::cout << "Artifact " << id << " removed." << std::endl;
    }
}

void ArtifactManager::handleHireResearcher(const std::string tokens[], int count)
{
    // Expected: HIRE_RESEARCHER <name> <capacity>
    // TODO:
    // 1) Parse name and capacity.
    // 2) Create Researcher and insert into RedBlackTree.
    // 3) On success: "Researcher <name> hired."
    // 4) On duplicate: error message.
    std::string name = tokens[1];
    int cap = std::stoi(tokens[2]);
    Researcher r(name, cap);
    if (researcherTree.insertResearcher(r)) {
        std::cout << "Researcher " << name << " hired." << std::endl;
    } else {
        // err??
    }
}

void ArtifactManager::handleFireResearcher(const std::string tokens[], int count)
{
    // Expected: FIRE_RESEARCHER <name>
    // TODO:
    // 1) Find researcher by name. If not found, print error.
    // 2) For each artifact ID in their assignment list:
    //      - clear assignedToName in AVL.
    // 3) Remove researcher from RBT.
    // 4) Print success message.

    std::string name = tokens[1];
    ResearcherNode* rNode = researcherTree.findResearcher(name);
    if (!rNode) {
        return;
    }
    
    for (int i=0; i < rNode->data.numAssigned; i++) {
        int artID = rNode->data.assignedArtifacts[i];
        artifactTree.clearAssignedTo(artID);
    }
    
    researcherTree.removeResearcher(name);
    std::cout << "Researcher " << name << " fired." << std::endl;
}

void ArtifactManager::handleRequest(const std::string tokens[], int count)
{
    // Expected: REQUEST <researcherName> <artifactID>
    // TODO:
    // 1) Find researcher by name; error if missing.
    // 2) Find artifact by ID; error if missing.
    // 3) Check artifact is unassigned; error if already assigned.
    // 4) Check researcher capacity; error if at full capacity.
    // 5) On success: add artifact to researcher list AND set assignedToName in AVL.
    //    Print "Artifact <id> assigned to <name>."

    std::string rName = tokens[1];
    int artID = std::stoi(tokens[2]);

    ResearcherNode* rNode = researcherTree.findResearcher(rName);
    ArtifactNode* aNode = artifactTree.findArtifact(artID);

    if (!rNode) {
        std::cout << "Error: Researcher " << rName << " not found." << std::endl;
        return;
    }
    if (!aNode) {
        std::cout << "Error: Artifact " << artID << " not found." << std::endl;
        return;
    }
    if (!aNode->data.assignedToName.empty()) {
        std::cout << "Error: Artifact " << artID << " is already assigned." << std::endl;
        return;
    }
    if (rNode->data.numAssigned >= rNode->data.capacity) {
        std::cout << "Error: Researcher " << rName << " is at full capacity." << std::endl;
        return;
    }

    rNode->data.addArtifact(artID);
    artifactTree.setAssignedTo(artID, rName);
    aNode->data.updateValueBasedOnUsage();

    std::cout << "Artifact " << artID << " assigned to " << rName << "." << std::endl;
}

void ArtifactManager::handleReturn(const std::string tokens[], int count)
{
    // Expected: RETURN <researcherName> <artifactID>
    // TODO:
    // 1) Validate existence of researcher and artifact.
    // 2) Check that artifact.assignedToName == researcherName.
    // 3) If not, print error.
    // 4) Otherwise, remove artifact from researcher list, clear assignedToName in AVL.
    //    Print "Artifact <id> returned by <name>."

    std::string rName = tokens[1];
    int artID = std::stoi(tokens[2]);

    ResearcherNode* rNode = researcherTree.findResearcher(rName);
    ArtifactNode* aNode = artifactTree.findArtifact(artID);

    if (!rNode || !aNode) return; // err?
    
    if (aNode->data.assignedToName != rName) {
         // err?
         return;
    }

    rNode->data.removeArtifact(artID);
    artifactTree.clearAssignedTo(artID);
    std::cout << "Artifact " << artID << " returned by " << rName << "." << std::endl;
}

void ArtifactManager::handleReturnAll(const std::string tokens[], int count)
{
    // Expected: RETURN_ALL <researcherName>
    // TODO:
    // 1) Find researcher; error if missing.
    // 2) For each artifact they supervise, clear assignedToName in AVL.
    // 3) Clear researcher's assignment list (removeAllArtifacts()).
    // 4) Print appropriate confirmation message.

    std::string rName = tokens[1];
    ResearcherNode* rNode = researcherTree.findResearcher(rName);
    if (!rNode) {
        std::cout << "Error: Researcher " << rName << " not found." << std::endl;
        return;
    }

    for (int i=0; i<rNode->data.numAssigned; i++) {
        artifactTree.clearAssignedTo(rNode->data.assignedArtifacts[i]);
    }
    rNode->data.removeAllArtifacts();
    std::cout << "All artifacts returned by " << rName << "." << std::endl;
}

void ArtifactManager::handleResearcherLoad(const std::string tokens[], int count)
{
    // Expected: RESEARCHER_LOAD <name>
    // TODO:
    // 1) Find researcher by name.
    // 2) If not found, print error.
    // 3) Otherwise, print number of supervised artifacts in required format.

    std::string name = tokens[1];
    ResearcherNode* rNode = researcherTree.findResearcher(name);
    if(rNode) {
        std::cout << rNode->data.numAssigned << std::endl; // ??
    } else {
        std::cout << "Error: Researcher " << name << " not found." << std::endl;
    }
}

void ArtifactManager::handleMatchRarity(const std::string tokens[], int count)
{
    // Expected: MATCH_RARITY <minRarity>
    // TODO:
    // Traverse AVL tree and print all artifacts with rarity >= minRarity.
    // You may choose any reasonable order (probably inorder) unless specified otherwise
    // in your PDF. Artifacts may be assigned or unassigned; print as required.

    int minRarity = std::stoi(tokens[1]);
    std::cout << "=== MATCH_RARITY " << minRarity << " ===" << std::endl; // Başlık eklendi
    artifactTree.printMatchRarity(minRarity);
}

void ArtifactManager::handlePrintUnassigned(const std::string tokens[], int count)
{
    // Expected: PRINT_UNASSIGNED
    // TODO:
    // Print a header if needed, then call artifactTree.printUnassigned().
    std::cout << "Unassigned artifacts:" << std::endl;
    artifactTree.printUnassigned();

}

void ArtifactManager::handlePrintStats(const std::string tokens[], int count)
{
    // Expected: PRINT_STATS
    // TODO:
    // 1) Compute:
    //    - totalArtifacts (artifactTree.getArtifactCount())
    //    - totalResearchers (researcherTree.getResearcherCount())
    //    - average artifact rarity (floor of totalRarity / totalArtifacts)
    //    - average researcher load (floor of totalLoad / totalResearchers)
    // 2) Print summary lines exactly as in the spec.
    // 3) Then:
    //    - Print researchers using preorder traversal:
    //      researcherTree.traversePreOrderForStats()
    //    - Print artifacts using postorder traversal:
    //      artifactTree.traversePostOrderForStats()
    //    (Exact formatting defined in your PDF.)

    int totalArt = artifactTree.getArtifactCount();
    int totalRes = researcherTree.getResearcherCount();
    int totalRarity = artifactTree.getTotalRarity();
    int totalLoad = researcherTree.getTotalLoad();
    
    int avgRarity = (totalArt == 0) ? 0 : totalRarity / totalArt;
    int avgLoad = (totalRes == 0) ? 0 : totalLoad / totalRes;

    std::cout << "=== SYSTEM STATISTICS ===" << std::endl; // Başlık
    std::cout << "Artifacts: " << totalArt << std::endl;
    std::cout << "Researchers: " << totalRes << std::endl;
    std::cout << "Average rarity: " << avgRarity << std::endl;
    std::cout << "Average load: " << avgLoad << std::endl;
    
    std::cout << "Researchers:" << std::endl;
    researcherTree.traversePreOrderForStats();
    
    std::cout << "Artifacts:" << std::endl;
    artifactTree.traversePostOrderForStats();

}

void ArtifactManager::handleClear(const std::string tokens[], int count)
{
    // Expected: CLEAR
    // TODO:
    // Clear both trees and print confirmation message.
    artifactTree.clear();
    researcherTree.clear();
    std::cout << "All data cleared." << std::endl;
}
