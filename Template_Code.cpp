#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>
#include <limits>
#include <algorithm> 

#ifdef _WIN32
    #include <conio.h>
#else
    #include <termios.h>
    #include <unistd.h>
#endif

using namespace std;

// HELPER FUNCTIONS
bool validatePassword(string pwd) {
    return pwd.length() >= 6;
}

string getHiddenPassword(const string& prompt) {
    cout << prompt;
    string pwd;

#ifdef _WIN32
    char ch;
    while ((ch = _getch()) != '\r') {
        if (ch == '\b') { 
            if (!pwd.empty()) {
                pwd.pop_back();
                cout << "\b \b";
            }
        } else {
            pwd.push_back(ch);
            cout << '*';
        }
    }
#else
    termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    char ch;
    while (cin.get(ch) && ch != '\n') {
        if (ch == 127 || ch == '\b') {
            if (!pwd.empty()) {
                pwd.pop_back();
                cout << "\b \b";
            }
        } else {
            pwd.push_back(ch);
            cout << '*';
        }
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif

    cout << endl;
    return pwd;
}

// ========================
//        USER CLASS
// ========================
class User {
private:
    string username;
    string password;
    string phoneNumber;
    string status;
    string lastSeen;

public:
    User() {
        username = "";
        password = "";
        phoneNumber = "";
        status = "";
        updateLastSeen();
    }

    User(string uname, string pwd, string phone) {
        username = uname;
        password = pwd;
        phoneNumber = phone;
        status = "Hey there! I am using WhatsApp.";
        updateLastSeen();
    }

    string getUsername() const {
        return username;
    }

    string getPhoneNumber() const {
        return phoneNumber;
    }

    string getStatus() const {
         return status;
    }

    string getLastSeen() const {
        return lastSeen;
    }

    void setStatus(string newStatus) {
        status = newStatus;
        updateLastSeen();
    }

    void setPhoneNumber(string phone) {
        phoneNumber = phone;
        updateLastSeen();
    }

    void updateLastSeen() {
        time_t now = time(nullptr);
        string t = ctime(&now);
        if (!t.empty() && t.back() == '\n') t.pop_back();
        lastSeen = t;
    }

    bool checkPassword(string pwd) const {
        return password == pwd;
    }

    void changePassword(string newPwd) {
        while (!validatePassword(newPwd)) {
            cout << "Invalid Password. Try Again." << endl;
            getline(cin, newPwd);
        }

        this->password = newPwd;
        cout << "Password Changed Successfully!" << endl;
        updateLastSeen();
    }
};

// Struct to safely hold reply details without holding pointers to dynamic/vector objects
struct ReplySnapshot {
    string sender;
    string content;
};

// ========================
//      MESSAGE CLASS
// ========================
class Message {
private:
    string sender;
    string content;
    string timestamp;
    string status;
    bool holdsReply;
    ReplySnapshot replyData;

public:
    Message() {
        sender = "";
        content = "";
        status = "";
        holdsReply = false;
        updateTimestamp();
    }
    
    Message(string sndr, string cntnt) { 
        sender = sndr;
        content = cntnt;
        status = "Sent";
        holdsReply = false;
        updateTimestamp();
    }

    string getContent() const {
        return content;
    }

    string getSender() const {
        return sender;
    }

    string getTimestamp() const {
        return timestamp;
    }

    string getStatus() const {
        return status;
    }

    bool hasReply() const {
        return holdsReply;
    }

    ReplySnapshot getReplyData() const {
        return replyData;
    }

    void setStatus(string newStatus) {
        status = newStatus;
    }

    void markAsRead() {
        status = "Read";
    }

    // Capture the target message snapshot directly to detach lifecycle dependency
    void setReplyTo(const Message& msg) {
        replyData.sender = msg.getSender();
        replyData.content = msg.getContent();
        holdsReply = true;
    }

    void updateTimestamp() {
        time_t now = time(nullptr);
        string t = ctime(&now);
        if (!t.empty() && t.back() == '\n') t.pop_back();
        timestamp = t;
    }

    void display() const {
        if (holdsReply) {
            cout << "   \u21B3 Replying to " << replyData.sender
                 << ": \"" << replyData.content << "\"" << endl;
        }

        string icon = "✓";
        if (status == "Delivered") {
            icon = "✓✓";
        }
        else if (status == "Read") {
            icon = "✓✓ (Read)";
        }

        cout << "[" << timestamp << "] " << sender << ": " << content << " " << icon << endl;
    }
};

// ========================
//        CHAT CLASS (BASE)
// ========================
class Chat {
protected:
    vector<string> participants;
    vector<Message> messages;
    string chatName;

public:
    Chat() {
        participants = {};
        messages = {};
        chatName = "";
    }

    Chat(vector<string> users, string name) {
        participants = users;
        chatName = name;
    }
    
    virtual ~Chat() {}

    bool isMember(const string& username) const {
        for (const auto& p : participants) {
            if (p == username) return true;
        }
        return false;
    }

    void markAllAsRead(const string& viewer) {
        for (Message& msg : messages) {
            if (msg.getSender() != viewer) {
                msg.markAsRead();
            }
        }
    }

    void addMessage(const Message& msg) {
        messages.push_back(msg);
        messages[messages.size() - 1].setStatus("Delivered");
    }

    bool deleteMessage(int index, const string& username) {
        if (index < 0 || index >= (int)messages.size()) {
            return false;
        }
        if (messages[index].getSender() != username) {
            return false;
        }

        // Erasing from vector invalidates iterators/indexes, 
        // but existing Message replySnapshots remain completely untouched!
        messages.erase(messages.begin() + index);
        return true;
    }

    virtual void displayChat() const {
        cout << chatName << endl;
        for (const Message& msg : messages) {
            msg.display();
        }
    }
    
    string getChatName() const {
        return chatName;
    }

    vector<Message> searchMessages(string keyword) const {
        vector<Message> results;
        for (const Message& m : messages) {
            if (m.getContent().find(keyword) != string::npos) {
                results.push_back(m);
            }
        }
        return results;
    }
    
    void exportToFile(const string& filename) const {
        ofstream out(filename);
        if (!out.is_open()) {
            cout << "Failed to open file: " << filename << endl;
            return;
        }
        out << "Chat: " << chatName << "\n";
        out << "Participants: ";
        for (size_t i = 0; i < participants.size(); ++i) {
            out << participants[i];
            if (i + 1 < participants.size()) out << ", ";
        }
        out << "\n\n";
        for (const Message& m : messages) {
            if (m.hasReply()) {
                out << "   [Replying to " << m.getReplyData().sender 
                    << ": \"" << m.getReplyData().content << "\"]\n";
            }
            out << "[" << m.getTimestamp() << "] "
                << m.getSender() << ": " << m.getContent() << "\n";
        }
        out.close();
        cout << "Chat exported to " << filename << endl;
    }
};

// ========================
//      PRIVATE CHAT CLASS
// ========================
class PrivateChat : public Chat {
private:
    string user1;
    string user2;

public:
    PrivateChat(string u1, string u2) 
        : Chat({u1, u2}, "Chat between " + u1 + " and " + u2) {
       user1 = u1;
       user2 = u2;
    }

    void displayChat() const override {
        cout << "\n--- Private Chat: " << user1 << " & " << user2 << " ---\n";
        for (size_t i = 0; i < messages.size(); i++) {
            messages[i].display();
        }
    }
};

// ========================
//      GROUP CHAT CLASS
// ========================
class GroupChat : public Chat {
private:
    vector<string> admins;
    string description;

public:
    GroupChat(vector<string> users, string name, string description, string creator) 
        : Chat(users, name) { 
            this->description = description;
            admins.push_back(creator); 
    }

    bool isAdmin(string username) const {
        for (const auto& a : admins) {
            if (a == username) return true;
        }
        return false;
    }

    bool isParticipant(string username) const {
        return isMember(username);
    }
    
    string getDescription() const {
        return description;
    }

    void setDescription(string desc) {
        description = desc;
    }

    void displayChat() const override {
        cout << "\nGroup: " << chatName << endl;
        cout << "Participants: ";
        for (const auto& p : participants) {
            cout << p << " ";
        }
        cout << endl;
        cout << "Admins: ";
        for (const auto& a : admins) {
            cout << a << " ";
        }
        cout << endl;
        cout << "Description: " << (description.empty() ? "No description available" : description) << "\n";

        Chat::displayChat();
    } 
};

// ========================
//     WHATSAPP APP CLASS
// ========================
class WhatsApp {
private:
    vector<User> users;
    vector<Chat*> chats;
    int currentUserIndex;
    bool loggedIn = false;
    
    int findUserIndex(string username) const {
        for (size_t i = 0; i < users.size(); ++i) {
            if (users[i].getUsername() == username) {
                return i;
            }
        }
        return -1;
    }

    bool isLoggedIn() const {
        return loggedIn;
    }

    string getCurrentUsername() const {
        if (isLoggedIn()) {
            return users[currentUserIndex].getUsername();
        }
        return "";
    }

public:
    WhatsApp() : currentUserIndex(-1) {}

    ~WhatsApp() {
        for (Chat* c : chats) {
            delete c;
        }
        chats.clear();
    }

    void signUp() {
        User newUser;
        string uname, pwd, phone;
        bool valid;

        do {
            cout << "Enter username: ";
            getline(cin, uname);
            valid = validateUsername(uname);
            if (!valid) cout << "Invalid username. Please try again." << endl;
        } while (!valid);

        do {
            pwd = getHiddenPassword("Enter password: ");
            valid = validatePassword(pwd);
            if (!valid) cout << "Invalid password. Please try again." << endl;
        } while (!valid);

        do {
            cout << "Enter phone number: ";
            getline(cin, phone);
            valid = validatePhone(phone);
            if (!valid) cout << "Invalid phone number. Please try again." << endl;
        } while (!valid);

        newUser = User(uname, pwd, phone);
        users.push_back(newUser);
    }

    bool validateUsername(string uname) {
        for (User user: users) {
            if (user.getUsername() == uname) {
                return false;
            }
        }
        return true;
    }
    
    bool validatePhone(string phone, int excludeUserIndex = -1) {
        if (phone.length() != 11) {
            cout << "Phone number must be 11 digits long." << endl;
            return false;
        }
        if (phone[0] != '0' || phone[1] != '1') {
            cout << "Phone number must start with '01'." << endl;
            return false;
        }

        for (size_t i = 0; i < users.size(); ++i) {
            if ((int)i == excludeUserIndex) continue;
            if (users[i].getPhoneNumber() == phone) {
                cout << "Phone number already exists. Please use a different phone number." << endl;
                return false;
            }
        }

        return true;
    }

    void login() {
        string uname, pwd;
        cout << "\n=== User Login ===\n";
        cout << "Enter Username: ";
        getline(cin, uname);
        
        int index = findUserIndex(uname);
        
        if (index == -1) {
            cout << "[Error] User not found!\n";
            return;
        }

        pwd = getHiddenPassword("Enter Password: ");
        
        if (users[index].checkPassword(pwd)) {
            currentUserIndex = index;
            users[currentUserIndex].setStatus("Online");
            users[currentUserIndex].updateLastSeen();
            loggedIn = true;
            cout << "\n[Success] Logged in successfully! Welcome, " << getCurrentUsername() << "!\n";
        } else {
            cout << "[Error] Incorrect password!\n";
        }
    }

    void createGroup() {
        vector<string> participants;
        string groupName;
        string description;
        
        cout << "Enter Group Name: ";
        getline(cin, groupName);

        if (find(participants.begin(), participants.end(), getCurrentUsername()) == participants.end()) {
            participants.push_back(getCurrentUsername());
        }

        if (groupName.empty()) {
            cout << "Group name cannot be empty." << endl;
            return;
        }
        if (participants.size() < 2) {
            cout << "Group must have at least 2 participants." << endl;
            return;
        }

        GroupChat* newGroup = new GroupChat(participants, groupName, description, getCurrentUsername());
        chats.push_back(newGroup);

        users[currentUserIndex].updateLastSeen();
    }

    void viewChats() const {
        string current = getCurrentUsername();
        for (Chat* chat : chats) {
            if (chat && chat->isMember(current)) {
                chat->markAllAsRead(current);
                chat->displayChat();
            }
        }
    }
    
    void exportChat() {
        if (chats.empty()) {
            cout << "No chats to export." << endl;
            return;
        }
        cout << "\nSelect a chat to export:\n";
        for (size_t i = 0; i < chats.size(); ++i) {
            cout << (i + 1) << ". " << chats[i]->getChatName() << "\n";
        }
        cout << "Choice: ";
        int idx;
        cin >> idx;
        if (idx < 1 || idx > (int)chats.size()) {
            cout << "Invalid choice." << endl;
            return;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Filename: ";
        string filename;
        getline(cin, filename);
        chats[idx - 1]->exportToFile(filename);
        users[currentUserIndex].updateLastSeen();
    }

    void logout() {
        users[currentUserIndex].updateLastSeen();
        currentUserIndex = -1;
        loggedIn = false;
    }

    void run() {
        while (true) {
            if (!isLoggedIn()) {
                cout << "\n1. Login\n2. Sign Up\n3. Exit\nChoice: ";
                int choice;
                cin >> choice;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                if (choice == 1) login();
                else if (choice == 2) signUp();
                else if (choice == 3) break;
            }
            else {
                cout << "\n1. Create Group\n2. View Chats\n3. Edit Account\n4. Export Chat\n5. Logout\nChoice: ";
                int choice;
                cin >> choice;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                switch (choice) {
                    case 1:
                        createGroup();
                        break;
                    case 2:
                        viewChats();
                        break;
                    case 3: {
                        bool editing = true;
                        while (editing) {
                            cout << "\n--- Edit Account ---\n";
                            cout << "1. Change Status\n";
                            cout << "2. Change Phone Number\n";
                            cout << "3. Change Password\n";
                            cout << "4. Back to Main Menu\n";
                            cout << "Choice: ";

                            int editChoice;
                            cin >> editChoice;
                            cin.ignore(numeric_limits<streamsize>::max(), '\n');

                            switch (editChoice) {
                                case 1: {
                                    string newStatus;
                                    cout << "Enter new status: ";
                                    getline(cin, newStatus);
                                    users[currentUserIndex].setStatus(newStatus);
                                    cout << "Status updated successfully.\n";
                                    break;
                                }
                                case 2: {
                                    string newPhone;
                                    do {
                                        cout << "Enter new phone number: ";
                                        getline(cin, newPhone);
                                    } while (!validatePhone(newPhone, currentUserIndex));

                                    users[currentUserIndex].setPhoneNumber(newPhone);
                                    cout << "Phone number updated successfully.\n";
                                    break;
                                }
                                case 3: {
                                    string newPass = getHiddenPassword("Enter new password: ");
                                    users[currentUserIndex].changePassword(newPass);
                                    break;
                                }
                                case 4:
                                    editing = false;
                                    break;
                                default:
                                    cout << "Invalid choice. Please try again.\n";
                            }
                        }
                        break;
                    }
                    case 4:
                        exportChat();
                        break;
                    case 5:
                        logout();
                        break;
                    default:
                        cout << "Invalid choice. Please try again." << endl;
                }
            }
        }
    }
};

// ========================
//          MAIN
// ========================
int main() {
    WhatsApp whatsapp;
    whatsapp.run();
    return 0;
}