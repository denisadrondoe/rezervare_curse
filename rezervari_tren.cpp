#include <iostream>
#include <string>
#include <fstream>
#include <sstream> //avem nevoie cand extragem elemente din fisierul csv
#include <ctime>
#include <cstdio>
#include <cctype> //pentru functia isalpha

using namespace std;
// implementam algoritmul Vigenère Cipher pentru criptarea parolelor

// functia pentru extinderea cheii
string extendKey(const string &text, const string &key)
{
    string extendedKey = "";
    int keyIndex = 0;
    for (int i = 0; i < text.length(); i++)
    {
        extendedKey += key[keyIndex];

        keyIndex = (keyIndex + 1) % key.length(); // revenim la inceputul cheii dacaajunge la final
    }

    return extendedKey;
}

// functia prentru criptarea textului folosind cheia
string encryptVigenere(const string &text, const string &key)
{
    string cipher_text = "";
    string extendedKey = extendKey(text, key);

    for (int i = 0; i < text.length(); i++)
    {
        // daca caracterul este litera, criptam
        if (isalpha(text[i]))
        {
            char base;
            if (isupper(text[i]))
            { // determinam baza 'A' sau 'a'
                base = 'A';
            }
            else
            {
                base = 'a';
            }

            char textChar = text[i] - base;                        // retinem pozitia relativa din alfabet(0-25)
            char keyChar = tolower(extendedKey[i]) - 'a';          // ne asiguram  ca lucram cu litere mici si transformam caracterul corespunzator intr o valoare numerica
            char encryptedChar = (textChar + keyChar) % 26 + base; // operatia de criptare

            cipher_text += encryptedChar;
        }
        else
        {
            // daca nu este litera il lasam nemodificat
            cipher_text += text[i];
        }
    }
    return cipher_text;
}

// clasa pentru Cursa
class Cursa
{
private:
    string destinatie;
    string plecare;
    string data;
    string ora;
    int clasa;
    int numarLocuri;

public:
    // Constructor default
    Cursa() : clasa(0), numarLocuri(0) {}

    // constructor cu parametrii
    Cursa(string destinatie, string plecare, string data, string ora, int clasa, int numarLocuri)
        : destinatie(destinatie), plecare(plecare), data(data), ora(ora), clasa(clasa), numarLocuri(numarLocuri) {}

    // setter
    void setDestinatie(string destinatie)
    {
        this->destinatie = destinatie;
    }
    void setPlecare(string plecare)
    {
        this->plecare = plecare;
    }
    void setData(string data)
    {
        this->data = data;
    }
    void setOra(string ora)
    {
        this->ora = ora;
    }
    void setClasa(int clasa)
    {
        this->clasa = clasa;
    }
    void setNumarLocuri(int numarLocuri)
    {
        this->numarLocuri = numarLocuri;
    }

    // getter
    string getDestinatie()
    {
        return destinatie;
    }
    string getPlecare()
    {
        return plecare;
    }
    string getData()
    {
        return data;
    }
    string getOra()
    {
        return ora;
    }
    int getClasa()
    {
        return clasa;
    }
    int getNumarLocuri()
    {
        return numarLocuri;
    }

    // verificam daca data este in trecut
    bool isPastDate(string &data)
    {
        tm dateStruct = {};
        sscanf(data.c_str(), "%2d/%2d/%4d", &dateStruct.tm_mday, &dateStruct.tm_mon, &dateStruct.tm_year);

        dateStruct.tm_mon -= 1;     // lunile sunt 0-11, deci trebuie să scădem 1
        dateStruct.tm_year -= 1900; // anul este calculat începând de la 1900

        time_t timeNow = time(0); // obtinem data curenta
        tm *currentTime = localtime(&timeNow);

        return difftime(mktime(&dateStruct), mktime(currentTime)) < 0; // calculeaza diferenta in secunde dintre cele doua date
        // pentru a putea vedea daca este in trecut
    }

    // verificam daca orasul contine caractere nepermise
    bool CityName(string &city)
    {
        // Verificam fiecare caracter din sirul city
        for (char c : city)
        {
            // daca caracterul nu este o litera sau un spatiu asta inseamna caorasul are in componenta sa caractere nedorite
            if (!isalpha(c) && c != ' ')
            {
                return false; // numele pe care l am introdus pentru oras nu este corect
            }
        }
        return true;
    }

