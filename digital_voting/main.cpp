#include <iostream>
#include <string>
#include "sqlite3.h"
#include <cstdlib>
#include <vector>
#include <algorithm>


using namespace std;


struct VoterNode {
    string id;
    string name;
    string party;
    VoterNode* next;
};


VoterNode* head = nullptr;

void clearScreen() {
    #ifdef _WIN64
        system("cls");
    #else
        system("clear");
    #endif
}


bool executeSQL(sqlite3* db, string query) {
    char* errMsg = nullptr;
    if (sqlite3_exec(db, query.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        cout << "SQL Error: " << errMsg << endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}


void loadVoters(sqlite3* db) {
    string query = "SELECT id, name FROM voters;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        string id = (char*)sqlite3_column_text(stmt, 0);
        string name = (char*)sqlite3_column_text(stmt, 1);


        VoterNode* newNode = new VoterNode;
        newNode->id = id;
        newNode->name = name;
        newNode->party = "";
        newNode->next = head;
        head = newNode;
    }
    sqlite3_finalize(stmt);


    VoterNode* current = head;
    query = "SELECT id, party FROM votes;";
    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        string id = (char*)sqlite3_column_text(stmt, 0);
        string party = (char*)sqlite3_column_text(stmt, 1);


        current = head;
        while (current != nullptr) {
            if (current->id == id) {
                current->party = party;
                break;
            }
            current = current->next;
        }
    }
    sqlite3_finalize(stmt);
}

bool isVoterRegistered(string id) {
    VoterNode* current = head;
    while (current != nullptr) {
        if (current->id == id) {
            return true;
        }
        current = current->next;
    }
    return false;
}
bool hasVoted(string id) {

    VoterNode* current = head;
    while (current != nullptr) {
        if (current->id == id) {
            return !current->party.empty();
        }
        current = current->next;
    }
    return false;
}

void addVoter(sqlite3* db, string id, string name) {

    string checkQuery = "SELECT name, age FROM CITIZEN WHERE id = '" + id + "';";

sqlite3_stmt* stmt;
sqlite3_prepare_v2(db, checkQuery.c_str(), -1, &stmt, nullptr);

bool foundA,foundN = false;
string dbName;
int dbAge;

if (sqlite3_step(stmt) == SQLITE_ROW) {
    dbName = (char*)sqlite3_column_text(stmt, 0);
    dbAge = sqlite3_column_int(stmt, 1);


    cout << "Retrieved from DB: Name = " << dbName << ", Age = " << dbAge << endl;


    if (dbName == name ) {
        foundN = true;
        if(dbAge >= 18){
            foundA=true;

        }
    }

} else {

    cout << "No citizen found with ID: " << id << endl;
}

sqlite3_finalize(stmt);


if (foundA && foundN) {

    if (isVoterRegistered(id)) {
        cout << "You are already registered. Please proceed to voting." << endl;
        return;
    }

    VoterNode* newNode = new VoterNode;
    newNode->id = id;
    newNode->name = name;
    newNode->party = "";
    newNode->next = head;
    head = newNode;


    string query = "INSERT INTO voters (id, name) VALUES ('" + id + "', '" + name + "');";
    executeSQL(db, query);
    cout << "Voter added successfully!" << endl;

} else {
    if (foundN && dbAge<18){

        cout << "You must be at least 18 years old to register." << endl;}

}}

// add parties
void addParty(sqlite3* db) {
    clearScreen();
    string partyName;
    cout << "Enter the name of the new party: ";
    cin.ignore();
    getline(cin, partyName);

    string query = "INSERT INTO parties (name) VALUES ('" + partyName + "');";

    if (executeSQL(db, query)) {
        cout << "Party added successfully!" << endl;
    } else {
        cout << "Failed to add party. It may already exist." << endl;
    }
}
// load parties from the database
vector<string> getParties(sqlite3* db) {
    vector<string> parties;
    string query = "SELECT name FROM parties;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        string party = (char*)sqlite3_column_text(stmt, 0);
        parties.push_back(party);
    }
    sqlite3_finalize(stmt);
    return parties;
}


