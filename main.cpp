#include <bits/stdc++.h>
using namespace std;

struct Account {
    int account_no;
    string name;
    char type; // 'S' for savings, 'C' for current
    double balance;
};

// File used to store accounts as CSV
const string DBFILE = "accounts.csv";

// Utility: trim whitespace (helpful when reading lines)
static inline string trim(const string &s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

// Parse a CSV line into Account (basic, no commas in names)
bool parseAccountCSV(const string &line, Account &acc) {
    // expected: account_no,name,type,balance
    stringstream ss(line);
    string token;
    if (!getline(ss, token, ',')) return false;
    acc.account_no = stoi(trim(token));
    if (!getline(ss, token, ',')) return false;
    acc.name = trim(token);
    if (!getline(ss, token, ',')) return false;
    string t = trim(token);
    acc.type = t.empty() ? 'S' : t[0];
    if (!getline(ss, token, ',')) return false;
    acc.balance = stod(trim(token));
    return true;
}

string accountToCSV(const Account &acc) {
    // ensure no commas in name (simple approach)
    string cleanName = acc.name;
    for (char &c : cleanName) if (c == ',') c = ' ';
    stringstream ss;
    ss << acc.account_no << "," << cleanName << "," << acc.type << "," << fixed << setprecision(2) << acc.balance;
    return ss.str();
}

// Load all accounts from file
vector<Account> loadAllAccounts() {
    vector<Account> arr;
    ifstream fin(DBFILE);
    if (!fin) return arr; // no file yet
    string line;
    while (getline(fin, line)) {
        if (trim(line).empty()) continue;
        Account a;
        if (parseAccountCSV(line, a)) arr.push_back(a);
    }
    return arr;
}

// Save all accounts to file (overwrite)
void saveAllAccounts(const vector<Account> &arr) {
    ofstream fout(DBFILE, ios::trunc);
    for (const auto &a : arr) {
        fout << accountToCSV(a) << "\n";
    }
}

// Generate next account number (1 + max existing)
int nextAccountNo() {
    auto arr = loadAllAccounts();
    int mx = 0;
    for (auto &a : arr) if (a.account_no > mx) mx = a.account_no;
    return mx + 1;
}

// Find index of account in vector by account_no, -1 if not found
int findAccountIndex(const vector<Account> &arr, int accno) {
    for (size_t i = 0; i < arr.size(); ++i) if (arr[i].account_no == accno) return (int)i;
    return -1;
}

// Menu operations
void createAccount() {
    Account acc;
    acc.account_no = nextAccountNo();
    cout << "\nCreating account. Account number will be: " << acc.account_no << "\n";
    cout << "Enter full name: ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, acc.name);
    cout << "Account type (S = Savings, C = Current): ";
    cin >> acc.type;
    acc.type = toupper(acc.type);
    while (acc.type != 'S' && acc.type != 'C') {
        cout << "Invalid type. Enter S or C: ";
        cin >> acc.type;
        acc.type = toupper(acc.type);
    }
    cout << "Initial deposit (minimum 0): ";
    cin >> acc.balance;
    while (acc.balance < 0) {
        cout << "Balance cannot be negative. Enter again: ";
        cin >> acc.balance;
    }

    auto arr = loadAllAccounts();
    arr.push_back(acc);
    saveAllAccounts(arr);
    cout << "Account created successfully!\n";
}

void displayAccount(const Account &a) {
    cout << "----------------------------------------\n";
    cout << "Account No : " << a.account_no << "\n";
    cout << "Name       : " << a.name << "\n";
    cout << "Type       : " << (a.type == 'S' ? "Savings" : "Current") << "\n";
    cout << "Balance    : " << fixed << setprecision(2) << a.balance << "\n";
    cout << "----------------------------------------\n";
}

void showAccount() {
    cout << "Enter account number: ";
    int accno; cin >> accno;
    auto arr = loadAllAccounts();
    int idx = findAccountIndex(arr, accno);
    if (idx == -1) {
        cout << "Account not found.\n";
        return;
    }
    displayAccount(arr[idx]);
}

void depositWithdraw(bool isDeposit) {
    cout << "Enter account number: ";
    int accno; cin >> accno;
    auto arr = loadAllAccounts();
    int idx = findAccountIndex(arr, accno);
    if (idx == -1) {
        cout << "Account not found.\n";
        return;
    }
    double amt;
    if (isDeposit) {
        cout << "Enter amount to deposit: ";
        cin >> amt;
        if (amt <= 0) { cout << "Amount must be positive.\n"; return; }
        arr[idx].balance += amt;
        cout << "Deposited successfully. New balance: " << fixed << setprecision(2) << arr[idx].balance << "\n";
    } else {
        cout << "Enter amount to withdraw: ";
        cin >> amt;
        if (amt <= 0) { cout << "Amount must be positive.\n"; return; }
        if (amt > arr[idx].balance) { cout << "Insufficient balance.\n"; return; }
        arr[idx].balance -= amt;
        cout << "Withdrawal successful. New balance: " << fixed << setprecision(2) << arr[idx].balance << "\n";
    }
    saveAllAccounts(arr);
}

void modifyAccount() {
    cout << "Enter account number to modify: ";
    int accno; cin >> accno;
    auto arr = loadAllAccounts();
    int idx = findAccountIndex(arr, accno);
    if (idx == -1) {
        cout << "Account not found.\n";
        return;
    }
    cout << "Current details:\n";
    displayAccount(arr[idx]);
    cout << "Enter new name (leave blank to keep): ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    string newName;
    getline(cin, newName);
    if (!trim(newName).empty()) arr[idx].name = newName;
    cout << "Enter new account type (S/C) or press Enter to keep [" << arr[idx].type << "]: ";
    string t;
    getline(cin, t);
    if (!trim(t).empty()) {
        char nt = toupper(t[0]);
        if (nt == 'S' || nt == 'C') arr[idx].type = nt;
        else cout << "Invalid type entered. Keeping previous type.\n";
    }
    cout << "Enter new balance or press Enter to keep [" << fixed << setprecision(2) << arr[idx].balance << "]: ";
    string balStr;
    getline(cin, balStr);
    if (!trim(balStr).empty()) {
        try {
            double nb = stod(balStr);
            if (nb >= 0) arr[idx].balance = nb;
            else cout << "Negative balance not allowed. Keeping previous.\n";
        } catch (...) {
            cout << "Invalid number. Keeping previous balance.\n";
        }
    }
    saveAllAccounts(arr);
    cout << "Account updated.\n";
}

void deleteAccount() {
    cout << "Enter account number to delete: ";
    int accno; cin >> accno;
    auto arr = loadAllAccounts();
    int idx = findAccountIndex(arr, accno);
    if (idx == -1) {
        cout << "Account not found.\n";
        return;
    }
    cout << "Account to be deleted:\n";
    displayAccount(arr[idx]);
    cout << "Are you sure? (Y/N): ";
    char c; cin >> c; c = toupper(c);
    if (c != 'Y') { cout << "Delete cancelled.\n"; return; }
    arr.erase(arr.begin() + idx);
    saveAllAccounts(arr);
    cout << "Account deleted.\n";
}

void displayAllAccounts() {
    auto arr = loadAllAccounts();
    if (arr.empty()) {
        cout << "No accounts found.\n";
        return;
    }
    cout << "\nAll accounts:\n";
    for (const auto &a : arr) displayAccount(a);
}

// Simple menu driver
void menu() {
    while (true) {
        cout << "\n======== Bank Management System ========\n";
        cout << "1. Create new account\n";
        cout << "2. Display account details\n";
        cout << "3. Deposit amount\n";
        cout << "4. Withdraw amount\n";
        cout << "5. Modify account\n";
        cout << "6. Delete account\n";
        cout << "7. List all accounts\n";
        cout << "0. Exit\n";
        cout << "Choose an option: ";
        int choice;
        if (!(cin >> choice)) {
            cout << "Invalid input. Try again.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        switch (choice) {
            case 1: createAccount(); b