    string getDetails() const
    {
        stringstream ss;
        ss << destinatie << ", " << plecare << ", " << data << ", " << ora << ", Clasa: " << clasa << ", Locuri: " << numarLocuri;
        return ss.str();
    }
};

// clasele operator si utilizator
// Operator: activități de login și adăugare/ștergere curse (contul este deja existent în sistem cu user și parolă, iar
// acestea se adauga dintr-un fisier csv)
class Operator
{
private:
    string username;
    string password;

public:
    Operator(string user, string pass) : username(user), password(pass) {} // facem un constructor si initializam user si pass

    // Login pentru operatori
    void login(string user, string pass)
    {
        ifstream file("operator.csv"); // citeste fisierul csv
        if (!file.is_open())
        {
            cout << "Nu s a putut deschide fisierul operator.csv" << endl;
            throw runtime_error("fisier operator.csv nupoate fi deschis");
        }
        else
        {
            cout << "fisierul a fost deschis cu succes" << endl;
        }

        string line;
        bool found = false;

        while (getline(file, line)) // Citește o linie completă
        {
            stringstream ss(line); // transforma linia intr-un flux
            string fileUser, filePass;

            getline(ss, fileUser, ','); // Citește până la virgulă
            getline(ss, filePass, ',');

            // Debugging: afișează valorile citite
            cout << "Din fisier: username = '" << fileUser << "', password = '" << filePass << "'" << endl;

            if (fileUser == user && filePass == pass)
            { // verifica login-ul
                found = true;
                break;
            }
        }
        file.close();

        if (found == false)
        {
            throw invalid_argument("LOGIN NEREUSIT!! Username sau parola gresita!");
        }
    }

    // Adaugare cursa
    void adaugaCursa(Cursa cursa)
    {
        // cazul in care a lasat vreo informatie necompletata
        if (cursa.getDestinatie().empty() || cursa.getPlecare().empty() || cursa.getData().empty() || cursa.getOra().empty() || cursa.getClasa() == 0 || cursa.getNumarLocuri() == 0)
        {
            throw invalid_argument("Nu ati completat toate datele despre cursa!");
        }

        // validare pentru data format: DD/MM/YYYY
        if (cursa.getData().size() != 10 || cursa.getData()[2] != '/' || cursa.getData()[5] != '/')
        {
            throw invalid_argument("Format data gresit! Incercati sa introduceti data sub forma: DD/MM/YYYY");
        }

        // verificam daca numele orasului este valid
        if (!cursa.CityName(cursa.getDestinatie()) || !cursa.CityName(cursa.getPlecare()))
        {
            throw invalid_argument("orasul contine caractere nevalide, folositi doar litere si spatii!!");
        }

        // verificam daca data este in trecut
        if (cursa.isPastDate(cursa.getData()))
        {
            throw invalid_argument("Data pe care ati introdus-o este din trecut!");
        }

        // adaugam cursa in fieisrul csv OFSTREAM -deschide fisierul pentru scriere
        ofstream outFile("curse.csv", ios::app); // deschidem fisierul in modul append, pentru a adauga noi elemente
        if (!outFile.is_open())
        {
            throw runtime_error("Nu s-a putut deschide fisierul pentru adaugarea cursei.");
        }

        // mai avem nevoie de: ora, clasa si numarul de locuri disponibile

        outFile << cursa.getDestinatie() << "," << cursa.getPlecare() << "," << cursa.getData() << "," << cursa.getOra() << "," << cursa.getClasa() << "," << cursa.getNumarLocuri() << endl;
        outFile.close();
        cout << "Cursa a fost adaugata cu succes!!" << endl;
    }

