#include <fstream>
#include <json.hpp>

using json = nlohmann::json;

class User {
public:
    static User& getInstance() {
        static User instance;
        return instance;
    }

    void save() {
        json j = { {"score", totalScore}, {"lvl", currentUnlockedLvl} };
        std::ofstream file(savePath);
        if (file.is_open()) {
            file << j.dump(4);
        }
    }

    void load() {
        std::ifstream file(savePath);
        if (file.is_open()) {
            json j;
            file >> j;
            totalScore = j.value("score", 0);
            currentUnlockedLvl = j.value("lvl", 1);
        }
    }

    void onLevelComplete(int lvlCompleted, int pointsEarned) {
        addScore(pointsEarned);
        if (lvlCompleted == currentUnlockedLvl)
        {
            unlockNextLevel();
        }
        save();
    }

    // Getters and Setters
    int getScore() const { return totalScore; }
    void addScore(int amount) { totalScore += amount; }

    int getLevel() const { return currentUnlockedLvl; }
    void unlockNextLevel() { currentUnlockedLvl++; }
private:
    User() { 
        load(); 
    }

    User(const User&) = delete;
    User& operator=(const User&) = delete;

    int totalScore = 0;
    int currentUnlockedLvl = 1;
    const std::string savePath = "user_save.json";

};