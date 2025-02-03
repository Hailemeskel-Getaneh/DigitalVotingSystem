
```

---

### digitalVotingSystem.md

```markdown
# DIGITAL VOTING SYSTEM

**Project Title:** DIGITAL VOTING SYSTEM  
**Course:** Data Structure and Algorithm

Welcome to the Digital Voting System project repository! This project is designed to simulate a simple yet secure digital voting system using a combination of linked lists and SQLite for data management. We are proud to present a forward-thinking approach to digital voting that emphasizes security, ease-of-use, and transparency.

## Table of Contents

- [Overview](#overview)
- [Project Structure](#project-structure)
- [Features](#features)
- [Requirements](#requirements)
- [Installation & Usage](#installation--usage)
- [Team Members](#team-members)
- [License](#license)

## Overview

This digital voting system allows users to register and cast their votes for one of three parties : **Prosperity**, **Ezema**, or **Enat**. It is possible to add parites and these are for sample. Administrators have exclusive access to review registered users, vote counts per party, and determine the winning party. The system uses SQLite for storing voter and vote data and employs linked lists for in-memory management of registered voters.

## Project Structure

```
digital_voting/
│
├── bin/
│   └── debug/
│       └── [Executable File]     # Run this file to start the application.
│
├── obj/
│   └── [Intermediate Build Files]
│
├── sqlite3/                      # SQLite database files used by the project.
│
└── main.cpp                      # Main source code file (comments removed for production).
```

## Features

- **Voter Registration:** Easily register as a voter using your unique ID.
- **Secure Voting:** Cast your vote for one of the available parties after successful registration.
- **Admin Controls:** Admins can view registered users, check vote counts, and determine the winner.
- **Data Integrity:** Utilizes SQLite to ensure that all vote and registration data is stored safely.
- **Simple CLI:** A command-line interface that guides users through registration, voting, and administrative tasks.

## Requirements

- **C++ Compiler:** Ensure you have a modern C++ compiler installed.
- **SQLite3 Library:** The project uses SQLite3 for database operations. Make sure the SQLite3 library is available on your system.
- **CMake/Make (Optional):** For building the project if you are not using an IDE.

## Installation & Usage

1. **Clone the Repository:**
    ```bash
    git clone https://github.com/Hailemeskel-Getaneh/DigitalVotingSystem.git
    cd digital_voting
    ```

2. **Build the Project:**
    - If you are using an IDE, open the project and build it.
    - For command-line:
      ```bash
      g++ main.cpp -lsqlite3 -o digital_voting_system
      ```

3. **Run the Application:**
    - Navigate to the `bin/debug` folder and run the executable:
      ```bash
      ./digital_voting_system
      ```
    - Follow the on-screen instructions to register as a voter, vote, or access the admin menu.
    -Even if it must be secured let me tell you admin password which is admin123. since it  helps you running the program.

## Team Members

| No | Name                    | ID            |
|----|-------------------------|---------------|
| 1  | Hailemeskel Getaneh     | DBU1501246    |
| 2  | Amanuale G/Egziabher    | DBU1501758    |
| 3  | Yonas Yirgu             | DBU1501579    |



---

```

---
