#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using namespace std;

string filename = "Clients.txt";
string delim = "#//#";

struct sclient
{
    string account_number;
    string pincode;
    string name;
    string phone;
    bool mark_for_update = true;
    int balance;
};

sclient current_client;

vector<string> split_string(const string& str, const string& delim) {
    vector<string> vString;
    size_t start = 0, end;

    while ((end = str.find(delim, start)) != string::npos) {
        string part = str.substr(start, end - start);
        if (!part.empty()) {
            vString.push_back(part);
        }
        start = end + delim.length();
    }

    if (start < str.length()) {
        vString.push_back(str.substr(start));
    }

    return vString;
}

sclient ConvertLinetoRecord(const string& line) {
    sclient client;
    vector<string> vClientData = split_string(line, delim);

    if (vClientData.size() == 5) {
        client.account_number = vClientData[0];
        client.pincode = vClientData[1];
        client.name = vClientData[2];
        client.phone = vClientData[3];
        client.balance = stoi(vClientData[4]);
    }

    return client;
}

vector<sclient> LoadClientsDataFromFile() {
    vector<sclient> vclient;
    fstream file(filename, ios::in);

    if (file.is_open()) {
        string line;

        while (getline(file, line)) {
            sclient client = ConvertLinetoRecord(line);
            vclient.push_back(client);
        }

        file.close();
    }
    else {
        cerr << "Error: Unable to open file for reading.\n";
    }

    return vclient;
}

bool is_client_in_database(sclient& client, string accountn, string password) {
    vector<sclient> vclient = LoadClientsDataFromFile();

    for (sclient& C : vclient) {
        if (accountn == C.account_number && password == C.pincode) {
            client = C;
            return true;
        }
    }

    return false;
}

string convert_client_record_to_data(const sclient& client) {
    return client.account_number + delim +
        client.pincode + delim +
        client.name + delim +
        client.phone + delim +
        to_string(client.balance);
}

vector<sclient> update_clients_in_file(vector<sclient>& vclient, string FILENAME)
{
    fstream file;
    file.open(FILENAME, ios::out);  // Open the file in output mode to overwrite it

    if (file.is_open())
    {
        // Write all updated client data back into the file
        for (const sclient& c : vclient)
        {
            // Avoid writing empty client records with only delimiters
            if (!c.account_number.empty() && !c.pincode.empty() && !c.name.empty())
            {
                string dataline = convert_client_record_to_data(c);
                file << dataline << endl;  // Write client data
            }
        }
        file.close();
    }
    else {
        cerr << "Error: Unable to open file for writing.\n";
    }

    return vclient;
}



sclient process_withdrawal(int amount, vector<sclient>& vclient, const string& filename) {
    if (amount > current_client.balance) {
        cout << "\tThe amount exceeds your balance, you need to make a deposit\n";
        return current_client;
    }
    else {
        current_client.balance -= amount;

        // Now we need to update the file with the latest client data after withdrawal
        vector<sclient> updated_clients = LoadClientsDataFromFile();

        // Find the client and update it in the vector
        for (auto& client : updated_clients) {
            if (client.account_number == current_client.account_number && client.pincode == current_client.pincode) {
                client.balance = current_client.balance;
            }
        }

        // Now update the file with the updated list of clients
        update_clients_in_file(updated_clients, filename);

        cout << "\tOperation was done successfully.\n";
        return current_client;
    }
}

sclient process_deposit(vector<sclient>& vclient, const string& filename) {

    char answere;

    int amount = 0;
    cout << "\tEnter your deposit amount: "; cin >> amount; cout << endl;

    if (amount < 10) {
        cout << "\tThe amount is not valid, you need to make a deposit of at least $10\n";
        return current_client;
    }
    else {

        cout << "\tAre you sure you want to confirm this transaction? (y/n) ";
        cin >> answere;
        cout << endl;

        if (answere == 'y' || answere == 'Y')
        {
            current_client.balance += amount;

            // Now we need to update the file with the latest client data after withdrawal
            vector<sclient> updated_clients = LoadClientsDataFromFile();

            // Find the client and update it in the vector
            for (auto& client : updated_clients) {
                if (client.account_number == current_client.account_number && client.pincode == current_client.pincode) {
                    client.balance = current_client.balance;
                }
            }

            // Now update the file with the updated list of clients
            update_clients_in_file(updated_clients, filename);

            cout << "\tOperation was done successfully.\n";
            return current_client;
        }
        else
        {
            return current_client;
        }
    }
}