void showRegisteredVoters() {
    clearScreen();
    VoterNode* current = head;


    cout << "\n+----------------------------------------------------+" << endl;
    cout << "|            Registered Voters                        |" << endl;
    cout << "+------------------------------------------------------+" << endl;
    cout << "|       ID   |           Name           |    Party    |" << endl;
    cout << "+------------------------------------------------------+" << endl;


    while (current != nullptr) {

        cout << "| "
             << "  "  << current->id    // ID
             << "  | " << "\t" << current->name    // Name
             << "| " << "\t\t  " << (current->party.empty() ? "Not Voted Yet" : current->party)  // Party
             << " |" << endl;
        current = current->next;
    }


    cout << "+-----------------------------------------+" << endl;
}


void showVotedUsers() {
    clearScreen();
    VoterNode* current = head;


    cout << "\n+--------------------------------------------+" << endl;
    cout << "|           Voters Who Have Voted           |" << endl;
    cout << "+--------------------------------------------+" << endl;
    cout << "|   ID   |           Name           |    Party    |" << endl;
    cout << "+--------------------------------------------+" << endl;


    while (current != nullptr) {
        if (!current->party.empty()) {

            cout << "| " << current->id;


            for (int i = current->id.length(); i < 6; i++) {
                cout << " ";
            }

            cout << "| " << current->name;


            for (int i = current->name.length(); i < 20; i++) {
                cout << " ";
            }

            cout << "| " << current->party << " |" << endl;
        }
        current = current->next;
    }

    cout << "+--------------------------------------------+" << endl;
}



// Function to merge two sorted lists
VoterNode* mergeSortedLists(VoterNode* a, VoterNode* b) {
    if (!a) return b;
    if (!b) return a;

    if (a->name < b->name) {
        a->next = mergeSortedLists(a->next, b);
        return a;
    } else {
        b->next = mergeSortedLists(a, b->next);
        return b;
    }
}

// Function to perform merge sort on the linked list
VoterNode* mergeSort(VoterNode* head) {
    if (!head || !head->next) return head;

    VoterNode* slow = head;
    VoterNode* fast = head->next;

    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }

    VoterNode* mid = slow->next;
    slow->next = nullptr;

    VoterNode* left = mergeSort(head);
    VoterNode* right = mergeSort(mid);

    return mergeSortedLists(left, right);
}

void sortVotersByName() {
    head = mergeSort(head);
    cout << "Voters sorted successfully.\n";
}

