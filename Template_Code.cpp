#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <limits>
using namespace std;

// HELPER FUNCTION
bool validatePassword(string pwd) {
    // TODO: Implement password validation (FR3)
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
    }
    
    User(string uname, string pwd, string phone) {
        // TODO: Implement parameterized constructor
        // FR5 HOOK (please call at end): updateLastSeen();
    }
    
    string getUsername() const {
        // TODO: Implement getter
        return "";
    }
    
    string getPhoneNumber() const {
        return phoneNumber;
    }
    
    string getStatus() const {
         return status;
    }
    
    string getLastSeen() const {
        // TODO: Implement getter
        return "";
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
        // TODO: Implement password check
        return false;
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
        // TODO: Implement default constructor
    }
    
    Message(string sndr, string cntnt) {
        // TODO: Implement parameterized constructor
    }
    
    // ---- FR12 needs this ----
    string getContent() const {
        return content;
    }
    
    string getSender() const {
        return sender;
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
        return replyTo;
    }
    
    void setStatus(string newStatus) {
        // TODO: Implement setter
    }
    
    void setReplyTo(Message* msg) {
        replyTo = msg;
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
        // TODO: Implement chat display
    }
    
    // ---- FR12: search messages by keyword ----
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
    }
    
    void addAdmin(string newAdmin) {
        // TODO: Implement add admin
    }
    
    bool removeParticipant(const string& admin, const string& userToRemove) {
        // TODO: Implement remove participant
        return false;
    }
    
    bool isAdmin(string username) const {
        // TODO: Implement admin check
        return false;
    }
    
    bool isParticipant(string username) const {
        // TODO: Implement participant check
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
        // use validateUsername() to check username uniqueness
    }
    
    bool validateUsername(string uname) {
        for(User user: users){
            if(user.getUsername() == uname){
                return false;
            }
        }
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
        // FR5 NOTE: method is `const`; drop `const` if you want lastSeen refreshed here.
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
                cout << "\n1. Start Private Chat\n2. Create Group\n3. View Chats\n4. Change Password\n5. Logout\nChoice: ";
                int choice;
                cin >> choice;
                
                if (choice == 1) startPrivateChat();
                else if (choice == 2) createGroup();
                else if (choice == 3) viewChats();
                else if (choice == 4) {
                    string newPass;
                    // TODO: Passwords must not be displayed when typing
                    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // flush buffer
                    cout << "Enter new password: " << endl;
                    getline(cin, newPass);
                    users[currentUserIndex].changePassword(newPass);
                }
                else if (choice == 5) logout();
            }
        }
    }
};

// HELPER FUNCTION
bool validatePassword(string pwd) {
    // TODO: Implement password validation (FR3)
    return true;
}

// ========================
//          MAIN
// ========================
int main() {
    WhatsApp whatsapp;
    whatsapp.run();
    return 0;
}