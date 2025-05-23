#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <iomanip>
#include <cmath>
using namespace std;

struct factor {int sanity; int fear;};
int prob = 40;
int flash = 0;
bool hide = false;
int temperature = 25;
int cooldown = 0;
bool disable = false;

void update_time();
void Read(string, int, int);

bool item_check[6][3] = {
	{false, false, false},
	{false, false, false},
	{false, false, false},
	{false, false, false},
	{false, false, false},
	{false, false, false}
};

class item {
    public:
        string name;
        int level;
        int use;
        
        void usage();
        
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
        
        void special();
};

class location {
    public:
        string name;
        int pos;
        int creepiness;
        int linger;
        ghost *entity;
        string spec;
        int item_prob;
        bool locked;
        item item_1;
        item item_2;

        void creep(){
            linger++;
            player.fear += ceil(creepiness/2) - (flash * floor(creepiness/2));
        }

        void haunt(){
            entity->anger++;
            player.fear += floor(linger/5);
            player.sanity -= floor(player.fear/10);
            cout << "\n\n";
            Read("haunt.txt", pos * 12 + floor(player.fear/10) + 2, pos * 12 + floor(player.fear/10) + 2);
        }
        
        void act();

        location() = default;
        location (string x, int y, int z, ghost *w, string v, int p, bool b, item i1, item i2) {
            name = x;
            pos = y;
            creepiness = z;
            linger = 0;
            entity = w;
            spec = v;
            item_prob = p;
            locked = b;
            item_1 = i1;
            item_2 = i2;
        }    
};

bool gameOver = false;

void Update();
void Read();
void Start();
void Gameloop();
void CheckforSlot(item given);

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
void Throw();
void Chase();

ghost melo ("Melody", 0, 5);
ghost dupe ("Doppelganger", 3, 1);
ghost burnt ("The Burnt Man", 5, 4);
ghost lady ("Lady in White", 0, 2);
ghost chain ("The Chained", 5, 3);
ghost forty ("41st Student", 10, 6);
ghost ghosts[]{melo, dupe, burnt, lady, chain, forty};

item bagwa("Bagwa", 3, 1);
item cl_key("Chemistry Lab Key", 1, 2);
item shs_key("Senior High School Building Key", 0, 2);
item fr_key("Faculty Room Key", 2, 2);
item med("Medkit", 5, 4);
item emf("EMF Detector", 3, 3);
item fl("Flashlight", 0, 0);
item h2o("Holy Water", 5, 1);
item bag("Sling Bag", 0, -1);

item noitem ("none", 0, -1);

location sb ("Senior Highschool Building", 4, 4, &chain, "HIDE", 10, true, fr_key, med);
location cl ("Chemistry Lab", 3, 3, &dupe, "MIX", 25, true, shs_key, h2o);
location fr ("Faculty Room", 2, 4, &burnt, "COOL", 5, true, bag, med);
location mr ("Music Room", 1, 5, &melo, "PLAY", 15, false, med, bagwa);
location gh ("Guard House", 0, 1, &lady, "CCTV", 30, false, cl_key, emf);
location ccc ("Cebu Cultural Center", 5, 10, &forty, "RUN", 0, false, noitem, noitem);
location locations[]{sb, cl, fr, mr, gh, ccc};

location *current_loc;

item inventory[] = {noitem, fl, noitem, noitem, noitem};
item &onHand = inventory[0];

void ghost::special() {
    if (anger < 3) return;
    switch (level)
    {
        case 1:
            cout << "\nSomething with your exact likeness smiles at you. The entity taunted all spirits across the school.\n";
            for (int i = 0; i < 6; i++) ghosts[i].anger += level;
            player.sanity -= anger;
            player.fear += anger * 2;
            break;

        case 2:
            cout << "\nA branch fell down on you. You lost your onhand item.\n";
            inventory[0] = noitem;
            player.sanity -= anger;
            player.fear += anger * 2;
            cooldown = 5;
            break;

        case 3:
            cout << "\nYou are restrained by a ghostly chain. You cannot move to another location.\n";
            cooldown = 5;
            disable = true;
            player.sanity -= anger;
            player.fear += anger * 2;
            break;

        case 4:
            //if (temperature < 30) break;
            cout << "\nThe immense heat has been unknowingly becoming worse.\n";
            temperature += 15;
            player.sanity -= anger + floor(temperature / 2);
            player.fear += anger + floor(temperature / 2);
            break;

        case 5:
            cout << "\nA mesmerizing melody echoes around the room. You slept for two hours.\n";
            gametime[0] += 2;
            player.sanity -= anger;
            player.fear += anger * 2;
            break;
    }
}

