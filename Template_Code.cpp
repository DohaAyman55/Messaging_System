#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>
#include <limits>
#include <algorithm> 
using namespace std;

// HELPER FUNCTION
bool validatePassword(string pwd) {
   if (pwd.length() >= 6){
        return true;
    }else{
    return false;}
}

// ========================
//       USER CLASS
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
        // TODO: Implement default constructor
        // FR5 HOOK (please call at end): updateLastSeen();
        username = "";
        password = "";
        phoneNumber = "";
        status = "";
        updateLastSeen();
    }

    User(string uname, string pwd, string phone) {
        // TODO: Implement parameterized constructor
        // FR5 HOOK (please call at end): updateLastSeen();
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
        updateLastSeen(); // FR5
    }

    void setPhoneNumber(string phone) {
        phoneNumber = phone;
        updateLastSeen(); // FR5
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
        while(!validatePassword(newPwd)){
            cout << "Invalid Password. Try Again." << endl;
            getline(cin, newPwd);
        }

        this->password = newPwd;
        cout << "Password Changed Successfully!" << endl;
        updateLastSeen(); // FR5
        return;
    }
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
    Message* replyTo;

public:
    Message() {
        sender = "";
        content = "";
        status = "";
        replyTo = nullptr;
        updateTimestamp();
    }
    
    Message(string sndr, string cntnt) { 
        sender = sndr;
        content = cntnt;
        status = "Sent";
        replyTo = nullptr;
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

    Message* getReplyTo() const {
        return replyTo;
    }

    void setStatus(string newStatus) {
        status = newStatus;
    }

    void markAsRead() {
        status = "Read";
    }

    void setReplyTo(Message* msg) {
        replyTo = msg;
    }

    void updateTimestamp() {
        // TODO: Implement timestamp update
        // FR7
    }

    void display() const {
        if (replyTo != nullptr) {
            cout << "  \u21B3 Replying to " << replyTo->getSender()
                << ": \"" << replyTo->getContent() << "\"" << endl;
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

    void addEmoji(string emojiCode) {
        // TODO: Implement emoji support
    }
};

// ========================
//       CHAT CLASS (BASE)
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
    
    // ---- SCRUM-34: virtual so `delete chatPtr` runs the right destructor
    //      for PrivateChat / GroupChat (prevents UB and leaks) ----
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
        // TODO: Implement message addition
    }

    bool deleteMessage(int index, const string& username) {
        if (index < 0 || index >= (int)messages.size()) {
            return false;
        }
        if (messages[index].getSender() != username) {
            return false;
        }

        Message* deletedPtr = &messages[index];
        for (size_t i = 0; i < messages.size(); ++i) {
            if (messages[i].getReplyTo() == deletedPtr) {
                messages[i].setReplyTo(nullptr);
            }
        }

        messages.erase(messages.begin() + index);
        return true;
    }

    virtual void displayChat() const {
        cout << chatName << endl;
        for (const Message& msg : messages) {
            msg.display();
        }
    }
    
    // Accessor used by the export-chat menu option
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
    
    // ---- SCRUM-32: export chat to file ----
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
            out << "[" << m.getTimestamp() << "] "
                << m.getSender() << ": " << m.getContent() << "\n";
        }
        out.close();
        cout << "Chat exported to " << filename << endl;
    }
};

