#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <iomanip>
#include <cmath>
using namespace std;

struct factor {int sanity; int fear;};
int prob = 75;

void update_time();
void Read(string, int, int);

bool item_check[6][2] = {
	{false, false},
	{false, false},
	{false, false},
	{false, false},
	{false, false},
	{false, false}
};

class item {
    public:
        string name;
        int level;
        int use;
        
        void usage() {
        	Read("use.txt", use+1, use+1);
        }
        
        item() = default;
        item(string x, int y, int z) {
            name = x;
            level = y;
            use = z;
        }
};

int gametime[] = {21, 0};
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
        
        void special() {
        	
        }
};

class location {
    public:
        string name;
        int pos;
        int creepiness;
        int linger;
        ghost entity;
        string spec;
        int item_prob;

        void creep(){
            linger++;
            player.fear += creepiness;
        }

        void haunt(){
            entity.anger++;
            player.sanity -= floor(player.fear/10);
            cout << "\n";
            Read("haunt.txt", floor(player.fear/10) + 2, floor(player.fear/10) + 2);
            cout << "\n";
        }
        
        void act(){
        	
        }

        location() = default;
        location (string x, int y, int z, ghost w, string v, int p) {
            name = x;
            pos = y;
            creepiness = z;
            linger = 0;
            entity = w;
            spec = v;
            item_prob = p;
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
void Catalogue();
void Notes();

ghost melo ("Melody", 0, 5);
ghost dupe ("Doppelganger", 3, 1);
ghost burnt ("The Burnt Man", 5, 4);
ghost lady ("Lady in White", 3, 2);
ghost chain ("The Chained", 5, 3);
ghost forty ("41st Student", 10, 6);
ghost ghosts[]{melo, dupe, burnt, lady, chain, forty};

location sb ("Senior Highschool Building", 4, 4, chain, "HIDE", 10);
location cl ("Chemistry Lab", 3, 3, dupe, "INSPECT", 25);
location fr ("Faculty Room", 2, 4, burnt, "INSPECT", 10);
location mr ("Music Room", 1, 5, melo, "PLAY", 15);
location gh ("Guard House", 0, 1, lady, "CCTV", 30);
location ccc ("Cebu Cultural Center", 5, 10, forty, "RUN", 0);
location locations[]{sb, cl, fr, mr, gh, ccc};

item bagwa("Bagwa", 3, 1);
item fl("Flashlight", 0, 0);

item noitem ("none", 0, -1);

item inventory[] = {noitem, fl, noitem, noitem, noitem};
item &onHand = inventory[0];

int main() {
    //starT
    //Read("explore.txt", 44, 50);
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
    cout << "\n";
    Read("start.txt", 15, 25);
    Read("start.txt", current_loc.pos + 35, current_loc.pos + 35);
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
    Read("start.txt", 28, 32);
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
        cout << "\nInvalid Input!\n";
        Move();
    }
    
}

void Action(string buff) {
    if (buff == "-1") gameOver = true;
    else if (buff == "STATS") Stats();
    else if (buff == "MOVE") Move();
    else if (buff == "INVENTORY") CheckInv();
    else if (buff == "HOLD") Hold();
    else if (buff == "USE") Use();
    else if (buff == "TIME") Watch();
    //else if (buff == "EXPLORE") Explore();
    //else if (buff == "CATALOGUE") Catalogue();
    //else if (buff == "NOTES") Notes();
    else cout << "\nInvalid Input!\n";
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
    cout << "\n";
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
            cout << "\nslot empty!\n";
            goto holding;
        }
        item temp = inventory[i - 1];
        inventory[i - 1] = inventory[0];
        inventory[0] = temp;
        Update();
}

void Use() {
	onHand.usage();
	Update();
}

void Explore() {
	int randnum = rand() % 100;
	if (randnum <= current_loc.item_prob && item_check[current_loc.pos][0]) {
		item_check[current_loc.pos][0] = true;
	}
	else if (randnum <= prob && !item_check[current_loc.pos][1]) {
		if (randnum % 3 == 0) {
			item_check[current_loc.pos][1] = true;
		}
		else {
			
		}
	}
	else {
		
	}
}