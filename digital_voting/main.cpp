#include <iostream>
#include <string>
#include "sqlite3.h"


using namespace std;


struct VoterNode {
    string id;
    string name;
    string party;
    VoterNode* next;
};


VoterNode* head = nullptr;

void clearScreen() {
    #ifdef _WIN32
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



void sortVotersByName() {
    clearScreen();
    if (!head || !head->next) {
        return;
    }

    bool swapped;
    do {
        swapped = false;
        VoterNode* current = head;
        while (current && current->next) {
            if (current->name > current->next->name) {
                // Swap the data, not the nodes themselves
                string tempId = current->id;
                string tempName = current->name;
                string tempParty = current->party;

                current->id = current->next->id;
                current->name = current->next->name;
                current->party = current->next->party;

                current->next->id = tempId;
                current->next->name = tempName;
                current->next->party = tempParty;

                swapped = true;
            }
            current = current->next;
        }
    } while (swapped);

    cout << "Voters Sorted by Name:" << endl;
    showRegisteredVoters();
}

void searchVoterById(string id) {
    clearScreen();
    VoterNode* current = head;
    bool found = false;


    while (current != nullptr) {
        if (current->id == id) {

            cout << "\n+-----------------------------------------------+" << endl;
            cout << "|           Voter Found by ID: " << id << "           |" << endl;
            cout << "+-----------------------------------------------+" << endl;
            cout << "|   ID   |           Name           |    Party    |" << endl;
            cout << "+-----------------------------------------------------+" << endl;
            cout << "| " << current->id;


            for (int i = current->id.length(); i < 6; i++) {
                cout << "  ";
            }

            cout << "| " << current->name;


            for (int i = current->name.length(); i < 20; i++) {
                cout << " ";
            }

            // Party column alignment
            cout << "| " << (current->party.empty() ? "Not Voted Yet" : current->party) << " |" << endl;
            cout << "+-----------------------------------------------+" << endl;

            found = true;
            break;
        }
        current = current->next;
    }

    if (!found) {
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

void userMenu(sqlite3* db) {
    clearScreen();
    int choice, c;
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
            clearScreen();
            string id, party;
            cout << "Enter your ID: ";
            cin >> id;
            cout << "Here are the parties:\n";
            cout << "\t1. Prosperity Party\n";
            cout << "\t2. Ezema Party\n";
            cout << "\t3. Enat Party\n";
            cout << "Enter your choice: ";
            cin >> c;

            if(c == 1) { party = "Prosperity"; }
            else if(c == 2) { party = "Ezema"; }
            else if(c == 3) { party = "Enat"; }
            else { cout << "Your input is invalid.\n"; return; }

            if(hasVoted(id)){
                cout << "You have already voted.\n";
                return;
            }

            VoterNode* current = head;
            while (current != nullptr) {
                if (current->id == id) {
                    current->party = party;
                    string query = "INSERT INTO votes (id, party) VALUES ('" + id + "', '" + party + "');";
                    executeSQL(db, query);
                    cout << "Thank you for voting!\n";
                    return;
                }
                current = current->next;
            }
            cout << "Voter not registered!\n";
        } else if (choice == 3) {
            break;
        } else {
            cout << "Invalid choice, please try again.\n";
        }

    }
}
void adminMenu(sqlite3* db) {
    clearScreen();
    string p;
    string key="one";
    cout<<"\nenter admin password to login\n";
    cin>>p;
    if(p!=key){
        cout<<"the password is not correct\n";
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
        cout << "| 7. Exit                      |" << endl;
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
            case 6: {
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

                sqlite3_finalize(stmt);

                if (winnerParty.empty()) {
                    cout << "No votes cast yet.\n";
                } else if (tie) {
                    cout << "There is a tie between the parties: " << winnerParty << " with " << maxVotes << " votes.\n";
                } else {
                    cout << "The winner is: " << winnerParty << " with " << maxVotes << " votes.\n";
                }
                break;
            }
            case 7:
                return;
            default:
                cout << "Invalid choice, please try again.\n";
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
                               "('122', 'YONI', 20), "
                               "('334', 'HAILE', 10);";



    string createVoterTable = "CREATE TABLE IF NOT EXISTS voters (id TEXT PRIMARY KEY, name TEXT);";
    string createVotesTable = "CREATE TABLE IF NOT EXISTS votes (id TEXT, party TEXT, FOREIGN KEY(id) REFERENCES voters(id));";
    executeSQL(db, createCitizenTable);
     executeSQL(db, insertCitizenData);
    executeSQL(db, createVoterTable);
    executeSQL(db, createVotesTable);


    loadVoters(db);


    int choice;
    while (true) {
    clearScreen();
      while (true) {
         cout<<"\n+-----------------------+\n";
        cout << "\n__Welcome to homepage____|" << endl;
        cout << "1. Voter Menu               |" << endl;
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