// ========================
//     PRIVATE CHAT CLASS
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
        for (int i = 0; i < messages.size(); i++) {
            messages[i].display();
            // TODO: Implement private chat display
        }
    }

    void showTypingIndicator(const string& username) const {
        // TODO: Implement typing indicator
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

    void addAdmin(string newAdmin) {
        // TODO: Implement add admin
    }

    bool removeParticipant(const string& admin, const string& userToRemove) {
        // TODO: Implement remove participant
        return false;
    }

    bool isAdmin(string username) const {
        for (const auto& a : admins) {
            if (a == username) {
                return true;
            }
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
            cout << p << endl;
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

    void sendJoinRequest(const string& username) {
        if (isParticipant(username)) {
        cout << username << " is already a participant in this group." << endl;
        return;
    }
        cout << username << " has requested to join the group \"" << chatName << "\"." << endl;
    }
};

// ========================
//    WHATSAPP APP CLASS
// ========================
class WhatsApp {
private:
    vector<User> users;
    vector<Chat*> chats;
    int currentUserIndex;
    bool loggedIn = false;
    
    int findUserIndex(string username) const {
        // TODO: Implement user search
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

    // ---- SCRUM-34: release all Chat* allocations (SRS §7.5, §10.5) ----
    ~WhatsApp() {
        for (Chat* c : chats) {
            delete c;
        }
        chats.clear();
    }

    void signUp() {
        // TODO: Implement user registration
        User newUser;
        string uname, pwd, phone;
        bool IsValidPassword = false;
        bool IsValidUsername = false;
        bool IsValidPhone = false;
        bool IsValidUserInputs = false;

        do {
            cout << "Enter username: ";
            getline(cin, uname);
            IsValidUsername = validateUsername(uname);
            if (!IsValidUsername) {
                cout << "Invalid username. Please try again." << endl;
            }
        } while (!IsValidUsername);
        do {
            cout << "Enter password: ";
            getline(cin, pwd);
            IsValidPassword = validatePassword(pwd);
            if (!IsValidPassword) {
                cout << "Invalid password. Please try again." << endl;
            }
        } while (!IsValidPassword);
        do {
            cout << "Enter phone number: ";
            getline(cin, phone);
            IsValidPhone = validatePhone(phone);
            if (!IsValidPhone) {
                cout << "Invalid phone number. Please try again." << endl;
            }
        } while (!IsValidPhone);
 
        IsValidUserInputs = IsValidPassword && IsValidUsername && IsValidPhone;
     
        
        if (IsValidUserInputs) {
            newUser = User(uname, pwd, phone);
            users.push_back(newUser);
        }
    }

    bool validateUsername(string uname) {
        for(User user: users){
            if(user.getUsername() == uname){
                return false;
            }
        }
        return true;
    }
    
    bool validatePhone(string phone, int excludeUserIndex  = -1) {
  
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
        // TODO: Implement user login
        // FR5 HOOK (please call after successful auth):
        //   users[currentUserIndex].updateLastSeen();
        // set logged in to true and currentUserIndex to the index of 
        // the logged-in user using findUserIndex()
        string uname, pwd;
        cout << "\n=== User Login ===\n";
        cout << "Enter Username: ";
        cin >> uname;
        
        int index = findUserIndex(uname);
        
        if (index == -1) {
            cout << "[Error] User not found!\n";
            return;
        }
        
        cout << "Enter Password: ";
        cin >> pwd;
        
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

    void startPrivateChat() {
        // TODO: Implement private chat creation
        // FR5 HOOK (please call at end):
        //   users[currentUserIndex].updateLastSeen();
    }

    void createGroup() {
        // TODO: Implement group creation
        // FR17: Groups require a name and at least 2 participants
        vector<string> participants;
        string groupName;
        string description;
        
        // FR17
        // Get group name and participants from user input
        // ensure that input usernames exist 

        if (find(participants.begin(), participants.end(), getCurrentUsername()) == participants.end()) {
            participants.push_back(getCurrentUsername());
        }
        // check for empty group name and at least 2 participants
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

        // FR5 HOOK (please call at end):
        users[currentUserIndex].updateLastSeen();
    }

    void viewChats() const {
        // TODO: Implement chat viewing
        // FR5 NOTE: method is `const`; drop `const` if you want lastSeen refreshed here.
        // FR10: set messages as read when viewing a chat
        // FR23: Display all participants and admins when viewing a group

        string current = getCurrentUsername();
        for (Chat* chat : chats) {
            if (chat && chat->isMember(current)) {
                chat->markAllAsRead(current);
                chat->displayChat();
            }
        }
    }
    
    // ---- SCRUM-32 (menu wiring): let user pick a chat and export it ----
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
        users[currentUserIndex].updateLastSeen(); // FR5
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

                if (choice == 1) login();
                else if (choice == 2) signUp();
                else if (choice == 3) break;
            }
            else {
                cout << "\n1. Start Private Chat\n2. Create Group\n3. View Chats\n4. Edit Account\n5. Change Password\n6. Export Chat\n7. Logout\nChoice: ";
                int choice;
                cin >> choice;
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // flush buffer

                switch (choice) {
                    case 1:
                        startPrivateChat();
                        break;
                    case 2:
                        createGroup();
                        break;
                    case 3:
                        viewChats();
                        break;
                    case 4: {
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
                                    do{
                                        cout << "Enter new phone number: ";
                                        getline(cin, newPhone);
                                    } while (!validatePhone(newPhone, currentUserIndex));

                                    users[currentUserIndex].setPhoneNumber(newPhone);
                                    cout << "Phone number updated successfully.\n";
                                    break;
                                }
                                case 3: {
                                    // TODO: Passwords must not be displayed when typing
                                    string newPass;
                                    cout << "Enter new password: ";
                                    getline(cin, newPass);
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
                    case 5:
                        exportChat();
                        break;
                    case 6:
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
