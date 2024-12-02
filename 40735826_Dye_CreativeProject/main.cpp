// DonovanDye_CreativeProject_CMPG121
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <memory>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <chrono>
#include <thread>
#include <tuple>
#include <numeric>
#include <math.h>
#include <random>


using namespace std;

string MonsterQ_Names[10];         // Array to hold monster names                                  #Queue
int MonsterQ_Stat[10][3];         // 2D array to hold monster stats (health, attack, probability)

string MonsterNames[100];        // Array to hold monster names                                   #Initial
int MonsterStat[100][3];        // 2D array to hold monster stats (health, attack, probability)

int monsterCount = 0;          // Global count of monsters
int monstersLeft;             // Amount of monsters left


// Template function to validate user input for both strings and integers
template <typename T>
T getValidInput(const vector<T>& expectedValues) {
    T input;
    bool valid = false;

    while (!valid) {
        if (cin >> input) {
            if (find(expectedValues.begin(), expectedValues.end(), input) != expectedValues.end()) {
                valid = true;
            }
            else {
                cout << "Invalid input. Try again." << endl;
            }
        }
        else {
            cout << "Invalid input type. Try again." << endl;
            cin.clear();
            cin.ignore();
        }
    }
    return input;
}


class Character {
private:
    string name;
    int health;
    int attack;
    int blockChance;  // Chance to block an attack
    int dodgeChance;  // Chance to dodge an attack
    bool isStunned;   // If the character is stunned it can't attack

public:
    // Constructor
    Character(string charName, int charHealth, int charAttack ,int charBlockChance = 26, int charDodgeChance= 14)
        : name(charName), health(charHealth), attack(charAttack), blockChance(charBlockChance), dodgeChance(charDodgeChance ), isStunned(false) {}

    // Getters
    string getName() const { return name; }
    int getAttack() const { return attack; }
    int getHealth() const { return health; }
    int getBlockChance() const { return blockChance; }
    int getDodgeChance() const { return dodgeChance; }
    bool getIsStunned() const { return isStunned; }

    // Setters
    void setName(string newName) { name = newName; }
    void setAttack(int newAttack) { attack = newAttack; }
    void setHealth(int newHealth) { health = newHealth; }
    void setBlockChance(int newBlockChance) { blockChance = newBlockChance; }
    void setDodgeChance(int newDodgeChance) { dodgeChance = newDodgeChance; }
    void setIsStunned(bool status) { isStunned = status; }

    // Method to display character info
    void displayCharacter() const {
        cout << "Name: " << name << endl;
        cout << "Health: " << health << endl;
        cout << "Attack: " << attack << endl;
        cout << "Block Chance: " << blockChance << "%" << endl;
        cout << "Dodge Chance: " << dodgeChance << "%" << endl;
    }

    // Method to calculate if character dodges an attack
    bool dodge() {
        int dodgeRoll = rand() % 100 + 1;
        return dodgeRoll <= dodgeChance;
    }

    // Method to calculate if character blocks an attack
    bool block() {
        int blockRoll = rand() % 100 + 1;
        return blockRoll <= blockChance;
    }

    // Method to attack another character
    void attackCharacter(Character& other) {
        if (isStunned) {
            cout << name << " is stunned and cannot attack!" << endl;
            return;
        }

        // Check if the other character dodges
        if (other.dodge()) {
            cout << other.getName() << " dodged the attack!"<<endl;
            return;
        }

        // Check if the other character blocks
        if (other.block()) {
            cout << other.getName() << " blocked the attack!" << endl;
            return;
        }

        // Simulate a dice roll for extra attack
        int diceRoll = rand() % 6 + 1;
        int totalAttack = attack + diceRoll;

        cout << name << " attacks " << other.getName() << " for " << totalAttack << " damage (including a dice roll of " << diceRoll << ")!" << endl;
        other.setHealth(other.getHealth() - totalAttack);


    }

    // Method to apply stunn
    void applyStatusEffect(string effect) {
        if (effect == "stun") {
            isStunned = true;
            cout << name << " is stunned and cannot attack!" << endl;
        }

    }
};


void clearScreen() {
    //Clear the screen
    system("CLS");
}

//Function to clear the File
void clearFile(const string& filename) {
    ofstream file(filename, std::ofstream::out | std::ofstream::trunc);
    file.close();  // Close the file after clearing it
}