    // Stergerea unei curse
    void stergeCursa(string destinatie, string plecare, string data, string ora)
    {
        ifstream inFile("curse.csv");
        if (!inFile.is_open())
        {
            throw runtime_error("Nu s-a putut deschide fisierul pentru citire.");
        }

        ofstream tempFile("temp.csv"); // fisier temporar pentru a rescrie datele
        if (!tempFile.is_open())
        {
            throw runtime_error("nu s a putut crea fisierul temporar");
        }

        string line;
        bool found = false;

        while (getline(inFile, line))
        {
            stringstream ss(line);
            string fileDestinatie, filePlecare, fileData, fileOra;

            // extragem campurile din fisierul CSV
            getline(ss, fileDestinatie, ',');
            getline(ss, filePlecare, ',');
            getline(ss, fileData, ',');
            getline(ss, fileOra, ',');

            // comparam linia cu criteriile de stergere
            if (fileDestinatie == destinatie && fileData == data && filePlecare == plecare && fileOra == ora)
            {
                found = true; // a fost gasita cursa care trebuie stearsa
            }
            else // daca linia este diferita de ceea ce vrem noi sa stergem, nu necesita stergere
            {
                tempFile << line << endl; // o adauugam inapoi in fieiser, deoarece trebuie pastrata
            }
        }

        inFile.close();
        tempFile.close();

        if (!found)
        {
            remove("temp.csv"); // stergem fisierul temporar daca nu am gasit nimic
            throw invalid_argument("Cursa specificata nu a fost gasita");
        }

        // inlocuim fisierul original cu cel temporar
        if (remove("curse.csv") != 0 || rename("temp.csv", "curse.csv") != 0)
        {
            throw runtime_error("eroare la actualizarea fisierului curse.csv");
        }

        cout << "cursa a fost stearsa cu succes!" << endl;
    }
};

// clasa pentru Utilizator
class Utilizator
{
private:
    string username;
    string email;
    string password;

public:
    Utilizator(string user, string mail, string pass) : username(user), email(mail), password(pass) {}

    // functia pentru email valid
    bool isValidEmail(const string &email)
    {
        // check the first character if is an alphabet or not
        // if is not, email is not valid
        if (!isalpha(email[0]))
        {
            throw invalid_argument("Email invalid: primul caracter nu este o litera!");
        }

        // variable to store position of At=@ and Dot=.
        int At = -1, Dot = -1;

        // traverse over the email id stringto find position of Dot and At
        for (int i = 0; i < email.length(); i++)
        {
            // if the carcter is '@'
            if (email[i] == '@')
            {
                At = i;
            }

            // if character is '.'
            if (email[i] == '.')
            {
                Dot = i;
            }
        }

        // if At or Dot is not present
        if (At == -1)
        {
            throw invalid_argument("email invalid: lipseste caracterul @ din componenta sa");
        }
        if (Dot == -1)
        {
            throw invalid_argument("email invalid: lipseste caracterul . din componenta sa");
        }
        // if Dot is present before At
        if (At > Dot)
        {
            throw invalid_argument("email invalid: caracterul . apare inaintea caracterului @");
        }
        // if Dot is presen at the end
        if (Dot >= (email.length() - 1))
        {
            throw invalid_argument("email invalid: '.' nu poate fi ultimul caracter");
        }
        return true;
    }
    // functie pentru validarea parolei
    string passwordStrength(const string &password)
    {
        bool hasLower = false, hasUpper = false, hasDigit = false, hasSpecial = false;
        if (password.size() < 8)
        {
            throw invalid_argument("password is too short. Minimum length is 8 characters.");
        }

        for (char c : password)
        { // aici ne ia fiecare caracter din parola si il verifica
            if (islower(c))
                hasLower = true;
            if (isupper(c))
                hasUpper = true;
            if (isdigit(c))
                hasDigit = true;
            if (ispunct(c))
                hasSpecial = true; // carcatere speciale
        }

        int strength = hasLower + hasUpper + hasDigit + hasSpecial;

        if (strength < 2)
        {
            throw invalid_argument("Password is weak.");
        }
        else if (strength == 2)
        {
            return "ok";
        }
        else
        {
            return "good";
        }
    }
    // functie pentru a verifica daca username ul sau emeil ul exista deja in fisierul utilizator.csv
    bool isDuplicate(const string &username, const string &email)
    {
        ifstream inFile("utilizator.csv");

        if (!inFile.is_open())
        {
            throw runtime_error("Nu s a putut deschide fisierul");
            ;
        }

        string line;
        while (getline(inFile, line))
        {
            stringstream ss(line);
            string existingUsername, existingEmail;

            getline(ss, existingUsername, ',');
            getline(ss, existingEmail, ',');

            if (existingUsername == username || existingEmail == email)
            {
                return true;
            }
        }

        return false;
    }

