#include "Exceptions.h"
#include "FileSystem.h"

using namespace std;

FileSystem::~FileSystem()
{
	delete root_;
	current_ = nullptr;
}

void FileSystem::loadHierarchy(const string& fs_filepath, const string& log_filepath) // ucitavanje iz fajla i konstruisanje stabla
{
	root_ = new Folder(); //koren je folder bez naziva 
	output_file_ = log_filepath;
	input_file_ = fs_filepath;
	fstream input_file(input_file_, ios::in);
	while (input_file.peek() != EOF) {
		string command;
		getline(input_file, command); //jednu po jednu liniju iz fajla uzimamo
		createNewNode(command);
	}
	input_file.close();
	current_ = root_; //trenutni direktorijum je na pocetku koreni
}

void FileSystem::execute(const string& filepath)
{
	fstream inputFile(filepath, ios::in);
	fstream outputFile(output_file_, ios::out);
	while (inputFile.peek() != EOF) {
		string command;
		getline(inputFile, command); 
		int pos = command.find(' '); // razdvajamo oblik komande i njen sadrzaj 
		string subst = command.substr(0, pos);
		try {
			switch (hash(subst)) {
			case LS:
				sortTree(); // prvo sortiramo pa onda ispisemo stablo
				print(current_, outputFile);
				if (inputFile.peek() != EOF) //ako nije poslednja komanda u fajlu onda predji u novi red
					outputFile << endl;
				break;
			case NEW: 
				createNew(command.substr(pos + 1), current_); // command.substr hvata string posle oblika komande 
				break; 
			case CD:
				changeDirectory(command.substr(pos + 1));  
				break;
			case DEL:
				deleteNode(command.substr(pos + 1), current_);
				break;
			case EXE:
				doExe(command.substr(pos + 1), current_, outputFile, inputFile);
				break;
			case DEF:
				throw ErrorExe("Error");
				break;
			}
		}
		catch(ErrorExe& e){
			outputFile << e.what();
		}
		catch(string str){
			outputFile << str;
			exception_ = "";
		}
	}
	inputFile.close();
	outputFile.close();
}


void FileSystem::createNewNode(string& path) // na osnovu jedne linije(jedne putanje) pravimo novi cvor u stablu
{
	TreeNode* current = root_; 
	TreeNode* new_node;
 	int pos = 1; // pocinjemo od 2. karaktera
	try {
		if (path[0] != '\\') { // ako prvi nije karakter / izbaci gresku
			throw ErrorInput("Input Error\n"); 
		}
		else {
			string sub_path = ""; // podstring 
			while (path[pos] != '\0') { // dok ne dodjemo do kraja linije 
				while ((path[pos] != '\\' || (path[pos] == '\\' && path[pos + 1] == 'n')) && path[pos] != '\0') { 
					/* ^^^^uzimaj karakter po karakter sve dok ne dodjes do kose crte 
					(ako je kosa crta onda nije poslednji u putanji 
					i trebalo bi da negde vec postoji pa udji u njega)
					ili do kraja linije a ako naidjes na \n onda nastavi*/ 
					sub_path += path[pos];
					pos++;
				}
				if (path[pos] == '\0') { // ako je on poslednji u putanji napravi ga
					new_node = parse(sub_path, current);
				}
				else { // ako nije onda nadji fajl sa tim imenom i pokusaj da napravis u njemu dete
					for (auto child : current->getChildren()) { //ispitaj da li postoji medju decom trenutnog cvora neki sa imenom koje smo nasli
						if (child->getName() == sub_path) {
							if (child->getExt() == "fol") { // naravno mora biti folder da bismo sisli jedan nivo nize
								current = child;
								sub_path = "";
								pos++;
							}
							else {
								throw ErrorInput("Input Error\n");
							}
						}
					}
				}
			}
		}
	}
	catch(ErrorInput& e){
		string error = e.what() + path;
		cout << error;
	}
	
	
}