//Get a random number between two numbers
int gRandNum(int min, int max) {
    return rand() % (max - min + 1) + min;
}

// Function to load monsters into global arrays
bool loadMonsters(const string& filename) {
    ifstream file(filename);  // Open the file


    // Check if the file was successfully opened
    if (!file.is_open()) {
        return false;  // Return false if the file could not be opened
    }

    monsterCount = 0;  // Reset the monster count
    string name;
    int health, attack, probability;

    // Read data from the file and load into the global arrays
    while (file >> name >> health >> attack >> probability) {
        if (monsterCount > 100) {
            break;  // Limit the number of monsters to 100
        }

        MonsterNames[monsterCount] = name;           // Store the monster's name
        MonsterStat[monsterCount][0] = health;       // Store health
        MonsterStat[monsterCount][1] = attack;       // Store attack
        MonsterStat[monsterCount][2] = probability;  // Store probability
        monsterCount++;  // Increase the count
    }

    file.close();  // Close the file
    return true;
}

// Function to calculate the total probability (sum of all monster weights)
int calculateTotalProbability() {
    int totalProbability = 1;
    for (int i = 0; i < monsterCount; i++) {
        totalProbability += MonsterStat[i][2];  // The probability is stored in the 3rd column
    }
    return totalProbability;
}

// Function to select a random monster based on its probability
int weightedRandomSelection(int totalProbability) {
    int temp = round(totalProbability + 1);
    int randomNumber = rand() % temp;  // Generate random number between 1 and totalProbability
    int cumulativeProbability = 0;

    // Find which monster corresponds to the random number
    for (int i = 0; i < monsterCount; i++) {
        cumulativeProbability += MonsterStat[i][2];  // Accumulate the probabilities
        if (randomNumber <= cumulativeProbability) {
            return i;  // Return the index of the selected monster
        }
    }

    return monsterCount - 1;  // Fallback, should not happen due to rounding
}
//Function to Populate the monters into a Queue by means of weighted randomization
void populateMonsterQueue() {
    int totalProbability = calculateTotalProbability();

    for (int i = 0; i < 10; i++) {
        int selectedIndex = weightedRandomSelection(totalProbability);

        // Copy the selected monster to the new queue arrays
        MonsterQ_Names[i] = MonsterNames[selectedIndex];
        MonsterQ_Stat[i][0] = MonsterStat[selectedIndex][0];  // Health
        MonsterQ_Stat[i][1] = MonsterStat[selectedIndex][1];  // Attack
        MonsterQ_Stat[i][2] = MonsterStat[selectedIndex][2];  // Probabilities
    }
}

void populateMonster(shared_ptr<Character> monster, string MonsterQ_Names[10], int MonsterQ_Stat[10][3], int i) {
    monster->setName(MonsterQ_Names[i]);                          // Set name
    monster->setHealth(MonsterQ_Stat[i][0]);                     // Set Health
    monster->setAttack(MonsterQ_Stat[i][1]);                    // SetAttack
    monster->setBlockChance(MonsterQ_Stat[i][2] * 10 / 20);    // Use probability for block chance (multiplied by 10)
    monster->setDodgeChance(MonsterQ_Stat[i][2] * 5 / 20);    // Use probability for dodge chance (multiplied by 5)
}

Character createCharacter(string charName,int charHealth, int charAttack) {
    Character newCharacter(charName, charHealth, charAttack);
    return newCharacter;
}

void displayFileContent(const string& filename) {ifstream file(filename);  // Open the file #FUN STUFF

    // Check if the file was successfully opened
    if (!file.is_open()) {
        cout << "Error: Could not open the file!" << endl;
        return;  // Exit the function, but the program continues
    }

    string line;
    // Read the file line by line and display the content
    while (getline(file, line)) {
        cout << line << endl;  // Display each line on the console
    }

    file.close();  // Close the file when done
}