    void autentificare(string username, string email, string parola)
    {
        // verificam daca formatul email-ului este corect
        isValidEmail(email);

        // confirmarea parolei
        cout << "va rugam sa introduceti din nou aceeasi parola pentru confirmare: ";
        cin.ignore(); // Curatam buffer-ul pentru a permite utilizarea getline
        string parolaConfirmare;
        getline(cin, parolaConfirmare);

        while (parolaConfirmare != parola) // Continuăm să cerem parola dacă nu se potrivește
        {
            cout << "cele doua parole nu corespund. Incercati din nou";
            getline(cin, parolaConfirmare);
        }
        // criptam parola
        string encryptedPassword = encryptVigenere(parola, "key");

        // trebuie toate datele introduse in csv adica username, email si parola
        ofstream outFile("utilizator.csv", ios::app); // deschidem fisierul pentru a scrie  (append)
        if (!outFile.is_open())
        {
            throw runtime_error("nu s a putut deschide fisierul");
        }

        outFile << username << "," << email << "," << encryptedPassword << endl;
        outFile.close();
        cout << "ati fost autentificat cu succes";
    }

    // Login pentru utilizator
    void login(string user, string pass)
    {
        // citim din fisierul csv
        ifstream file("utilizator.csv");
        if (!file.is_open())
        {
            throw runtime_error("Nu s-a putut deschide fisierul");
        }

        string line, fileUser, fileEmail, filePass;
        bool found = false;
        // criptam parola introdusa
        pass = encryptVigenere(pass, "key");

        while (getline(file, line)) // Citește o linie completă
        {
            stringstream ss(line);       // transforma linia intr-un flux
            getline(ss, fileUser, ',');  // Citește username-ul
            getline(ss, fileEmail, ','); // Citește email-ul
            getline(ss, filePass, ',');  // Citește parola criptată

            if (fileUser == user && filePass == pass)
            { // verifica login-ul
                found = true;
                break;
            }
        }
        file.close();

        if (!found)
        {
            throw invalid_argument("LOGIN NEREUSIT!! Username sau parola gresita!"); // aici as putea sa fac cu un tr catch sa le introduca din nou
        }
    }

    // rezervare loc, specificat clasa trenului si ora rezervarii si se adauga utilizatorii noi i csv (adica ii punem pe locuri zic eu ), bun si trebuie sa avem o parte
    void rezervareLoc(string username, string destinatie, string plecare, string data, string ora, int clasa, int nrlocuri)
    {
        // introducere detalii gresite la rezervare cursa tren
        // trebuie sa ii aratam utilizatorului ce curse sunt disponibile si cate locuri pe care le luam din csv ul operatorului

        ifstream inFile("curse.csv");
        if (!inFile.is_open())
        {
            throw runtime_error("Nu s-a putut deschide fisierul curse.csv");
            return;
        }

        // deschidem un fisier temporar pentru a actualiza informatiile despre locuri
        ofstream tempFile("curse_temp.csv");
        if (!tempFile.is_open())
        {
            throw runtime_error("nu s a putut deschide fisierul temporar curse_temp.csv");
        }

        // verificam daca exista cursa in fiserul curse.csv
        bool cursaGasita = false; // pt a determina daca s a gasit cursa

        // afisam tot ce se afla in fisierul curese.csv
        string line;
        while (getline(inFile, line))
        {
            stringstream ss(line);
            string fileDestinatie, filePlecare, fileData, fileOra;
            int fileClasa, fileNumarLocuri;

            // citim campurile din linie
            getline(ss, fileDestinatie, ',');
            getline(ss, filePlecare, ',');
            getline(ss, fileData, ',');
            getline(ss, fileOra, ',');

            ss >> fileClasa;
            ss.ignore();
            ss >> fileNumarLocuri;

            // Verificăm dacă aceasta este cursa căutată
            if (fileDestinatie == destinatie && filePlecare == plecare && fileData == data && fileOra == ora && fileClasa == clasa)
            {
                cursaGasita = true;
                if (fileNumarLocuri > nrlocuri)
                {
                    // actualizam nummarul de locuri disponibile
                    fileNumarLocuri -= nrlocuri;
                    cout << "Locuri rezervat cu succes! Locuri ramase: " << fileNumarLocuri << endl;
                }
                else
                {
                    cout << "Nu mai sunt locuri disponibile pentru aceasta cursa!" << endl;
                    tempFile << line << endl; // scriem linia originala
                    continue;                 // sarim la urmatoarea linie
                }
            }
            // scriem linia (modificata sau nemodificata) in fisierul temporar

            tempFile << fileDestinatie << "," << filePlecare << "," << fileData << "," << fileOra << "," << fileClasa << "," << fileNumarLocuri << endl;
        }

        inFile.close();
        tempFile.close();

        // inlocuim fisierul original cu cel temporar
        remove("curse.csv");
        rename("curse_temp.csv", "curse.csv");

        if (!cursaGasita)
        {
            cout << "cursa specificata nu exista" << endl;
            return;
        }

        // adaugam detaliile rezervarii in fisierul rezervari
        ofstream rezervariOut("rezervari.csv", ios::app);
        if (!rezervariOut.is_open())
        {
            throw runtime_error("nu s a putut deschide fisierul rezervari");
        }

        rezervariOut << username << "," << destinatie << "," << plecare << "," << data << "," << ora << "," << clasa << endl;
        rezervariOut.close();
    }
};