void location::act(){
    string buff;
    bool noloc = true;
    switch (pos)
    {
        case 0:
            Read("start.txt", 28, 32);
            checkloc:
                cout << "\nEnter Location\n>> ";
                getline(std::cin, buff);

                for (int i = 0; i < 6; i++) {
                    if (locations[i].name == buff){
                        if (locations[i].entity->anger >= 8) {
                            cout << "\nMovement spotted at the " << locations[i].name << ".\n";
                        }
                        else {
                            cout << "\nNothing at at the " << locations[i].name << ".\n";
                        }
                        switch (locations[i].creepiness){
                            case 1: cout << "The room is well lit and accomodating.\n"; break;
                            case 3: cout << "The area is dimly lit. Your vision tricks you into seeing shadows.\n"; break;
                            case 4: cout << "The building, although well lit, looks menacing under the moonlight.\n"; break;
                            case 5:
                                cout << "Everything about the room emits a feeling of creepiness. Just by looking at it sends shivers down your spine.\n";
                                player.fear += 3;
                                break;
                        }
                    }
                    noloc = false;
                }

            if (noloc) {
                cout << "\nInvalid Input!\n";
                goto checkloc;
            }

            
            gametime[1] += 10;
            break;

        case 1:
            Read("art.txt", 21, 28);
            cout << "\nPlay a melody (SPACE SEPARATED CAPITAL LETTERS)\n>> ";
            getline(std::cin, buff);
            if (buff == "D E A D" && melo.anger > 8) {
                melo.anger = 0;
                cout << "\nYou feel lighter and more calm.\n";
                player.sanity += 20;
                player.fear -= floor(player.fear/2);
            }
            else {
                melo.anger += 8;
                cout << "\nThe mirrors echo a dissonant tune. The frequency made you feel more anxious.\n";
                player.sanity -= 10;
                player.fear += 15;
                linger++;
            }
            gametime[1] += 10;

        case 2:
            temperature -= 5;
            burnt.anger += floor(burnt.anger /2);
            cout << "\nYou felt the sweet cold air of the fan, although you feel something worse approaching.\n";
            gametime[1] += 10;
            break;

        case 4:
            hide = true;
            gametime[1] += 10;
            break;

        case 3:
            cout << "\nEnter Elements\n>> ";
            string elements[3];
            int scount = 0;
            bool fe = false;
            cin >> elements[0] >> elements[1] >> elements[2];
            for (int i = 0; i < 3; i++) {
                if (elements[i] == "S") scount++;
                else if (elements[i] == "Fe") fe = true;
            }
            if (scount == 2 && fe && dupe.anger > 8) {
                dupe.anger = 0;
                cout << "\nThe elements mix and form a metal that looks like gold. Your vision clears with its luster.\n";
                player.sanity += 20;
                player.fear -= floor(player.fear/2);
            }
            else {
                dupe.anger += 8;
                cout << "\nThe flasks turn into a blood red ooze. You hear a chilling cackle outside.\n";
                player.sanity -= 10;
                player.fear += 15;
                linger++;
            }
            gametime[1] += 10;
            break;

    }

    

    Update();
}



void item::usage() {
    // 0 - reduce creepiness
    // 1 - dispell
    // 2 - key
    // 3 - detect
    // 4 - heal
    Read("use.txt", use+1, use+1);
    switch (use)
    {
    case 0:
        flash = 1;
        break;
    case 1:
        if (level >= current_loc->entity->level) {
            current_loc->entity->anger -= floor(current_loc->entity->anger*5/6);
            cout << "\nYou felt the ghost weakening. The item loses a part of its power.\n";
            level--;
            player.fear -= floor(player.fear/10);
        }
        else {
            current_loc->entity->anger += 2 * current_loc->entity->level - level;
            cout << "\nThe room trembles. You felt the ghost's presence becoming more violent.\n";
            player.fear += floor(player.fear/10);
            player.sanity += log(player.fear) * 2;   
        }
        break;
    case 2:
        locations[level].locked = false;
        cout << "\nThe " << locations[level].name << " is now unlocked.\n";
        inventory[0] = noitem;
        CheckInv();
        break;
    case 3:
        if (current_loc->entity->level < 5) {
            cout << "\nGhost aggression: " << current_loc->entity->anger << "\n";
            cout << "\nLength of stay: " << current_loc->linger << "\n";
        }
        break;
    case 4:
        player.sanity += 10 + floor(player.fear/2);
        cout << "\nYou patched yourself up, trying to recollect your thoughts.\n";
        inventory[0] = noitem;
        CheckInv();
        break;
    }
    
}

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
    
    current_loc = &locations[4];
}

void Gameloop() {
    string buff;
    cout << "\n\n";
    if (disable) cout << "(CHAINED)\n";
    Read("start.txt", 15, 23);
    if (player.fear >= 50) cout << "(BROKEN BY GHOST) ";
    Read("start.txt", current_loc->pos + 35, current_loc->pos + 35);
    cout << "\n\nEnter Action\n>> ";
    getline(std::cin, buff);
    Action(buff);
}