sclient withdraw(char withdraw_choice) {

    if (withdraw_choice == '9')
    {
        return current_client;
    }

    char answere;
    vector<sclient> vclient = LoadClientsDataFromFile();  // Reload clients from file

    cout << "\tAre you sure you want to confirm this transaction? (y/n) ";
    cin >> answere;
    cout << endl;

    do {
        if (answere == 'Y' || answere == 'y') {
            switch (withdraw_choice) {
            case '1': return process_withdrawal(10, vclient, filename);
                break;
            case '2': return process_withdrawal(20, vclient, filename);
                break;
            case '3': return process_withdrawal(50, vclient, filename);
                break;
            case '4': return process_withdrawal(100, vclient, filename);
                break;
            case '5': return process_withdrawal(150, vclient, filename);
                break;
            case '6': return process_withdrawal(200, vclient, filename);
                break;
            case '7': return process_withdrawal(500, vclient, filename);
                break;
            case '8': return process_withdrawal(1000, vclient, filename);
                break;
            default:
                break;
            }
        }
    } while (withdraw_choice != '9');

    return current_client;
}

sclient deposit() {
    vector<sclient> vclient = LoadClientsDataFromFile();  // Reload clients from file

    process_deposit(vclient, filename);
    
    return current_client;
}

sclient normal_withdraw(const string& filename) {
    int amount = 0;
    char answere;

    cout << "\tEnter the amount you want to be withdrawn "; cin >> amount; cout << endl;

    if (amount > current_client.balance) {
        cout << "\tThe amount exceeds your balance, you need to make a deposit\n";
        return current_client;
    }
    else if (amount % 5 != 0) {
        cout << "\tThe amount must be a multiplication of 5. retry... \n";
        return current_client;
    }
    else {

        cout << "\tAre you sure you want to confirm this transaction? (y/n) ";
        cin >> answere;
        cout << endl;

        if (answere == 'Y' || answere == 'y')
        {
            current_client.balance -= amount;

            // Now we need to update the file with the latest client data after withdrawal
            vector<sclient> updated_clients = LoadClientsDataFromFile();

            // Find the client and update it in the vector
            for (auto& client : updated_clients) {
                if (client.account_number == current_client.account_number && client.pincode == current_client.pincode) {
                    client.balance = current_client.balance;
                }
            }

            // Now update the file with the updated list of clients
            update_clients_in_file(updated_clients, filename);

            cout << "\tOperation was done successfully.\n";
            return current_client;
        }
        else
        {
            return current_client;
        }
    }
}

char show_main_menu() {
    char choice;

    cout << "===========================================\n";
    cout << "\t\tMain Menu Screen\n";
    cout << "===========================================\n";
    cout << "\t[1] Quick withdraw.\n";
    cout << "\t[2] Deposit.\n";
    cout << "\t[3] Normal withdraw.\n";
    cout << "\t[4] Check balance.\n";
    cout << "\t[5] Logout.\n";
    cout << "===========================================\n";

    cout << "\n\tEnter an option to proceed: "; cin >> choice; cout << endl;

    return choice;
}

char show_fast_withdraw_menu() {
    char choice;

    cout << "===========================================\n";
    cout << "\t\tFast withdraw Menu\n";
    cout << "===========================================\n";
    cout << "\t[1] $10,       [5] $150\n";
    cout << "\t[2] $20,       [6] $200\n";
    cout << "\t[3] $50,       [7] $500\n";
    cout << "\t[4] $100,      [8] $1000\n";
    cout << "\t[9] Back to main menu\n";
    cout << "===========================================\n";

    cout << "\n\tEnter an option to proceed: "; cin >> choice; cout << endl;

    return choice;
}

void check_balance() {

    cout << "\n\tYour balance: " << current_client.balance << endl;

}

void grant_access() {
    char mainchoice;
    char innerchoice;
    vector <sclient> vclient;

    do {
        system("cls");
        mainchoice = show_main_menu();

        switch (mainchoice) {
        case '1': {
            system("cls");
            innerchoice = show_fast_withdraw_menu();
            withdraw(innerchoice);
            system("pause");
            break;
        }
        case '2': {
            system("cls");
            deposit();
            system("pause");
            break;
        }
        case '3': {
            system("cls");
            normal_withdraw(filename);
            system("pause");
            break;
        }
        case '4': {
            system("cls");
            check_balance();
            system("pause");
            break;
        }
        default:
            break;
        }

    } while (mainchoice != '5');

    cout << "\tExiting the program\n";
    system("pause");
}

void login() {
    string account_n, passowrd;

    int login_attempts = 3;

    cout << "===========================================\n";
    cout << "\t\tLogin Screen\n";
    cout << "===========================================\n";

    while (login_attempts > 0) {
        cout << "\tEnter your account number: "; cin >> account_n; cout << endl;
        cout << "\tEnter your passowrd: "; cin >> passowrd; cout << endl;

        if (is_client_in_database(current_client, account_n, passowrd)) {
            grant_access();
            return;
        }
        else {
            login_attempts--;
            cout << "\n\tIncorrect credentials, please check your account number or password and try again\n";
            if (login_attempts > 0) {
                cout << "\n\tLogin attempts have decreased to: " << login_attempts << "\n";
            }
            else {
                cout << "\n\tToo many login attempts.. exiting the program\n";
                return;
            }
        }
    }
}

int main() {
    login();
    return 0;
}
