#pragma once

#include <fstream>
#include <json.hpp>
#include <unordered_map>
#include <algorithm>
#include <string>

#include "Entity/MoveLetter.hpp"
#include "Game/WordGameStrategy.hpp"
#include "Utils/Strings.hpp"

using json = nlohmann::json;

struct DifficultyProfile
{
    float cameraRot;
    float letterTimerOut; // Spawn interval
    float velocityMin, velocityMax;
    float velocityRot;
    float spawnRange; // 0.f - 1.f
    MoveType letterMoveType;
};

template <typename T>
class MilestoneMap {
public:
    MilestoneMap() = default;
    MilestoneMap(const std::map<int, T>& milestones) : m_milestones(milestones) {}

    void add(int threshold, const T& value) {
        m_milestones.insert_or_assign(threshold, value);
    }

    T* getAt(int currentProgress) {
        T* active = nullptr;
        for (auto& entry : m_milestones) {
            if (currentProgress >= entry.first) {
                active = &entry.second;
            }
        }
        return active;
    }

private:
    std::map<int, T> m_milestones;
};

class LevelData {
public:
    int level;
    int lifes;
    std::string firstText;
    MilestoneMap<DifficultyProfile> diffMap;
    MilestoneMap<WordGameStrategy> strategyMap;
    String3D text;

    LevelData()
    {
        std::cout << "Created\n";
    }

    static LevelData LoadFromFile(const std::string& filename, int level) {
        std::ifstream file(filename);
        if (!file.is_open()) return {};

        json j;
        file >> j;

        LevelData lvl;
        lvl.level = level;

        // "Cursor" variables to track current state (Cascading Inheritance)
        DifficultyProfile curDiff;
        curDiff.letterMoveType = MoveType::LINEAR;
        curDiff.cameraRot = 0.f;
        curDiff.spawnRange = 0.5f;
        curDiff.velocityRot = 0.f;

        WordGameStrategy curStrat(WordGameInit::ALL, WordGameMethod::BOTH_SIDES, WordGameStyle::AVAILABLE);

        int currentScore = 0; // The implicit score counter

        lvl.firstText = j["first_text"].get<std::string>();
        lvl.lifes = j["lifes"].get<int>();

        for (const auto& layer : j["text_layers"]) {
            size_t layerId = lvl.text.addLayer();

            for (const auto& item : layer) {
                
                if (item.is_string()) {
                    lvl.text.pushToLayer(layerId, item.get<std::string>());
                    currentScore++;
                } 
                else if (item.is_object()) {
                    // --- Handle Difficulty ---
                    if (item.contains("difficulty")) {
                        auto& d = item["difficulty"];
                        curDiff.cameraRot = d.value("cameraRot", curDiff.cameraRot);
                        curDiff.letterTimerOut = d.value("timerOut", curDiff.letterTimerOut);
                        curDiff.spawnRange = d.value("spawnRange", curDiff.spawnRange);
                        curDiff.velocityMin = d.value("vMin", curDiff.velocityMin);
                        curDiff.velocityMax = d.value("vMax", curDiff.velocityMax);
                        curDiff.velocityRot = d.value("vRot", curDiff.velocityRot);
                        
                        if (d.contains("moveType")) 
                            curDiff.letterMoveType = strToMove(d["moveType"], curDiff.letterMoveType);

                        lvl.diffMap.add(currentScore, curDiff);
                    }

                    // --- Handle Strategy ---
                    if (item.contains("strategy")) {
                        auto& s = item["strategy"];
                        
                        WordGameInit init = curStrat.initStyle; // Assuming public members or getters
                        WordGameMethod method = curStrat.method;
                        WordGameStyle style = curStrat.style;

                        if (s.contains("init"))   init = strToInit(s["init"], init);
                        if (s.contains("method")) method = strToMethod(s["method"], method);
                        if (s.contains("style"))  style = strToStyle(s["style"], style);

                        curStrat = WordGameStrategy(init, method, style);
                        lvl.strategyMap.add(currentScore, curStrat);
                    }
                }
            }
        }

        return lvl;
    }
    
    static int GetLetterCount(const std::string& jsonPath) {
        std::ifstream file(jsonPath);
        if (!file.is_open()) return 0;

        json j;
        file >> j;

        int totalLetters = 0;
        if (j.contains("text_layers")) {
            for (const auto& layer : j["text_layers"]) {
                for (const auto& item : layer) {
                    if (item.is_string()) {
                        std::string s = item.get<std::string>();
                        for (char c : s) if (c != ' ') totalLetters++;
                    }
                }
            }
        }
        return totalLetters;
    }
private:
    static WordGameInit strToInit(const std::string& s, WordGameInit def) {
        static const std::unordered_map<std::string, WordGameInit> m = {
            {"ALL", WordGameInit::ALL}, {"ROW", WordGameInit::ROW}, {"RND", WordGameInit::RND}
        };
        auto it = m.find(s);
        return (it != m.end()) ? it->second : def;
    }

    static WordGameMethod strToMethod(const std::string& s, WordGameMethod def) {
        static const std::unordered_map<std::string, WordGameMethod> m = {
            {"RIGHT", WordGameMethod::RIGHT}, {"LEFT", WordGameMethod::LEFT}, 
            {"BOTH_SIDES", WordGameMethod::BOTH_SIDES}, {"RND_SIDE", WordGameMethod::RND_SIDE}, {"RND", WordGameMethod::RND}
        };
        auto it = m.find(s);
        return (it != m.end()) ? it->second : def;
    }

    static WordGameStyle strToStyle(const std::string& s, WordGameStyle def) {
        static const std::unordered_map<std::string, WordGameStyle> m = {
            {"AVAILABLE", WordGameStyle::AVAILABLE}, {"INVISIBLE", WordGameStyle::INVISIBLE}, {"EXPLORING", WordGameStyle::EXPLORING}
        };
        auto it = m.find(s);
        return (it != m.end()) ? it->second : def;
    }

    static MoveType strToMove(const std::string& s, MoveType def) {
        static const std::unordered_map<std::string, MoveType> m = {
            {"LINEAR", MoveType::LINEAR}, {"SINE_WAVE", MoveType::SINE_WAVE}, {"CURVE", MoveType::CURVE}
        };
        auto it = m.find(s);
        return (it != m.end()) ? it->second : def;
    }
};