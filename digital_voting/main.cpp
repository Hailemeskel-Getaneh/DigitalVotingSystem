#include <iostream>
#include "sqlite3.h"
#include <string>
#include <algorithm>

using namespace std;

struct VoterNode {
    string id;
    string name;
    VoterNode* next;
    VoterNode(const string& id, const string& name) : id(id), name(name), next(nullptr) {}
};

bool execute(sqlite3* db, const string& query) {
    char* errMsg = nullptr;
    if (sqlite3_exec(db, query.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        cerr << "SQL Error: " << errMsg << endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

void addVoter(VoterNode*& head, const string& id, const string& name) {
    VoterNode* newNode = new VoterNode(id, name);
    newNode->next = head;
    head = newNode;
}

bool voterExists(VoterNode* head, const string& id) {
    VoterNode* current = head;
    while (current != nullptr) {
        if (current->id == id) {
            return true;
        }
        current = current->next;
    }
    return false;
}

void registerVoter(sqlite3* db, VoterNode*& head, const string& id, const string& name) {
    if (voterExists(head, id)) {
        cout << "The user with this ID is already registered. Please proceed to voting.\n";
        return;
    }
    addVoter(head, id, name);
    string query = "INSERT INTO voters (id, name) VALUES ('" + id + "', '" + name + "');";
    if (execute(db, query)) {
        cout << "Registration successful!\n";
    }
}

bool hasVoted(sqlite3* db, const string& id) {
    string query = "SELECT COUNT(*) FROM votes WHERE id='" + id + "';";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    sqlite3_step(stmt);
    bool voted = sqlite3_column_int(stmt, 0) > 0;
    sqlite3_finalize(stmt);
    return voted;
}

void vote(sqlite3* db, VoterNode* head, const string& id, const string& party) {
    if (party != "Prosperity" && party != "Ezema" && party != "Enat") {
        cout << "Invalid party name. Please choose from Prosperity, Ezema, or Enat.\n";
        return;
    }
    if (!voterExists(head, id)) {
        cout << "You are not registered! Please register first.\n";
        return;
    }
    if (hasVoted(db, id)) {
        cout << "You have already voted. Thank you!\n";
        return;
    }
    string query = "INSERT INTO votes (id, party) VALUES ('" + id + "', '" + party + "');";
    if (execute(db, query)) {
        cout << "Thank you for voting!\n";
    }
}

void displayRegisteredUsers(sqlite3* db) {
    string query = "SELECT * FROM voters;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    cout << "ID -------- Name\n";
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        string id = (const char*)sqlite3_column_text(stmt, 0);
        string name = (const char*)sqlite3_column_text(stmt, 1);
        cout << id << " -------- " << name << "\n";
    }
    sqlite3_finalize(stmt);
}

void displayVotes(sqlite3* db) {
    string query = "SELECT party, COUNT(*) FROM votes GROUP BY party;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Error preparing statement: " << sqlite3_errmsg(db) << endl;
        return;
    }
    cout << "1 for prosperity, 2 for ezema , 3 for enat party";
    cout << "Party -------- Votes\n";
    bool hasResults = false;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        hasResults = true;
        string party = (const char*)sqlite3_column_text(stmt, 0);
        int votes = sqlite3_column_int(stmt, 1);
        cout << party << " -------- " << votes << "\n";
    }
    if (!hasResults) {
        cout << "No votes cast yet.\n";
    }
    sqlite3_finalize(stmt);
}

void showWinner(sqlite3* db) {
    string query = "SELECT party, COUNT(*) as count FROM votes GROUP BY party ORDER BY count DESC;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    string partyWithMaxVotes;
    int maxVotes = -1;
    bool tie = false;
    string tiedParties = "";
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        string party = (const char*)sqlite3_column_text(stmt, 0);
        int votes = sqlite3_column_int(stmt, 1);
        if (votes > maxVotes) {
            maxVotes = votes;
            partyWithMaxVotes = party;
            tie = false;
            tiedParties = party;
        } else if (votes == maxVotes) {
            tie = true;
            tiedParties += ", " + party;
        }
    }
    sqlite3_finalize(stmt);
    if (tie) {
        cout << "There is a tie between the following parties: " << tiedParties << "\n";
    } else {
        cout << "The winner is: " << partyWithMaxVotes << "\n";
    }
}

void voterMenu(sqlite3* db, VoterNode*& head) {
    int choice;
    while (true) {
        cout << "\nWelcome to the voting page\n";
        cout << "1. Register me\n";
        cout << "2. Already registered, let me vote\n";
        cout << "3. Back to main page\n";
        cout << "Type your choice number: ";
        cin >> choice;
        cin.ignore();
        switch (choice) {
            case 1: {
                string id, name;
                cout << "Enter your ID: ";
                getline(cin, id);
                cout << "Enter your name: ";
                getline(cin, name);
                registerVoter(db, head, id, name);
                break;
            }
            case 2: {
                string id, party;
                cout << "Enter your ID: ";
                getline(cin, id);
                cout << "Enter party (Prosperity, Ezema, Enat): ";
                getline(cin, party);
                vote(db, head, id, party);
                break;
            }
            case 3:
                return;
            default:
                cout << "Invalid number\n";
        }
    }
}

void adminMenu(sqlite3* db) {
    string password;
    cout << "Enter admin password: ";
    cin.ignore();
    getline(cin, password);
    if (password != "admin123") {
        cout << "Incorrect password.\n";
        return;
    }

    int choice;
    
    while (true) {
        cout << "\nAdmin Menu:\n";
        cout << "1. Show registered users\n";
        cout << "2. Show votes for each party\n";
        cout << "3. Show the winner\n";
        cout << "4. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
        switch (choice) {
            case 1:
                displayRegisteredUsers(db);
                break;
            case 2:
                displayVotes(db);
                break;
            case 3:
                showWinner(db);
                break;
            case 4:
                return;
            default:
                cout << "Invalid choice. Please try again.\n";
        }
    }
}

int main() {

    sqlite3* db;
    if (sqlite3_open("voting_system.db", &db) != SQLITE_OK) {
        cerr << "Error opening database: " << sqlite3_errmsg(db) << endl;
        return 1;
    }
    string createVoterTable = R"(
        CREATE TABLE IF NOT EXISTS voters (
            id TEXT PRIMARY KEY,
            name TEXT
        );
    )";
    string createVotesTable = R"(
        CREATE TABLE IF NOT EXISTS votes (
            id TEXT,
            party TEXT,
            FOREIGN KEY(id) REFERENCES voters(id)
        );
    )";
    execute(db, createVoterTable);
    execute(db, createVotesTable);
    VoterNode* head = nullptr;
    int choice;
    while (true) {
        cout << "\nWelcome! Who are you?\n";
        cout << "1. Voter\n";
        cout << "2. Admin\n";
        cout << "Enter your choice: ";
        cin >> choice;
        switch (choice) {
            case 1:
                voterMenu(db, head);
                break;
            case 2:
                adminMenu(db);
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
        }
        string cont;
        cout << "Do you want to continue? (yes/no): ";
        cin >> cont;
        if (cont != "yes") {
            cout << "Goodbye!\n";
            break;
        }
    }
    sqlite3_close(db);
    return 0;
}