TreeNode* FileSystem::parse(string& s, TreeNode* parent) // parsira string tipa {*ime_foldera*} ili tipa {*ime fajla* *sadrzaj*}
{
	try {
		int flag = 0; // flag nam trazi tacku: ako tacka ne postoji onda ce cvor biti tipa Folder
		for (int i = 0; i < s.length(); i++) {
			if (s[i] == '.')
				flag = 1;
		}
		if (!flag) { //napravi novi objekat izvedene klase Folder
			Folder* new_folder = new Folder(parent, s, "fol", 0); 
			parent->getChildren().push_back(new_folder);
			parent->setNum(parent->getNum() + 1);
			return new_folder;
		}
		else { //Txt i Exe 
			int pos = 0;
			for (int i = 0; i < s.length(); i++) {
				int ext_pos = s.find('.'); // nadji ekstenziju
				string name = s.substr(0, ext_pos + 4); //odvoji ime fajla u name
				string ext = s.substr(ext_pos + 1, 3); //odvoji ekstenziju u ext
				if (ext == "txt") {
					TxtFile* new_txt_file = new TxtFile(parent, name, "txt", 0, s.substr(ext_pos + 5)); //napravi novi objekat izvedene klase TxtFile
					parent->getChildren().push_back(new_txt_file);
					parent->setNum(parent->getNum() + 1);
					return new_txt_file;
				}
				if (ext == "exe") {
					vector<Commands*> commands;
					parseExeFinal(s.substr(ext_pos + 5), commands); // parsiraj komande ExeFilea i ubaci ih u vektor commands
					ExeFile* new_exe_file = new ExeFile(parent, name, "exe", 0, commands); //napravi novi objekat izvedene klase ExeFile
					parent->getChildren().push_back(new_exe_file);
					parent->setNum(parent->getNum() + 1);
					return new_exe_file;

				}
			}
			return nullptr;
		}
	}
	catch (ErrorInput& e) {
		string error = e.what();
		cout << error;
		return nullptr;
	}
}


void FileSystem::parseExeFinal(string s, vector<Commands*> &c)
{
	int pos = 0;
	string subs = "";
	int flag = 0;
	s += "   ";
	while (s[pos] != '\0') {
		if (s[pos] != ' ') { //trazi novu komandu
			subs += s[pos];
			pos++;
		}
		else {
			pos++;
			switch (hash(subs))
			{
			case LS: //ako je LS dodaj je u c (to je vector koji pravimo gore)
				c.push_back(new LSC("LS"));
				break;
			case CD:
				subs = readContent(s, pos); //procitaj sta se nalazi uz komandu 
				c.push_back(new CDC("DEL", subs)); // i to ubaci u vector c
				break;
			case NEW:
				subs = readContent(s, pos);
				c.push_back(new NewC("NEW", subs));
				break;
			case DEL:
				subs = readContent(s, pos);
				c.push_back(new DelC("DEL", subs));
				break;
			case EXE: 
				subs = readContent(s, pos);
				c.push_back(new ExeC("EXE", subs));
				break; 
			case DEF:
				break;
			}
			subs = "";
			pos += 2; //stigli smo do \n znaci treba da preskocimo dva znaka 
		}
	}
}


comm_ FileSystem::hash(string in)// hesiranje ulaznog stringa u enume
{
	if (in == "NEW") return NEW;
	else if (in == "LS") return LS;
	else if (in == "CD") return CD;
	else if (in == "EXE") return EXE;
	else if (in == "DEL") return DEL;
	else return DEF;
}