void searchVoterById(string id) {
    clearScreen();

    // Step 1: Convert Linked List to Array
    vector<VoterNode*> voterArray;
    VoterNode* current = head;

    while (current != nullptr) {
        voterArray.push_back(current);
        current = current->next;
    }

    // Step 2: Sort the Array by ID (Ensure it's sorted for Binary Search)
    sort(voterArray.begin(), voterArray.end(), [](VoterNode* a, VoterNode* b) {
        return a->id < b->id;
    });

    // Step 3: Perform Binary Search
    int left = 0, right = voterArray.size() - 1;
    bool found = false;
    VoterNode* foundVoter = nullptr;

    while (left <= right) {
        int mid = left + (right - left) / 2;

        if (voterArray[mid]->id == id) {
            foundVoter = voterArray[mid];
            found = true;
            break;
        } else if (voterArray[mid]->id < id) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    // Step 4: Display Result
    if (found) {
        cout << "\n+-----------------------------------------------+" << endl;
        cout << "|           Voter Found by ID: " << id << "           |" << endl;
        cout << "+-----------------------------------------------+" << endl;
        cout << "|   ID   |           Name           |    Party    |" << endl;
        cout << "+-----------------------------------------------------+" << endl;
        cout << "| " << foundVoter->id;

        for (int i = foundVoter->id.length(); i < 6; i++) {
            cout << "  ";
        }

        cout << "| " << foundVoter->name;

        for (int i = foundVoter->name.length(); i < 20; i++) {
            cout << " ";
        }

        cout << "| " << (foundVoter->party.empty() ? "Not Voted Yet" : foundVoter->party) << " |" << endl;
        cout << "+-----------------------------------------------+" << endl;
    } else {
        cout << "\n+-----------------------------------------------+" << endl;
        cout << "|    Voter not found with ID: " << id << "       |" << endl;
        cout << "+-------------------------------------------------+" << endl;
    }
}



void deleteVoter(sqlite3* db, string id) {
    clearScreen();
    VoterNode* current = head;
    VoterNode* previous = nullptr;

    while (current != nullptr && current->id != id) {
        previous = current;
        current = current->next;
    }

    if (current != nullptr) {
        if (previous == nullptr) {
            head = current->next;
        } else {
            previous->next = current->next;
        }
        delete current;

        // Remove from SQLite database
        string query = "DELETE FROM voters WHERE id = '" + id + "';";
        executeSQL(db, query);
        executeSQL(db, "DELETE FROM votes WHERE id = '" + id + "';");
        cout << "Voter with ID " << id << " has been deleted." << endl;
    } else {
        cout << "Voter not found with ID: " << id << endl;
    }
}

void vote(sqlite3* db) {
    clearScreen();
    string id;
    cout << "Enter your ID: ";
    cin >> id;

    if (hasVoted(id)) {
        cout << "You have already voted.\n";
        return;
    }

    vector<string> parties = getParties(db);
    if (parties.empty()) {
        cout << "No parties available to vote for.\n";
        return;
    }

    cout << "Here are the available parties:\n";
    for (size_t i = 0; i < parties.size(); i++) {
        cout << "\t" << i + 1 << ". " << parties[i] << endl;
    }

    int choice;
    cout << "Enter your choice: ";
    cin >> choice;

    if (choice < 1 || choice > parties.size()) {
        cout << "Invalid selection.\n";
        return;
    }

    string selectedParty = parties[choice - 1];
    VoterNode* current = head;
    while (current != nullptr) {
        if (current->id == id) {
            current->party = selectedParty;
            string query = "INSERT INTO votes (id, party) VALUES ('" + id + "', '" + selectedParty + "');";
            executeSQL(db, query);
            cout << "Thank you for voting!\n";
            return;
        }
        current = current->next;
    }

    cout << "Voter not registered!\n";
}

void userMenu(sqlite3* db) {
    clearScreen();
    int choice;
    while (true) {
        cout << "\n+------------------------+" << endl;
        cout << "|      Voter Menu         |" << endl;
        cout << "+------------------------+" << endl;
        cout << "| 1. Register            |" << endl;
        cout << "| 2. Vote                |" << endl;
        cout << "| 3. Exit                |" << endl;
        cout << "+------------------------+" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == 1) {
            clearScreen();
            string id, name;
            cout << "Enter your ID: ";
            cin >> id;
            cout << "Enter your name: ";
            cin >> name;

            addVoter(db, id, name);
        } else if (choice == 2) {
            vote(db);
        } else if (choice == 3) {
            break;
        } else {
            cout << "Invalid choice, please try again.\n";
        }
    }
}



bool isStrongPassword(const string& password) {
    if (password.length() < 8) return false;

    bool hasLower = false, hasUpper = false, hasDigit = false, hasSpecial = false;
    for (char ch : password) {
        if (islower(ch)) hasLower = true;
        if (isupper(ch)) hasUpper = true;
        if (isdigit(ch)) hasDigit = true;
        if (ispunct(ch)) hasSpecial = true;
    }

    return hasLower && hasUpper && hasDigit && hasSpecial;
}



