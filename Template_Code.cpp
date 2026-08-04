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
        // FR5 HOOK (please call at end): updateLastSeen();
        username = "";
        password = "";
        phoneNumber = "";
        status = "";
        updateLastSeen();
    }

    User(string uname, string pwd, string phone) {
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

    // ---- Reply-safety branch ----
    // A Message used to store `Message* replyTo`, a raw pointer INTO
    // Chat::messages (a vector<Message>). That's unsafe for two reasons:
    //   1) Deleting the original message left the pointer dangling.
    //   2) Even without any deletion, vector<Message>::push_back() can
    //      reallocate and move every existing Message in memory, silently
    //      invalidating every replyTo pointer in the whole chat.
    // Fix: store a SNAPSHOT of the sender/content being replied to instead
    // of a pointer to the live object. Nothing to dangle, and the quoted
    // preview keeps showing correctly even after the original is deleted.
    bool isReply;
    string replyToSender;
    string replyToContent;

public:
    Message() {
        sender = "";
        content = "";
        status = "";
        isReply = false;
        updateTimestamp();
    }
    
    Message(string sndr, string cntnt) { 
        sender = sndr;
        content = cntnt;
        status = "Sent";
        isReply = false;
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
        return isReply;
    }

    string getReplyToSender() const {
        return replyToSender;
    }

    string getReplyToContent() const {
        return replyToContent;
    }

    void setStatus(string newStatus) {
        status = newStatus;
    }

    void markAsRead() {
        status = "Read";
    }

    // Takes a snapshot of `original` (sender + content) at the moment of
    // replying. Deliberately NOT a pointer/reference kept for later use -
    // this message owns its own copy, so it survives the original being
    // deleted, edited, or the vector it lived in being reallocated.
    void setReplyTo(const Message& original) {
        isReply = true;
        replyToSender = original.getSender();
        replyToContent = original.getContent();
    }

    void clearReply() {
        isReply = false;
        replyToSender = "";
        replyToContent = "";
    }

    void updateTimestamp() {
        time_t now = time(nullptr);
        string t = ctime(&now);
        if (!t.empty() && t.back() == '\n') t.pop_back();
        timestamp = t;
    }

    void display() const {
        if (isReply) {
            cout << "  \u21B3 Replying to " << replyToSender
                << ": \"" << replyToContent << "\"" << endl;
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

    // ---- FR13: emoji support ----
    // Looks up `emojiCode` (e.g. ":)", "<3") in the recognized emoji map and,
    // if found, replaces every occurrence of that literal code inside
    // `content` with the mapped emoji symbol - modifying content in place.
    // Unrecognized codes are left untouched (no-op) so callers can safely
    // pass arbitrary user input without corrupting the message.
    void addEmoji(string emojiCode) {
        static const vector<pair<string, string>> emojiMap = {
            {":)", "\U0001F642"},          // slightly smiling face
            {":(", "\U0001F641"},          // slightly frowning face
            {":D", "\U0001F600"},          // grinning face
            {"<3", "\u2764\uFE0F"},        // red heart
            {":thumbsup:", "\U0001F44D"}   // thumbs up
        };

        string emoji;
        for (const auto& entry : emojiMap) {
            if (entry.first == emojiCode) {
                emoji = entry.second;
                break;
            }
        }

        if (emoji.empty()) {
            // Not a recognized code - leave the message content unchanged.
            return;
        }

        size_t pos = 0;
        while ((pos = content.find(emojiCode, pos)) != string::npos) {
            content.replace(pos, emojiCode.length(), emoji);
            pos += emoji.length();
        }
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
    }

    // ---- Reply-safety branch ----
    // Overload for sending a message that replies to an existing message
    // in this chat. `replyToIndex` is looked up and its sender/content are
    // snapshotted into the new message via Message::setReplyTo() BEFORE
    // the new message is stored - no pointer into `messages` is ever kept.
    // Returns false if replyToIndex is out of range.
    bool addMessage(const Message& msg, int replyToIndex) {
        if (replyToIndex < 0 || replyToIndex >= (int)messages.size()) {
            return false;
        }
        Message newMsg = msg;
        newMsg.setReplyTo(messages[replyToIndex]);
        messages.push_back(newMsg);
        messages.back().setStatus("Delivered");
        return true;
    }

    bool deleteMessage(int index, const string& username) {
        if (index < 0 || index >= (int)messages.size()) {
            return false;
        }
        if (messages[index].getSender() != username) {
            return false;
        }

        // ---- Reply-safety branch ----
        // Replies no longer hold a pointer into `messages`, so there is no
        // dangling-pointer cleanup to do here anymore: any message that
        // replied to messages[index] already has its own private copy of
        // that message's sender/content and keeps displaying it correctly
        // after this erase.
        messages.erase(messages.begin() + index);
        return true;
    }

    virtual void displayChat() const {
        cout << chatName << endl;
        for (size_t i = 0; i < messages.size(); i++) {
            cout << i + 1 << ". ";
            messages[i].display();
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
        Chat::displayChat();
    }

    void showTypingIndicator(const string& username) const {
        if (isMember(username)) {
            cout << "\r" << username << " is typing";
            for (int i = 0; i < 3; ++i) {
                cout << ".";
                cout.flush();
            }
            cout << "\r\033[K";
            cout.flush();
        } else {
            cout << "User not found in this chat." << endl;
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

    void addAdmin(const string& admin, string newAdmin) {
        if (!isAdmin(admin)) {
            cout << "Only admins can promote participants to admin." << endl;
            return;
        }
        else{
            if (isParticipant(newAdmin) && !isAdmin(newAdmin)) {
                admins.push_back(newAdmin);
                cout << newAdmin << " has been promoted to admin." << endl;
            } else if (!isParticipant(newAdmin)) {
                cout << "User is not a participant in the group." << endl;
            } else {
                cout << "User is already an admin." << endl;
            }
        }
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

    // ---- SCRUM-34: release all Chat* allocations (SRS §7.5, §10.5) ----
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
        cout << "User registered successfully! You can now log in." << endl;
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
        // FR5 HOOK (please call after successful auth):
        //   users[currentUserIndex].updateLastSeen();
        // set logged in to true and currentUserIndex to the index of 
        // the logged-in user using findUserIndex()
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
            users[currentUserIndex].updateLastSeen();
            loggedIn = true;
            cout << "\n[Success] Logged in successfully! Welcome, " << getCurrentUsername() << "!\n";
        } else {
            cout << "[Error] Incorrect password!\n";
        }
    }

    void startPrivateChat() {
        string username;

        cout << "Enter the username to start a private chat with: ";
        getline(cin, username);

        if (username == getCurrentUsername()) {
            cout << "You cannot start a chat with yourself." << endl;
        }
        else if (findUserIndex(username) == -1) {
            cout << "User does not exist." << endl;
        }
        else {
            for (Chat* chat : chats) {
                PrivateChat* pc = dynamic_cast<PrivateChat*>(chat);
                if (pc &&
                    chat->isMember(getCurrentUsername()) &&
                    chat->isMember(username)) {

                    cout << "Private chat already exists.\n";
                    return;
                }
            }
            Chat* newChat = new PrivateChat(getCurrentUsername(), username);
            chats.push_back(newChat);

            cout << "Private chat created successfully with " << username << "." << endl;
        }

        users[currentUserIndex].updateLastSeen();
    }

    void createGroup() {
        vector<string> participants;
        string groupName;
        string description;

        cout << "Enter group name: ";
        getline(cin, groupName);
        cout << "Enter group description: ";
        getline(cin, description);

        while(true) {
            string participant;
            cout << "Enter participant username (or 'done' to finish): ";
            getline(cin, participant);
            if (participant == "done") break;
            if (find(participants.begin(), participants.end(), participant) != participants.end()) {
                cout << "Participant already added." << endl;
                continue;
            }
            // ensure that input usernames exist 
            if (findUserIndex(participant) == -1) {
                cout << "User does not exist." << endl;
                continue;
            }
            participants.push_back(participant);
        }

        // ensure the current user is also a participant
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

    void viewChats() {
        // FR5 NOTE: method is `const`; drop `const` if you want lastSeen refreshed here.
        // FR10: set messages as read when viewing a chat
        // FR23: Display all participants and admins when viewing a group

        vector<Chat*> myChats;

        for (Chat* chat : chats) {
            if (chat && chat->isMember(getCurrentUsername()))
                myChats.push_back(chat);
        }

        if (myChats.empty()) {
            cout << "You have no chats.\n";
            users[currentUserIndex].updateLastSeen();
            return;
        }

        cout << "\nYour Chats:\n";
        for (size_t i = 0; i < myChats.size(); ++i)
            cout << i + 1 << ". " << myChats[i]->getChatName() << '\n';

        cout << "Choice (0 to cancel): ";
        int chatChoice;
        cin >> chatChoice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (chatChoice <= 0 || chatChoice > (int)myChats.size()){
            users[currentUserIndex].updateLastSeen();
            return;
        }

        Chat* selected = myChats[chatChoice - 1];
        selected->markAllAsRead(getCurrentUsername());
        // selected->displayChat();
        users[currentUserIndex].updateLastSeen();
        openChat(selected);
    }

    void openChat(Chat* chat){
        bool inChat = true;
        GroupChat* group = dynamic_cast<GroupChat*>(chat);
        while (inChat) {

            cout << "\n==============================\n";
            chat->displayChat();

            cout << "\n1. Send Message\n";
            cout << "2. Reply to Message\n";
            cout << "3. Delete My Message\n";
            cout << "4. Search Messages\n";

            if (group) {
                cout << "5. Group Settings\n";
                cout << "6. Back\n";
            }
            else {
                cout << "5. Back\n";
            }

            int option;
            cin >> option;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            switch(option){
                case 1: {
                    PrivateChat* pc = dynamic_cast<PrivateChat*>(chat);
                    if (pc) {
                        pc->showTypingIndicator(getCurrentUsername());
                    }

                    string content;
                    cout << "Enter message: ";

                    getline(cin, content);
                    Message msg(getCurrentUsername(), content);
                    chat->addMessage(msg);
                    break;
                }
                case 2: {
                    PrivateChat* pc = dynamic_cast<PrivateChat*>(chat);
                    if (pc) {
                        pc->showTypingIndicator(getCurrentUsername());
                    }

                    int replyIndex;
                    cout << "Reply to message #: ";
                    cin >> replyIndex;
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    replyIndex--; // Convert to 0-based index

                    
                    string replyContent;
                    cout << "Enter your reply: ";
                    getline(cin, replyContent);
                    Message replyMsg(getCurrentUsername(), replyContent);

                    if (!chat->addMessage(replyMsg, replyIndex)) {
                        cout << "Invalid message index." << endl;
                        break;
                    }
                    break;
                }
                case 3: {
                    int index;
                    cout << "Delete message #: ";
                    cin >> index;
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');

                    if (!chat->deleteMessage(index - 1, getCurrentUsername()))
                        cout << "Cannot delete that message.\n";

                    break;
                }

                case 4: {
                    string keyword;
                    cout << "Keyword: ";
                    getline(cin, keyword);

                    vector<Message> results = chat->searchMessages(keyword);

                    if (results.empty()) {
                        cout << "No matching messages.\n";
                    } else {
                        cout << "\nResults:\n";
                        for (const Message& msg : results)
                            msg.display();
                    }
                    break;
                }
                case 5:
                    if (group) {
                        groupSettings(group);
                    } else {
                        inChat = false;
                    }
                    break;

                case 6:
                    if (group) {
                        inChat = false;
                    } else {
                        cout << "Invalid choice. Please try again.\n";
                    }
                    break;

                default:
                    cout << "Invalid choice. Please try again.\n";
            }
        }
        users[currentUserIndex].updateLastSeen();
    }

    void groupSettings(GroupChat* group) {
        if (!group->isAdmin(getCurrentUsername())) {
            cout << "Only group admins can access group settings." << endl;
            return;
        }

        bool inSettings = true;

        while (inSettings) {
            cout << "\n===== Group Settings =====\n";
            cout << "1. Remove Participant\n";
            cout << "2. Promote Participant to Admin\n";
            cout << "3. Back\n";
            cout << "Choice: ";

            int choice;
            cin >> choice;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            switch (choice) {
                case 1: {
                    string username;
                    cout << "Enter participant username: ";
                    getline(cin, username);

                    group->removeParticipant(getCurrentUsername(), username);
                    break;
                }

                case 2: {
                    string username;
                    cout << "Enter participant username: ";
                    getline(cin, username);

                    group->addAdmin(getCurrentUsername(), username);
                    break;
                }

                case 3:
                    inSettings = false;
                    break;

                default:
                    cout << "Invalid choice. Please try again." << endl;
            }
        }

        users[currentUserIndex].updateLastSeen();
    }

    // ---- SCRUM-32 (menu wiring): let user pick a chat and export it ----
    void exportChat() {
        vector<Chat*> myChats;

        for (Chat* chat : chats) {
            if (chat && chat->isMember(getCurrentUsername()))
                myChats.push_back(chat);
        }
        
        if (myChats.empty()) {
            cout << "No chats to export." << endl;
            return;
        }
        cout << "\nSelect a chat to export:\n";
        for (size_t i = 0; i < myChats.size(); ++i) {
            cout << (i + 1) << ". " << myChats[i]->getChatName() << "\n";
        }
        cout << "Choice: ";
        int idx;
        cin >> idx;
        if (idx < 1 || idx > (int)myChats.size()) {
            cout << "Invalid choice." << endl;
            return;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Filename: ";
        string filename;
        getline(cin, filename);
        myChats[idx - 1]->exportToFile(filename);
        users[currentUserIndex].updateLastSeen(); // FR5
    }

    void logout() {
        users[currentUserIndex].updateLastSeen();
        currentUserIndex = -1;
        loggedIn = false;
    }

    void editAccount() {
        bool editing = true;
        while (editing) {
            cout << "\n--- Edit Account ---\n";
            cout << "1. Change Status\n";
            cout << "2. Change Phone Number\n";
            cout << "3. Change Password\n";
            cout << "4. Back to Main Menu\n";
            cout << "Choice: ";

            int choice;
            cin >> choice;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            switch (choice) {
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
                cout << "\n1. Start Private Chat\n2. Create Group\n3. View Chats\n4. Edit Account\n5. Export Chat\n6. Logout\nChoice: ";
                int choice;
                cin >> choice;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                switch (choice) {
                    case 1:startPrivateChat(); break;
                    case 2:createGroup(); break;
                    case 3:viewChats(); break;
                    case 4:editAccount(); break;
                    case 5:exportChat(); break;
                    case 6: logout(); break;
                    default: cout << "Invalid choice. Please try again." << endl;
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