void FileSystem::print(TreeNode* t, fstream& output) // ispis stabla u output startujuci od cvora t(trenutni direktorijum)
{
	TreeNode* node = t; 
	stack<pair<TreeNode*, string>> unvisited_nodes; /* stek u kom svaki element sadrzi cvor i celu prethodnu putanju do tog cvora od trenutnog direktorijuma*/
	for (auto child : node->getChildren()) {
		unvisited_nodes.push(make_pair(child, "")); //ubacujemo decu trenutnog direktorijuma u stek 
	}
	while (!unvisited_nodes.empty()) { // radimo preorder prolaz kroz stablo
		node = unvisited_nodes.top().first;
		string s = unvisited_nodes.top().second;
		s = s + '\\' + node->getName(); // na prethodnu putanju dodajemo ime trenutnog
		output << setw(3) << setfill(' ') << s;
		vector<TreeNode*> w = node->getChildren(); // uzimamo decu od trenutnog 
		unvisited_nodes.pop();//trenutni skidamo a onda pitamo da li deca tog trenutnog postoje (ako je vektor dece prazan onda je to txt ili exe fajl u suprotnom je folder)
		if (!w.empty())
		{ //ako postoje ubacujemo ih na stek
			for (auto child : w)
				unvisited_nodes.push(make_pair(child, s));
		}
		if (!unvisited_nodes.empty()) { //posle poslednjeg ispisanog ne treba novi endl pa je uveden ovaj uslov
			output << endl;
		}
	}
}

void FileSystem::sortTree() // sortiramo stablo
{
	TreeNode* t = root_; //stablo uvek sortiramo od vrha (mada moze sa dodatnim argumentom u funkciji da radi i od bilo kog Foldera)
	TreeNode* dummy = nullptr; // dummy sluzi za swapovanje dece
	stack<TreeNode*> unvisited_nodes;
	unvisited_nodes.push(t); 
	while (!unvisited_nodes.empty()) {
		t = unvisited_nodes.top();
		vector<TreeNode*>::iterator itb1 = t->getChildren().begin(), ite1 = t->getChildren().end()--, itb2 = t->getChildren().begin()++, ite2 = t->getChildren().end();
		for (vector<TreeNode*>::iterator it_1 = itb1; it_1 != ite1; it_1++) { //najobicniji selection sort sa iteratorima
			for (vector<TreeNode*>::iterator it_2 = itb2; it_2 != ite2; it_2++)
			{
				if (toLower((*it_1)->getName()) > toLower((*it_2)->getName())) { //ispitujemo relaciju imena dva deteta
					dummy = *it_1;
					*it_1 = *it_2;
					*it_2 = dummy;
				}
			}
		}
		unvisited_nodes.pop(); //roditelja prethodno sortirane dece izbacujemo iz steka
		for (auto child : t->getChildren()) { //a onda njegovu decu AKO SU FOLDERI ubacujemo
			if (!child->getChildren().empty()) {
				unvisited_nodes.push(child);
			}
		}
	}
	t = nullptr;
	dummy = nullptr;
}

string FileSystem::readContent(string str, int& pos)
{
	string subs = "";
	while (str[pos] != '\\' || str[pos + 1] != 'n') { //dok ne dodjes do \n skupljaj string
		subs += str[pos];
		pos++;
	}
	return subs;
}

string FileSystem::toLower(string s)
{
	for (char& c : s) 
		c = tolower(c);
	return s;
}

void FileSystem::changeDirectory(string directory)
{
	if (directory == "..") { // ako treba da se vrati u roditelja
		try
		{
			if (!current_->getParent())

				throw ErrorExe("CD ");
			else
				current_ = current_->getParent();

		}
		catch (ErrorExe e)
		{
			string s = e.what();
			exception_ = "Error: " + s;
			exception_.append("\n");
			throw exception_;
		};
	}
	else { // inace trazi dete sa istim imenom kao sto je zadato 
		try {
			int flag = 0;
			for (auto child : current_->getChildren()) {
				if (child->getName() == directory)
				{
					flag = 1;
					current_ = child;
				}
			}
			if (!flag)
				throw ErrorExe("CD ");
		}
		catch (ErrorExe &e)
		{
			string s = e.what();
			exception_ = "Error: " + s + directory;
			exception_.append("\n");
			throw exception_;
		}
	}
}

