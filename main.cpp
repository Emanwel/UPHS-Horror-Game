#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <iomanip>
using namespace std;

struct factor {int sanity; int fear;};

void update_time();
void Read(string, int, int);

class item {
    public:
        string name;
        int level;
        item() = default;
        item(string x, int y) {
            name = x;
            level = y;
        }
};

item noitem ("none", 0);

int gametime[] = {21, 0};
item inventory[5] = {noitem, noitem, noitem, noitem, noitem};
item &onHand = inventory[0];
factor player = {100, 0};

void update_time() {
    if (gametime[1] >= 60){
        gametime[0]++;
        gametime[1] -= 60;
    }

    if (gametime[0] >= 24) gametime[0] -= 24;
}

void Read(string x, int start, int end) {
    string buff;
    ifstream file(x);
    int current = 1;
    while (getline (file, buff)) {
        if (current >= start && current <= end) cout << buff << "\n";
        if (current > end) break;
        current++;
    }
    file.close();
}

class ghost {
    public:
        string name;
        int anger;
        int level;

        ghost() = default;
        ghost (string w, int y, int z) {
            name = w;
            anger = y;
            level = z;
        }
};

class location {
    public:
        string name;
        int pos;
        int creepiness;
        int linger;
        ghost entity;

        void creep(){
            linger++;
            player.fear += creepiness;
        }

        void haunt(){
            entity.anger++;
        }

        location() = default;
        location (string x, int y, int z, ghost w) {
            name = x;
            pos = y;
            creepiness = z;
            linger = 0;
            entity = w;
        }    
};

location current_loc;
bool gameOver = false;

void Update();
void Read();
void Start();
void Gameloop();

void Action(string buff);
void Move();
void Interact();
void Explore();
void CheckInv();
void Arrange();
void Use();
void Watch();
void Stats();
void Hold();


ghost melo ("Melody", 0, 5);
ghost dupe ("Doppelganger", 3, 1);
ghost burnt ("The Burnt Man", 5, 3);
ghost lady ("Lady in White", 3, 2);
ghost chain ("The Chained", 5, 5);
ghost forty ("41st Student", 10, 6);
ghost ghosts[]{melo, dupe, burnt, lady, chain, forty};

location sb ("Senior Highschool Building", 7, 5, chain);
location cl ("Chemistry Lab", 5, 3, dupe);
location fr ("Faculty Room", 3, 4, burnt);
location mr ("Music Room", 2, 5, melo);
location gh ("Guard House", 0, 1, lady);
location ccc ("Cebu Cultural Center", 10, 10, forty);
location locations[]{sb, cl, fr, mr, gh, ccc};

int main() {
    //start
    Read("start.txt", 1, 1);
    Start();
    while (!gameOver) Gameloop();
    return 0;
}

void Start() {
    string input;
    cout << ">> ";
    getline(std::cin, input);
    if (input != "START") Start();
    
    current_loc = gh;
}

void Gameloop() {
    string buff;
    cout << "\n\nEnter Action\n>> ";
    getline(std::cin, buff);
    Action(buff);
}

void Update() {
    update_time();
    current_loc.creep();
    if (current_loc.linger >= 4) current_loc.haunt();
}

void Stats() {
    //temporary code
    cout << "sanity: " << player.sanity;
    cout << "\nfear: " << player.fear;
    cout << "\nlocation: " << current_loc.name;
}

void Move() {
    string buff;
    bool noloc = true;
    cout << "\nEnter Location\n>> ";
    getline(std::cin, buff);

    for (int i = 0; i < 6; i++) {
        if (locations[i].name == buff && buff != current_loc.name) {
            noloc = false;
            gametime[1] += abs(current_loc.pos - locations[i].pos) * 15;
            current_loc = locations[i];
            Update();
            break;
        }
    }

    if (noloc) {
        cout << "Invalid Input!\n";
        Move();
    }
    
}

void Action(string buff) {
    if (buff == "-1") gameOver = true;
    else if (buff == "STATS") Stats();
    else if (buff == "MOVE") Move();
    else if (buff == "INVENTORY") CheckInv();
    else if (buff == "HOLD") Hold();
    //else if (buff == "USE") Use();
    else if (buff == "TIME") Watch();
    //else if (buff == "EXPLORE") Explore();
    else cout << "Invalid Input!";
}

void Watch() {
    update_time();
    string meridian;
    if (gametime[0] < 24 && gametime[0] >= 12) meridian = "pm";
    else if (gametime[0] == 0) meridian = "mn";
    else meridian = "am";
    Read("art.txt", 1, 9);
    cout << "          "<< gametime[0] <<  ":" << setfill('0') << setw(2) << gametime[1] << meridian << "\n";
    Read("art.txt", 11, 19);
}

void CheckInv() {
    for (int i = 0; i < 5; i++) {
        if (inventory[i].name == "none") cout << "empty slot " << i + 1 << "\n";
        else cout << inventory[i].name << "\n";
    }
}

void Hold() {
    CheckInv();
    string buff;
    holding:
        cout << "\nChoose item to hold\n>> ";
        getline(std::cin, buff);
        int i = stoi(buff);
        if (inventory[i - 1].name == "none"){
            cout << "slot empty!\n";
            goto holding;
        }
        item temp = inventory[i - 1];
        inventory[i - 1] = inventory[0];
        inventory[0] = temp;
}