tuple<string, int, int> loadSave(const string& filename) {
    ifstream file(filename);  // Open the file

    if (!file.is_open()) {
        // If file cannot be opened, return empty or default values
        return std::make_tuple("", 0, 0);
    }

    string playerName;
    int playerHealth, playerAttack;
    int health, attack, probability;

    // Reading the first two lines from the file
    file >> playerName >> playerHealth >> playerAttack;
    file >> monstersLeft;

    // Storing monster data starting from the third line
    int i = 0;
    while (i < 10 && file >> MonsterQ_Names[i] >> health >> attack >> probability) {
        MonsterQ_Stat[i][0] = health;       // Store monster health
        MonsterQ_Stat[i][1] = attack;       // Store monster attack
        MonsterQ_Stat[i][2] = probability;  // Store monster probability
        i++;
    }

    file.close();  // Close the file

    // Return the player's name, health, and attack as a tuple
    return std::make_tuple(playerName, playerHealth, playerAttack);
}
// Function to create a file and write data to it
void SavePlayerToFile(const string& filename, string MonsterQ_Names[10], int MonsterQ_Stat[10][3], const string& name, int health, int attack) {

    ofstream outFile(filename);

    if (!outFile.is_open()) {

        cout << "Error opening the file!" << endl;

    }
    else {

        outFile << name << " " << health << " " << attack << endl;

        outFile << monstersLeft << endl;

        for (int i = 0; i < 10; i++)
        {
            outFile << MonsterQ_Names[i] << " " << MonsterQ_Stat[i][0] << " " << MonsterQ_Stat[i][1] << " " << MonsterQ_Stat[i][2] << "\n";
        }
        outFile.close();
    }
}

// Function to edit a file and add a new monster
void AddMonster(const string& filename, const string& name, int health, int attack, int probability) {

    ofstream outFile(filename, std::ios_base::app);

    if (!outFile.is_open()) {

        cout << "Error opening the file!" << endl;

    }
    else {
        string temp = name + " " + to_string(health) + " " + to_string(attack) + " " + to_string(probability);
        outFile << temp;
        outFile << "";

    }
    outFile.close();
}


void WWUD() { //what will you do
    cout << "What will you do?" << endl;
    cout << "1. Attack" << endl;
    cout << "2. Drink Potion" << endl;
    cout << "3. Exit to MainMenu" << endl;
}

void combatSequence(Character& Player, int playerHealth, string MonsterQ_Names[], int MonsterQ_Stat[][3], int counter, vector<string>& validResponses, vector<int>& validNumbers) {
    int MaxHealth = Player.getHealth();
    shared_ptr<Character> monster = make_shared<Character>("", 0, 0);
    clearScreen();
    while (counter < 10 && Player.getHealth() > 0) {
        populateMonster(monster, MonsterQ_Names, MonsterQ_Stat, counter);
        clearScreen();
        cout << "=============================================================" << endl;
        cout << "Enemy No." << counter + 1 << ": " << monster->getName() << endl;
        cout << "Monster Health: " << monster->getHealth() << endl;
        cout << "=============================================================" << endl;

        // Combat loop for player vs monster
        while (monster->getHealth() > 0 && Player.getHealth() > 0) {

            WWUD();  // What will you do?
            int UserInput = getValidInput(validNumbers);

            switch (UserInput) {
            case 1:  // Player chooses to attack
                if (!Player.getIsStunned()) {  // If Player is not stunned
                    Player.attackCharacter(*monster);
                }
                else {
                    cout << Player.getName() << " is stunned and cannot attack this turn.\n";
                    Player.setIsStunned(false);  // Reset stunned status after one turn
                }

                // Monster retaliates if still alive
                if (monster->getHealth() > 0) {
                    int specialAttackRoll = rand() % 100 + 1;
                    if (specialAttackRoll <= 20) {  // 20% chance to stun player
                        cout << monster->getName() << " uses a stunning attack!" << endl;
                        Player.applyStatusEffect("stun");
                    }
                    monster->attackCharacter(Player);
                }
                break;

            case 2:  // Player chooses to heal
                Player.setHealth(MaxHealth);
                cout << "Your health has been restored to " << Player.getHealth() << endl;
                break;

            case 3:  // Player chooses to exit
                return;  // Exit the combat loop
            }
            cout << "=============================================================" << endl;
            // Check if player's health dropped to 0 or below after any action
            if (Player.getHealth() <= 0) {
                cout << "You have been defeated. Game over." << endl;
                displayFileContent("Death.txt");  // Display Death.txt when health <= 0
                cout << "Press any key to continue..." << endl;
                cin.get();  // Wait for the user to press any key
                cin.ignore();
                return;  // Exit the function as the player is dead
            }
        }

        // Monster defeated, check if player wants to continue
        if (monster->getHealth() <= 0) {
            cout << "Monster " << monster->getName() << " has been defeated!" << endl;
        }

        // Ask if the player wants to continue
        cout << "=============================================================" << endl;
        cout << "(Y) Continue to next monster?" << endl;
        cout << "(N) Save and Exit to MainMenu" << endl;
        cout << "Enter Y or N: ";
        string UserR = getValidInput(validResponses);
        monstersLeft--;

        if (UserR == "Y") {
            counter++;  // Move to the next monster
        }
        else {
            SavePlayerToFile("Game.txt", MonsterQ_Names, MonsterQ_Stat, Player.getName(), MaxHealth, Player.getAttack());
              // Exit the combat sequence
              return;
        }
    }

    // End of the combat sequence
    if (Player.getHealth() > 0) {
        cout << "=============================================================" << endl;
        cout << "Congratulations! You defeated all the monsters!" << endl;
        displayFileContent("Win.txt");
        cout << "Press any key to continue..." << endl;
        cin.get();  // Wait for the user to press any key
        cin.ignore();
    }
    else {
        // In case player's health dropped to 0 after monster defeated
        cout << "=============================================================" << endl;
        cout << "You have been defeated. Game over." << endl;
        displayFileContent("Death.txt");
        cout << "Press any key to continue..." << endl;
        cin.get();  // Wait for the user to press any key
        cin.ignore();

    }
}