void FileSystem::createNew(string s, TreeNode* parent)
{
	try {
		TreeNode* w = parse(s, parent);// parse koristis isti kao gore za pravljenje stabla 
		int i = 0; 
		/*w je upravo napravljen cvor, i je broj poklapanja (ime i ext) u vektoru dece roditelja parent */
		for (auto child: parent->getChildren()) { //prolazimo kroz decu da vidimo ima li koje da se zove isto kao tek napravljeno dete i da li ima istu ext
			string s_1 = child->getName();
			string s_2 = w->getName();
			string e_1 = child->getExt();
			string e_2 = w->getExt();
			int a = 0;
			bool good_s = s_1 == s_2 ? true : false; //good_s imena ista 
			bool good_e = e_1 == e_2 ? true : false; //good_e ext iste
			if (good_s && good_e) 
				i++;
			if (i > 1) { //posto smo ubacili cvor u vektor dece tekuceg direktorijuma to znaci da ce sigurno biti jedno ponavljanje, a ako postoji vise od jednog izbrisi w
				deleteNode(w->getName(), current_);
				throw ErrorExe("NEW ");
			}
		}
	}
	catch (ErrorExe& e) { // ako uhvatis exception izmeni exception_ i baci ga dalje (isti su svi exceptioni, samo je razlika gde se koji poziva)
		string s_e = e.what();
		exception_ = "Error: " + s_e + s;
		exception_.append("\n");
		throw exception_;
	}
}

void FileSystem::deleteNode(string s, TreeNode* parent)
{
	try {
		int flag = 0;
		for (auto child : parent->getChildren()) {
			if (child->getName() == s) { //nadji dete koje se zove kao s
				delete child;
				child = nullptr;
				parent->setNum(parent->getNum() - 1);
				flag = 1;
				vector<TreeNode*>::iterator ite = parent->getChildren().end();
				vector<TreeNode*>::iterator it_2 = parent->getChildren().begin(); 
				it_2++; // drugi clan
				vector<TreeNode*>::iterator it_1 = parent->getChildren().begin(); // prvi clan
				int flag_1 = 0; 
				TreeNode* dummy = nullptr;
				while (it_2 != ite) { //gurni dete do kraja niza dece a onda izbaci iz vektora
					if ((*it_1) == child) {
						flag_1 = 1;
					}
					if (flag_1) {
						dummy = *it_1;
						*it_1 = *it_2;
						*it_2 = dummy;
					}
					it_1++;
					it_2++;
				}
				parent->getChildren().pop_back();
				dummy = nullptr;
				break;
			}
		}
		if (!flag)
			throw ErrorExe("DEL ");
	}
	catch(ErrorExe& e){ 
		string s_e = e.what();
		exception_ = "Error: " + s_e + s;
		exception_.append("\n");
		throw exception_;
	}
}

void FileSystem::doExe(string str, TreeNode* parent, fstream& output, fstream& input) // veoma slicno execute funkciji
{
	string s = "";
	try {
		for (auto file : parent->getChildren()) {
			if (file->getName() == str) { //samo je razlika u tome sto ovde imamo uz exe fajl koji pustamo njegove komande
				for (auto command : file->getCommands()) {
					switch (hash(command->getType()))
					{
					case LS:
						sortTree();
						print(parent, output);
						if (input.peek() != EOF)
							output << endl;
						break;
					case NEW:
						s = command->getNew();
						createNew(command->getNew(), parent);
						s = "";
						break;
					case CD:
						changeDirectory(command->getCD());
						break;
					case DEL:
						deleteNode(command->getDel(), parent);
						break;
					case EXE:
						break;
					case DEF:
						throw ErrorExe("EXE ");
						break;
					}
				}
				break;
			}
		}
	}
	catch (ErrorExe & e) {
		string s = e.what();
		exception_ = "Error: " + s + str;
		exception_.append("\n");
		throw exception_;
	}

}