void changeAdminPassword(sqlite3* db) {
    string user_name, old_pass, new_pass;
    cout << "\n+-------------------------------------+" << endl;
    cout << "|     Admin Change Password          |" << endl;
    cout << "+-----------------------------------+" << endl;
    cout << "| Enter user name: ";
    cin >> user_name;
    cout << "| Enter current password: ";
    cin >> old_pass;
    cout << "| Enter new password: ";
    cin >> new_pass;
    cout << "+-----------------------------------+" << endl;

    string query = "SELECT password FROM ADMIN WHERE user_name = '" + user_name + "';";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        string current_pass = (char*)sqlite3_column_text(stmt, 0);
        if (old_pass == current_pass) {
            if (isStrongPassword(new_pass)) {
                string updateQuery = "UPDATE ADMIN SET password = '" + new_pass + "' WHERE user_name = '" + user_name + "';";
                if (executeSQL(db, updateQuery)) {
                    cout << "Password updated successfully!" << endl;
                } else {
                    cout << "Failed to update password." << endl;
                }
            } else {
                cout << "New password is not strong enough. Ensure it has at least 8 characters, with a mix of uppercase, lowercase, numbers, and special characters." << endl;
            }
        } else {
            cout << "Incorrect current password." << endl;
        }
    } else {
        cout << "Admin user not found!" << endl;
    }

    sqlite3_finalize(stmt);
}


void showWinner(sqlite3* db) {
    string query = "SELECT party, COUNT(*) FROM votes GROUP BY party ORDER BY COUNT(*) DESC;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);

    int maxVotes = 0;
    string winnerParty;
    bool tie = false;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        string party = (char*)sqlite3_column_text(stmt, 0);
        int votes = sqlite3_column_int(stmt, 1);

        tie = (votes == maxVotes);

        maxVotes = max(maxVotes, votes);

        if (votes == maxVotes) {
            winnerParty += (winnerParty.empty() ? "" : " & ") + party;
        }
    }

    sqlite3_finalize(stmt);  // Finalize the statement to avoid memory leaks

    if (winnerParty.empty()) {
        cout << "No votes cast yet.\n";
    } else if (tie) {
        cout << "There is a tie between the parties: " << winnerParty << " with " << maxVotes << " votes.\n";
    } else {
        cout << "The winner is: " << winnerParty << " with " << maxVotes << " votes.\n";
    }
}

void showVotingStatus(sqlite3* db) {
    clearScreen();

    string query = "SELECT party, COUNT(*) FROM votes GROUP BY party;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);

    cout << "\n+-------------------------------------------+" << endl;
    cout << "|            Voting Status                 |" << endl;
    cout << "+-------------------------------------------+" << endl;
    cout << "| Party             | Number of Voters      |" << endl;
    cout << "+-------------------------------------------+" << endl;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        string party = (char*)sqlite3_column_text(stmt, 0);
        int count = sqlite3_column_int(stmt, 1);
        cout << "| " << party;

        for (int i = party.length(); i < 18; i++) {
            cout << " ";
        }

        cout << "| " << count << "                    |" << endl;
    }

    sqlite3_finalize(stmt);
    cout << "+-------------------------------------------+" << endl;
}