void NewGame() {
    vector<string> validResponses = { "Y", "N" };
    vector<int> validNumbers = { 1, 2, 3, 4 };
    int counter = 0;  // Initialize counter for monsters
    string PlayerName;
    srand(time(0));

    monstersLeft = 10;

    clearScreen();
    cout << "Please enter a name: ";
    cin >> PlayerName;

    // Create the Player character
    Character Player = createCharacter(PlayerName, gRandNum(20, 30), gRandNum(3, 6));

    cout << "------------------------------------------------------" << endl;
    Player.displayCharacter();
    cout << "------------------------------------------------------" << endl;

     cout << "Would you like to begin? (Y/N): ";
    string UserResponse = getValidInput(validResponses);

    if (UserResponse == "N") {
        return;
    }

    clearScreen();
    cout << "-----Welcome to the dungeon-----" << endl;
    displayFileContent("FUN.txt");
    cout << "Press any key to continue..." << endl;
    cin.get();  // Wait for the user to press any key
    cin.ignore();


    // Load monsters
    loadMonsters("Monsters.txt");
    clearScreen();
    populateMonsterQueue();

    combatSequence(Player, Player.getHealth(), MonsterQ_Names, MonsterQ_Stat, counter, validResponses, validNumbers);




}


void LoadPrevGame() {
    // Load player data and monster data from the save file "Game.txt"
    tuple<string, int, int> playerData = loadSave("Game.txt");

    // Create a Character object for the player using the data from the tuple
    Character Player = createCharacter(get<0>(playerData), get<1>(playerData), get<2>(playerData));
    if ((Player.getName()) == "" && (Player.getAttack() == 0 && (Player.getAttack() == 0))){
        cout <<"No Save File Found, Try again after saving your Game" <<endl;
        cout << "Press any key to continue..." << endl;
        cin.get();
        cin.ignore();
        return;
    }

    vector<string> validResponses = { "Y", "N" };
    vector<int> validNumbers = { 1, 2, 3, 4 };

    // Retrieve the number of remaining monsters from the global variable monstersLeft
    int counter = (10-monstersLeft);
    clearScreen();
    cout << "---------------------Welcome Back---------------------" << endl;
    cout << "======================================================" << endl;
    Player.displayCharacter();  // Display the player's stats
    cout << "------------------------------------------------------" << endl;

    cout << "Would you like to begin? (Y/N): ";
    string UserResponse = getValidInput(validResponses);

    if (UserResponse == "N") {
        return;  // Exit if the player chooses not to continue
    }

    // Start the combat sequence with the loaded player and monsters
    combatSequence(Player, Player.getHealth(), MonsterQ_Names, MonsterQ_Stat, counter, validResponses, validNumbers);

}


