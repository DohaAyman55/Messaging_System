#include <iostream>
#include <vector>
#include <string>
#include <ctime>
using namespace std;

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
        status = "Hey there! I am using WhatsApp.";
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
        // TODO: Implement getter
        return username;
    }

    string getPhoneNumber() const {
        // TODO: Implement getter
        return phoneNumber;
    }

    string getStatus() const {
        // TODO: Implement getter
        return status;
    }

    string getLastSeen() const {
        // TODO: Implement getter
        return lastSeen;
    }

    void setStatus(string newStatus) {
        // TODO: Implement setter
        // FR5 HOOK (please call at end): updateLastSeen();
        status = newStatus;
        updateLastSeen();
    }

    void setPhoneNumber(string phone) {
        // TODO: Implement setter
        // FR5 HOOK (please call at end): updateLastSeen();
        phoneNumber = phone;
        updateLastSeen();
    }

    // ---- FR5: this method is mine ----
    void updateLastSeen() {
        time_t now = time(nullptr);
        string t = ctime(&now);
        if (!t.empty() && t.back() == '\n') t.pop_back();
        lastSeen = t;
    }

    bool checkPassword(string pwd) const {
        // TODO: Implement password check
        return password == pwd;
    }

    void changePassword(string newPwd) {
        // TODO: Implement password change
        // FR5 HOOK (please call at end): updateLastSeen();
        password = newPwd;
        updateLastSeen();
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
        // TODO: Implement default constructor
    }

    Message(string sndr, string cntnt) {
        // TODO: Implement parameterized constructor
    }

    string getContent() const {
        // TODO: Implement getter
        return "";
    }

    string getSender() const {
        // TODO: Implement getter
        return "";
    }

    string getTimestamp() const {
        // TODO: Implement getter
        return "";
    }

    string getStatus() const {
        // TODO: Implement getter
        return "";
    }

    Message* getReplyTo() const {
        // TODO: Implement getter
        return nullptr;
    }

    void setStatus(string newStatus) {
        // TODO: Implement setter
    }

    void setReplyTo(Message* msg) {
        // TODO: Implement setter
    }

    void updateTimestamp() {
        // TODO: Implement timestamp update
    }

    void display() const {
        // TODO: Implement message display
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
        // TODO: Implement default constructor
    }

    Chat(vector<string> users, string name) {
        // TODO: Implement parameterized constructor
    }

    void addMessage(const Message& msg) {
        // TODO: Implement message addition
    }

    bool deleteMessage(int index, const string& username) {
        // TODO: Implement message deletion
        return false;
    }

    virtual void displayChat() const {
        // TODO: Implement chat display
    }

    vector<Message> searchMessages(string keyword) const {
        // TODO: Implement message search
        return {};
    }

    void exportToFile(const string& filename) const {
        // TODO: Implement export to file
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
    PrivateChat(string u1, string u2) {
        // TODO: Implement constructor
    }

    void displayChat() const override {
        // TODO: Implement private chat display
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
    GroupChat(vector<string> users, string name, string creator) {
        // TODO: Implement constructor
        participants = users;
        chatName = name;
        admins.push_back(creator);
    }

    void addAdmin(string newAdmin) {
        // TODO: Implement add admin
    }

    bool removeParticipant(const string& admin, const string& userToRemove) {
        if(!isAdmin(admin)){
            cout << "Only admins can remove participants." << endl;
            return false;
        }
        else if (isAdmin(userToRemove)) {
            cout << "Cannot remove an admin from the group." << endl;
            return false;
        }
        else if (!isParticipant(userToRemove)) {
            cout << "User not found in the group." << endl;
            return false;
        }
        else {
            for (int i = 0; i < participants.size(); i++) {
                if (participants[i] == userToRemove) {
                    participants.erase(participants.begin() + i);
                    cout << "Participant removed successfully." << endl;
                    return true;
                }
            }
        }
        return false;
    }

    bool isAdmin(string username) const {
        for (const auto& admin : admins) {
            if (admin == username) {
                return true;
            }
        }
        return false;
    }

    bool isParticipant(string username) const {
        for (const auto& participant : participants) {
            if (participant == username) {
                return true;
            }
        }
        return false;
    }

    void setDescription(string desc) {
        // TODO: Implement set description
    }

    void displayChat() const override {
        // TODO: Implement group chat display
    }

    void sendJoinRequest(const string& username) {
        // TODO: Implement join request
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

    int findUserIndex(string username) const {
        // TODO: Implement user search
        return -1;
    }

    bool isLoggedIn() const {
        // TODO: Implement login check
        return false;
    }

    string getCurrentUsername() const {
        // TODO: Implement get current user
        return "";
    }

public:
    WhatsApp() : currentUserIndex(-1) {}

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
        // TODO: Implement username validation (FR2)
        return true;
    }

    bool validatePassword(string pwd) {
        // TODO: Implement password validation (FR3)
        return true;
    }
    
    bool validatePhone(string phone) {
  
        if (phone.length() != 11) {
            cout << "Phone number must be 11 digits long." << endl;
            return false;
        }
        else if (phone[0] != '0' || phone[1] != '1') {
            cout << "Phone number must start with '01'." << endl;
            return false;
        }
        else {
            for (const auto& user : users) {
                if (user.getPhoneNumber() == phone) {
                    cout << "Phone number already exists. Please use a different phone number." << endl;
                    return false;
                }
            }
        }

        return true;
    }

    void login() {
        // TODO: Implement user login
        // FR5 HOOK (please call after successful auth):
        //   users[currentUserIndex].updateLastSeen();
    }

    void startPrivateChat() {
        // TODO: Implement private chat creation
        // FR5 HOOK (please call at end):
        //   users[currentUserIndex].updateLastSeen();
    }

    void createGroup() {
        // TODO: Implement group creation
        // FR5 HOOK (please call at end):
        //   users[currentUserIndex].updateLastSeen();
    }

    void viewChats() const {
        // TODO: Implement chat viewing
        // FR5 NOTE: method is `const`; to refresh lastSeen here, drop the `const`.
    }

    void logout() {
        // TODO: Implement logout
        // FR5 HOOK (please call BEFORE resetting currentUserIndex to -1):
        //   users[currentUserIndex].updateLastSeen();
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
                cout << "\n1. Start Private Chat\n2. Create Group\n3. View Chats\n4. Logout\nChoice: ";
                int choice;
                cin >> choice;

                if (choice == 1) startPrivateChat();
                else if (choice == 2) createGroup();
                else if (choice == 3) viewChats();
                else if (choice == 4) logout();
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