#include <iostream>
#include <string>
#include "sqlite3.h"

using namespace std;

// Node structure for the linked list
struct VoterNode {
    string id;
    string name;
    string party;
    VoterNode* next;
};

VoterNode* head = NULL;

// SQLite Execution Helper Function
bool executeSQL(sqlite3* db, string query) {
    char* errMsg = NULL;
    if (sqlite3_exec(db, query.c_str(), NULL, NULL, &errMsg) != SQLITE_OK) {
        cout << "SQL Error: " << errMsg << endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

// Load all voters from the database into the linked list
void loadVoters(sqlite3* db) {
    string query = "SELECT id, name FROM voters;";  
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        string id = (char*)sqlite3_column_text(stmt, 0);
        string name = (char*)sqlite3_column_text(stmt, 1);

        // Create new voter node and add to the linked list
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
    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        string id = (char*)sqlite3_column_text(stmt, 0);
        string party = (char*)sqlite3_column_text(stmt, 1);

        current = head;
        while (current != NULL) {
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
    while (current != NULL) {
        if (current->id == id) {
            return true;
        }
        current = current->next;
    }
    return false;
}
bool hasVoted(string id) {
    VoterNode* current = head;
    while (current != NULL) {
        if (current->id == id) {
            return !current->party.empty();  
        }
        current = current->next;
    }
    return false;
}
// Add voter to linked list and database
void addVoter(sqlite3* db, string id, string name) {
    if (isVoterRegistered(id))
        {cout<<"you have aleady registered please proceed to voting";
     return;}
    VoterNode* newNode = new VoterNode;
    newNode->id = id;
    newNode->name = name;
    newNode->party = "";  
    newNode->next = head;
    head = newNode;

    // Add to SQLite database
    string query = "INSERT INTO voters (id, name) VALUES ('" + id + "', '" + name + "');";
    executeSQL(db, query);
    cout << "Voter added successfully!" << endl;
}

// Show all registered voters
void showRegisteredVoters() {
    VoterNode* current = head;

    cout << "\n+----------------------------------------------------+" << endl;
    cout << "|            Registered Voters                        |" << endl;
    cout << "+------------------------------------------------------+" << endl;
    cout << "|       ID   |           Name           |    Party    |" << endl;
    cout << "+------------------------------------------------------+" << endl;

    while (current != NULL) {
        cout << "| "
             << "  "  << current->id    
             << "  | " << "\t" << current->name   
             << "| " << "\t\t  " << (current->party.empty() ? "Not Voted Yet" : current->party)  // Party
             << " |" << endl;
        current = current->next;
    }

    cout << "+-----------------------------------------+" << endl;
}


void showVotedUsers() {
    VoterNode* current = head;

    cout << "\n+--------------------------------------------+" << endl;
    cout << "|           Voters Who Have Voted           |" << endl;
    cout << "+--------------------------------------------+" << endl;
    cout << "|   ID   |           Name           |    Party    |" << endl;
    cout << "+--------------------------------------------+" << endl;

    while (current != NULL) {
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


// Sort voters by name using bubble sort on the linked list
void sortVotersByName() {
    if (!head || !head->next) {
        return;  
    }

    bool swapped;
    do {
        swapped = false;
        VoterNode* current = head;
        while (current && current->next) {
            if (current->name > current->next->name) {
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

// Search for a voter by ID
void searchVoterById(string id) {
    VoterNode* current = head;
    bool found = false;

    // Search for the voter by ID
    while (current != NULL) {
        if (current->id == id) {
            cout << "\n+-----------------------------------------------+" << endl;
            cout << "|           Voter Found by ID: " << id << "           |" << endl;
            cout << "+-----------------------------------------------+" << endl;
            cout << "|   ID   |           Name           |    Party    |" << endl;
            cout << "+-----------------------------------------------------+" << endl;
            cout << "| " << current->id;

            // Manually space to align columns
            for (int i = current->id.length(); i < 6; i++) {
                cout << "  ";
            }

            cout << "| " << current->name;

            // Space for name column alignment
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


// Delete voter by ID (from both linked list and database)
void deleteVoter(sqlite3* db, string id) {
    VoterNode* current = head;
    VoterNode* previous = NULL;

    while (current != NULL && current->id != id) {
        previous = current;
        current = current->next;
    }

    if (current != NULL) {
        if (previous == NULL) {
            head = current->next;  // Delete the head node
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

// User registration and voting
void userMenu(sqlite3* db) {
    int choice, c;
    while (true) {
        cout << "\n+------------------------+" << endl;
        cout << "|      User Menu         |" << endl;
        cout << "+------------------------+" << endl;
        cout << "| 1. Register            |" << endl;
        cout << "| 2. Vote                |" << endl;
        cout << "| 3. Exit                |" << endl;
        cout << "+------------------------+" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == 1) {
            string id, name;
            cout << "Enter your ID: ";
            cin >> id;
            cout << "Enter your name: ";
            cin >> name;
            addVoter(db, id, name);
        } else if (choice == 2) {
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
            while (current != NULL) {
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
                sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

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

    // Create tables if they do not exist

    string createVoterTable = "CREATE TABLE IF NOT EXISTS voters (id TEXT PRIMARY KEY, name TEXT);";
    string createVotesTable = "CREATE TABLE IF NOT EXISTS votes (id TEXT, party TEXT, FOREIGN KEY(id) REFERENCES voters(id));";
    executeSQL(db, createVoterTable);
    executeSQL(db, createVotesTable);

    // Load voters from the database to the linked list
    loadVoters(db);

    // Main Menu
    int choice;
    while (true) {
      while (true) {
         cout<<"\n+-----------------------+\n";
        cout << "\n__Welcome to homepage____|" << endl;
        cout << "1. User Menu               |" << endl;
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