void adminMenu(sqlite3* db) {
    clearScreen();

    string user_name, pass;
    cout << "\n+-------------------------------------+" << endl;
    cout << "|     Admin Authentication          |" << endl;
    cout << "+-----------------------------------+" << endl;
    cout << "| Enter user name: ";
    cin >> user_name;
    cout << "| Enter admin password: ";
    cin >> pass;
    cout << "+-----------------------------------+" << endl;

    string query = "SELECT user_name, password FROM ADMIN;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);

    bool authenticated = false;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        string name = (char*)sqlite3_column_text(stmt, 0);
        string key = (char*)sqlite3_column_text(stmt, 1);
        if (user_name == name && pass == key) {
            authenticated = true;
            break;
        }
    }

    sqlite3_finalize(stmt);

    if (!authenticated) {
        cout << "Incorrect username or password!" << endl;
        return;
    }

    int choice;
    while (true) {
        cout << "\n+------------------------------+" << endl;
        cout << "|     Admin Menu               |" << endl;
        cout << "+------------------------------+" << endl;
        cout << "| 1. Show registered voters    |" << endl;
        cout << "| 2. Show voters who have voted|" << endl;
        cout << "| 3. Sort by Name              |" << endl;
        cout << "| 4. Search by ID              |" << endl;
        cout << "| 5. Delete by ID              |" << endl;
        cout << "| 6. Show winner               |" << endl;
        cout << "| 7. Add Party                 |" << endl;
        cout << "| 8. Change Admin Password     |" << endl;  // Added option
        cout << "| 9. Show voting status        |" << endl;
        cout << "| 0. Exit                      |" << endl;
        cout << "+------------------------------+" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                showRegisteredVoters();
                break;
            case 2:
                showVotedUsers();
                break;
            case 3:
                sortVotersByName();
                break;
            case 4: {
                string id;
                cout << "Enter voter ID to search: ";
                cin >> id;
                searchVoterById(id);
                break;
            }
            case 5: {
                string id;
                cout << "Enter voter ID to delete: ";
                cin >> id;
                deleteVoter(db, id);
                break;
            }
            case 6:
                showWinner(db);
                break;
            case 7:
                addParty(db);
                break;
            case 8:
                changeAdminPassword(db);
                break;
            case 9:
              showVotingStatus(db);
                break;
            case 0:
                return;
            default:
                cout << "Invalid choice, please try again." << endl;
                break;
        }
    }
}


int main() {
    sqlite3* db;
    if (sqlite3_open("voting_system.db", &db) != SQLITE_OK) {
        cout << "Error opening database: " << sqlite3_errmsg(db) << endl;
        return 1;
    }

     string createCitizenTable = "CREATE TABLE IF NOT EXISTS CITIZEN ("
                                "id TEXT PRIMARY KEY, "
                                "name TEXT, "
                                "age INTEGER);";

    string insertCitizenData = "INSERT OR IGNORE INTO CITIZEN (id, name, age) VALUES "
                               "('111', 'AMAN', 40), "
                               "('122', 'YONAS', 20), "
                               "('112', 'BETEL', 20), "
                               "('334', 'HAILE', 10)";



    string createVoterTable = "CREATE TABLE IF NOT EXISTS voters (id TEXT PRIMARY KEY, name TEXT);";
     string createPartiesTable = "CREATE TABLE IF NOT EXISTS parties (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT UNIQUE);";
    string createVotesTable = "CREATE TABLE IF NOT EXISTS votes (id TEXT, party TEXT, FOREIGN KEY(id) REFERENCES voters(id));";
    string w="CREATE TABLE IF NOT EXISTS ADMIN (user_name TEXT PRIMARY KEY,password TEXT);";
    string y="INSERT OR IGNORE INTO ADMIN (user_name,password) VALUES ('HPR','H2P3R#')";
       executeSQL(db, w);
    executeSQL(db, y);

    executeSQL(db, createCitizenTable);
    executeSQL(db, insertCitizenData);
    executeSQL(db, createVoterTable);
    executeSQL(db, createVotesTable);
   executeSQL(db, createPartiesTable);

    executeSQL(db, w);
    executeSQL(db, y);

    loadVoters(db);


    int choice;
    while (true) {
    clearScreen();
      while (true) {
         cout<<"\n+-----------------------+\n";
        cout << "\n__Welcome to homepage____|" << endl;
        cout << "1. Voter Menu              |" << endl;
        cout << "2. Admin Menu              |" << endl;
        cout << "3. Exit                    |" << endl;
      cout<< "---------------------------+"<<endl;
      cout << "Enter your choice:-";
        cin >> choice;

        switch (choice) {
            case 1:
                userMenu(db);
                break;
            case 2:
                adminMenu(db);
                break;
            case 3:
                return 0;
            default:
                cout << "Invalid choice, please try again." << endl;
        }
    }
    }}