void Update() {
    if (onHand.name != "Flashlight") flash = 0;
    update_time();
    cooldown--;
    current_loc->creep();
    if (rand() % 100 <= player.fear && cooldown <= 0) current_loc->entity->special();
    if (cooldown <= 3) disable = false;
    if (current_loc->linger >= 4) current_loc->haunt();
    if (current_loc->name == "Faculty Room") temperature += 2;
    Chase();

    if (player.fear >= 100) player.sanity -= gametime[0];
    if (player.sanity <= 0) {
        cout << "You succumbed to your insanity and collapsed.\n";
        Stats();
        cout << "\nCause of death: " << current_loc->entity->name;
        cout <<"\nEnter any key to exit game.>> \n";

        string ends;
        cin >> ends;

        gameOver = true;
    }
    if (gametime[0] >= 6 && gametime[0] <= 12) {
        cout << "The guards came back from the college side and found you. You were expelled from the school.\n";
        Stats();
        cout <<"\nEnter any key to exit game.>> \n";

        string ends;
        cin >> ends;

        gameOver = true;
    }
    hide = false;
}

void Chase() {
    if (current_loc->linger % 6 == 0 && current_loc->name == "Senior Highschool Building") {
        if (!hide) {
            player.sanity /= 2;
            player.fear += 25;
            cout << "\nYou felt an intense gaze as your legs seem to fail you.\n";
        }
        else {
            player.sanity--;
            cout << "\nYou felt a heavy force around the closet. You are safe for now.\n"; 
        }
    }
}

void Stats() {
    //temporary code
    cout << "sanity: " << player.sanity;
    cout << "\nfear: " << player.fear;
    cout << "\nlocation: " << current_loc->name;
}

void Move() {
    string buff;
    bool noloc = true;

    Read("start.txt", 28, 32);
    cout << "\nEnter Location\n>> ";
    getline(std::cin, buff);

    for (int i = 0; i < 6; i++) {
        if (locations[i].locked && locations[i].name == buff) {
            noloc = false;
            cout << "\nThe " << locations[i].name << " is locked.\n\n";
            Move();
            break;
        }
        else if (locations[i].name == buff) {
            noloc = false;
            gametime[1] += abs(current_loc->pos - locations[i].pos) * 15;
            current_loc = &locations[i];
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
    else if (buff == "EXPLORE") Explore();
    else if (buff == current_loc->spec && player.fear < 50) current_loc->act();
    else cout << "\nInvalid Input!\n";
}

void Watch() {
    update_time();
    string meridian;
    if (gametime[0] < 24 && gametime[0] >= 12) meridian = "pm";
    else if (gametime[0] == 0) meridian = "mn";
    else meridian = "am";
    Read("art.txt", 1, 9);
    if (current_loc->name != "Chemistry Lab") {
        cout << "          " << gametime[0]%12 + 1 <<  ":" << setfill('0') << setw(2) << gametime[1] << meridian << "\n";
        cout << "          " << temperature << "*C\n";
    }
    else {
        cout << "          " << rand()%12 + 1 <<  ":" << setfill('0') << setw(2) << rand() % 60 << meridian << "\n";
        cout << "          " << rand()%100 << "*C\n";
    }
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
        cout << "\nChoose item to hold (number)\n>> ";
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
    cout << "\n";
	onHand.usage();
	Update();
}

void Throw() {
    CheckInv();
    string buff;
    holding:
        cout << "\nChoose item to throw (number)\n>> ";
        getline(std::cin, buff);
        int i = stoi(buff);
        if (inventory[i - 1].name == "none"){
            cout << "\nslot empty!\n";
            goto holding;
        }
        inventory[i - 1] = noitem;
}

void CheckforSlot(item given) {
    bool found = false;
    for (int i = 0; i < 5; i++) {
        if (inventory[i].name == "none") {
            found = true;
            inventory[i] = given;
            CheckInv();
            break;
        }
    }
    if (!found) Throw();
}

void Explore() {
	int randnum = rand() % 100;
    int loc = current_loc->pos*36;
    cout << "\n";


    if (randnum <= current_loc->item_prob/2) {
        Read("explore.txt", loc + 3, loc + 3);
        CheckforSlot(current_loc->item_1);
	}

    else if (randnum <= current_loc->item_prob) {
        Read("explore.txt", loc + 5, loc + 5);
        CheckforSlot(current_loc->item_2);
	}

	else if (randnum <= prob && !item_check[current_loc->pos][2]) {
        Read("explore.txt", loc + 7, loc + 29);
		item_check[current_loc->pos][2] = true;
    }

	else {
		Read("explore.txt", loc + 31 + (randnum % 5), loc + 31 + (randnum % 5));
	}

    gametime[1] += 10;
    Update();
}