void afiseazaCurseDisponibile()
{
    ifstream inFile("curse.csv");
    if (!inFile.is_open())
    {
        throw runtime_error("Nu s-a putut deschide fisierul curse.csv");
    }

    cout << "CURSELE DISPONIBILE SUNT:\n";
    string line;
    while (getline(inFile, line))
    {
        stringstream ss(line);
        string destinatie, plecare, data, ora;
        int clasa, locuri;

        // Citim câmpurile din linie
        getline(ss, destinatie, ',');
        getline(ss, plecare, ',');
        getline(ss, data, ',');
        getline(ss, ora, ',');
        ss >> clasa;
        ss.ignore();
        ss >> locuri;

        // Afișăm cursa curentă
        cout << destinatie << " -> " << plecare << " | Data: " << data
             << " | Ora: " << ora << " | Clasa: " << clasa
             << " | Locuri disponibile: " << locuri << endl;
    }

    inFile.close();
}

// functia main
int main()
{
    try
    {
        string userType;
        cout << "Bine ati venit!" << endl;

        // alegerea rolului: Operator sau Utilizator
        while (true)
        {
            cout << "\nSelectati rolul dumneavoastra operator sau utilizator sau tastati exit pentru a iesi: ";
            cin >> userType;

            if (userType == "exit")
            {
                cout << "La revedere!" << endl;
            }

            if (userType == "operator")
            {
                // gestionare aplicatie pentru operator
                string username, password;
                cout << "Introduceti username-ul operatorului: ";
                cin >> username;
                cout << "Introduceti parola operatorului: ";
                cin >> password;

                // cream un obiect pentru clasa operator
                Operator op(username, password);

                try
                {
                    op.login(username, password);
                    cout << "Login reusit pentru operator!" << endl;
                    int optiune;
                    do
                    {
                        cout << "\nMeniu operator:\n";
                        cout << "1. Adauga cursa\n";
                        cout << "2. Sterge cursa\n";
                        cout << "3. Logout\n";
                        cout << "Selectati o optiune: ";
                        cin >> optiune;

                        if (optiune == 1) // adaugam cursa
                        {
                            Cursa cursa; // instantierea unui obiect de tip cursa

                            string destinatie, plecare, data, ora;
                            int clasa, locuri;

                            cout << "introduceti destinatia: ";
                            cin.ignore(); // elimina caracterele ramase in buffer
                            getline(cin, destinatie);
                            cursa.setDestinatie(destinatie);

                            cout << "Introduceti locul de plecare: ";
                            getline(cin, plecare);
                            cursa.setPlecare(plecare);

                            cout << "introduceti data (DD/MM/YYYY): ";
                            cin >> data;
                            cursa.setData(data);

                            cout << "Introduceti ora (HH:MM): ";
                            cin >> ora;
                            cursa.setOra(ora);

                            cout << "Introduceti clasa trenului (1 sau 2): ";
                            cin >> clasa;
                            cursa.setClasa(clasa);

                            cout << "Introduceti numarul de locuri: ";
                            cin >> locuri;
                            cursa.setNumarLocuri(locuri);

                            op.adaugaCursa(cursa);
                        }
                        else if (optiune == 2)
                        {
                            string destinatie, plecare, data, ora;
                            cout << "introduceti destinatia cursei de sters: ";
                            cin.ignore();
                            getline(cin, destinatie);
                            cout << "introduceti locul de plecare: ";
                            getline(cin, plecare);
                            cout << "introduceti data cursei de sters(DD/MM/YYYY): ";
                            cin >> data;
                            cout << "introduceti ora cursei de stres (HH:MM): ";
                            cin >> ora;
                            // nu mai punem clasa deoarece daca se anuleaza cursa se va anula pentru ambele clasa, deci trebuie sa fie sters in ambele cazuri

                            op.stergeCursa(destinatie, plecare, data, ora);
                        }
                    } while (optiune != 3);
                }
                catch (const exception &e)
                {
                    cout << "Eroare: " << e.what() << endl;
                }
            }
            else if (userType == "utilizator")
            {
                // gestionarea pentru utilizator
                string username, email, password;

                cout << "1. Autentificare utilizator nou\n";
                cout << "2. Login utilizator existent\n";

                int optiuneUtilizator;
                cin >> optiuneUtilizator;

                cout << "introduceti username-ul";
                cin >> username;

                Utilizator utilizator(username, "", ""); // obiectul

                if (optiuneUtilizator == 1)
                {
                    cout << "introduceti email: ";
                    cin >> email;

                    // verificam daca username ul sau email ul exista deja in fisierul utilizator.csv
                    try
                    {
                        // verificam daca username ul sau email ul exista deja
                        while (utilizator.isDuplicate(username, email))
                        {
                            cout << "Username-ul sau emailul sunt deja folosite. Introduceti altele:\n";
                            cout << "Nume utilizator: ";
                            cin >> username;
                            cout << "Email: ";
                            cin >> email;
                            utilizator.isValidEmail(email); // validam din nou email ul
                        }
                    }
                    catch (const invalid_argument &e)
                    {
                        cout << "Eroare: " << e.what() << endl;
                    }
                    catch (const runtime_error &e)
                    {
                        cout << "Eroare la citirea fisierului: " << e.what() << endl;
                    }

                    // Buclă pentru cererea parolei, doar daca username ul si email ul sunt valide
                    bool passwordSet = false;
                    while (!passwordSet)
                    {
                        cout << "Introduceti parola: ";
                        cin >> password;

                        try
                        {
                            string strength = utilizator.passwordStrength(password);
                            cout << "Password strength: " << strength << endl;
                            passwordSet = true; // flag pentru a iesi din bucla
                        }
                        catch (const invalid_argument &e)
                        {
                            cout << "Error: " << e.what() << endl;
                            cout << "Please try again.\n";
                        }
                    }
                    cout << "Password succesfully set!\n";
                    try
                    {
                        utilizator.autentificare(username, email, password);
                    }
                    catch (const exception &e)
                    {
                        cout << "Eroare: " << e.what() << endl;
                    }
                }
                else if (optiuneUtilizator == 2)
                {
                    try
                    {
                        cout << "introduceti parola: ";
                        cin >> password;
                        utilizator.login(username, password);
                        cout << "Login reusit!" << endl;

                        // rezervare loc
                        cout << "\nDoriti sa rezervati o calatorie? (1 pentru DA 0 pentru NU): ";
                        int rezervareOptiune;
                        cin >> rezervareOptiune;

                        if (rezervareOptiune == 1)
                        {
                            // Afișăm cursele disponibile
                            afiseazaCurseDisponibile();

                            string destinatie, plecare, data, ora;
                            int clasa, locuri;
                            cout << "Introduceti destinatia: ";
                            cin.ignore();
                            getline(cin, destinatie);
                            cout << "Introduceti locul de plecare: ";
                            getline(cin, plecare);
                            cout << "Introduceti data (DD/MM/YYYY): ";
                            cin >> data;
                            cout << "Introduceti ora (HH:MM): ";
                            cin >> ora;
                            cout << "Introduceti clasa trenului (1 sau 2): ";
                            cin >> clasa;
                            cout << "Introduceti numarul de locuri pe care doriti sa le rezervati: ";
                            cin >> locuri;

                            utilizator.rezervareLoc(username, destinatie, plecare, data, ora, clasa, locuri);
                        }
                    }
                    catch (const exception &e)
                    {
                        cout << "Eroare la login: " << e.what() << endl;
                    }
                }
            }
            else
            {
                cout << "rol invalid. incercati din nou" << endl;
            }
        }
    }
    catch (const exception &e)
    {
        cout << "Eroare: " << e.what() << endl;
    }

    return 0;
}