void AddMonster_4() {
    vector<int> validNumbers = { 1, 2, 3, 4};
    vector<string> validTxt = { "Y" , "N" };
    vector<int> validNum(16);
    vector<int> validNum2(8);
    vector<int> validNum3(20);
    iota(validNum.begin(), validNum.end(), 5);
    iota(validNum2.begin(), validNum2.end(), 4);
    iota(validNum3.begin(), validNum3.end(), 1);
    string mName, mChoice;
    int mHealth, mAttack, mProbability, i;
    ofstream outFile("Monsters.txt", std::ios_base::app);


     clearScreen();
     cout << "Welcome to the Monster Creator where we help you to make new Monsters" << endl;
     cout << "=====================================================================" << endl;
     cout << "What would you like to do?" << endl;
     cout << "1. Create a new Monster" << endl;
     cout << "2. Restore Monsters to original bacup" << endl;
     cout << "3. Return to MainMenu" << endl;
     cout << "4. Exit " << endl;
     int Useri = getValidInput(validNumbers);
     cout << "=====================================================================" << endl;
     cout << " " << endl;

     switch (Useri)
     {
     case 1:
         cout << "Create a new monster" << endl;
         cout << "---------------------------------------------------------------------" << endl;
         cout << left << setw(20) << "Monster Name" << setw(15) << "Health" << setw(15) << "Attack" << setw(20) << "Probability" << endl;
         cout << "---------------------------------------------------------------------" << endl;
         cout << "Enter a name for your Monster: ";
         cin.ignore();
         getline(cin, mName);
         cout << "Enter the Monsters Health (5 - 20): " << endl;
         mHealth = getValidInput(validNum);
         cout << "Enter the Monsters Attack (4 - 12): " << endl;
         mAttack = getValidInput(validNum2);
         cout << "Enter the Monsters Probability (1 - 20): " << endl;
         mProbability = getValidInput(validNum3);
         cout << "---------------------------------------------------------------------" << endl;
         cout << "Here is your Monster it has been save in the MonsterFile" << endl;
         cout << left << setw(20) << mName << setw(15) << mHealth << setw(15) << mAttack << setw(20) << mProbability << endl;
         AddMonster("Monsters.txt", mName, mHealth, mAttack, mProbability);
         cout << "Press any key to continue..." << endl;
         cin.get();  // Wait for the user to press any key
         cin.ignore();
         AddMonster_4();
         break;

     case 2:
         cout << "Do you wish to restore the monster file back to it's original text?  (Y/N)" << endl;
         mChoice = getValidInput(validTxt);
         if (mChoice == "N") { return; }
         loadMonsters("Monsters - Backup.txt");

         clearFile("Monsters.txt");
         if (!outFile.is_open()) {

             cout << "Error opening the file!" << endl;

         }
         else {
             for (i = 0; i < monsterCount; i++)
             {
                 outFile << MonsterNames[i] << " " << MonsterStat[i][0] << " " << MonsterStat[i][1] << " " << MonsterStat[i][2] << "\n";
             }
             outFile.close();
             cout << "File has been restored" << endl;
         }
         cout << "Press any key to continue..." << endl;
         cin.get();  // Wait for the user to press any key
         cin.ignore();
         AddMonster_4();

         break;
     case 3:
         return;
         break;
     case 4:
         clearScreen();
         displayFileContent("End.txt");
         exit(0);
         break;
     }

}

int main()
{
    bool RUN = true;  // RUN starts as true

    do {

    vector<int> validNumbers = { 1, 2, 3, 4, 5 };
    clearScreen();
    cout << "Welcome to a text based fighting game" << endl;
    cout << "-------------------------------------" << endl;
    cout << "-------------------------------------" << endl;
    cout << " " << endl;
    cout << "What would you like to do?" << endl;
    cout << "-------------------------------------" << endl;
    cout << "1. Instructions" << endl;
    cout << "2. New Game" << endl;
    cout << "3. Load previous game" << endl;
    cout << "4. MONSTER MENU " << endl;
    cout << "5. Exit" << endl;
    cout << "-------------------------------------" << endl;





       int UserNum = getValidInput(validNumbers);

        switch (UserNum) {
        case 1:
            clearScreen();
            displayFileContent("Instructions.txt");         //Call Method to show instructions
            cout << "Press any key to continue..." << endl;
            cin.get();
            cin.ignore();
            break;
        case 2:
            NewGame();
            break;
        case 3:
            LoadPrevGame();
            break;
        case 4:
            AddMonster_4();
            break;
        case 5:
            clearScreen();
            displayFileContent("End.txt");
            RUN = false;
            exit(0);
            break;
        }

    } while (RUN);


}



