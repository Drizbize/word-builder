#pragma once

#include <functional>
#include <raylib-cpp.hpp>

#include "Entity/WordLetter.hpp"

enum class WordGameInit
{
    ALL = 0, // example
    ROW, // example => Example => EXample
    RND
};

enum class WordGameMethod // next letter a discover
{
    RIGHT = 0,
    LEFT,
    BOTH_SIDES,
    RND_SIDE, // TODO: fix
    RND
};

enum class WordGameStyle
{
    AVAILABLE = 0, // example
    INVISIBLE, // e       => Ex      => Exa...
    EXPLORING, // __a____ => _xAm___ => _xAMp__
};

class WordGameStrategy
{
public:
    WordGameInit initStyle;
    WordGameMethod method;
    WordGameStyle style;
    bool isHard;

    WordGameStrategy(WordGameInit i, WordGameMethod m, WordGameStyle s, bool hard = false)
        : initStyle(i), method(m), style(s), isHard(hard)
    {
    }

    void initWord(WordArray& word)
    {
        switch (style)
        {
        case WordGameStyle::AVAILABLE:
            for (size_t i = 0; i < word.size(); i++)
            {
                if (isHard)
                    word[i].style = WordLetterStyle::COLLIDEABLE;
                else
                    word[i].style = WordLetterStyle::UNTOUCHABLE;
                word[i].state = WordLetterState::KNOWN;
            }
            break;
        case WordGameStyle::INVISIBLE:
            for (size_t i = 0; i < word.size(); i++)
            {
                word[i].style = WordLetterStyle::INVISIBLE;
                word[i].state = WordLetterState::UNKNOWN;
            }
            break;
        case WordGameStyle::EXPLORING:
            for (size_t i = 0; i < word.size(); i++)
            {
                if (isHard)
                    word[i].style = WordLetterStyle::COLLIDEABLE;
                else
                    word[i].style = WordLetterStyle::UNTOUCHABLE;
                word[i].state = WordLetterState::UNKNOWN;
            }
            break;
        default:
            break;
        }

        switch (initStyle)
        {
        case WordGameInit::ALL:
            for (size_t i = 0; i < word.size(); i++)
            {
                word[i].style = WordLetterStyle::COLLIDEABLE;
                word[i].state = WordLetterState::KNOWN;
            }
            break;
        case WordGameInit::ROW:
            word[0].state = WordLetterState::KNOWN;
            word[0].style = WordLetterStyle::COLLIDEABLE;
            break;
        case WordGameInit::RND:
        {
            int randIndex;
            do
            {
                randIndex = GetRandomValue(0, word.size() - 1);
            } while (word[randIndex].letter == ' ');
            
            word[randIndex].state = WordLetterState::KNOWN;
            word[randIndex].style = WordLetterStyle::COLLIDEABLE;

            break;
        }
        default:
            break;
        }
    }
    bool setLetter(WordArray& word, int index, char letter)
    {
        if (index < 0 || index >= word.size()) return false;
        if (word[index].letter != letter) return false;
        if (word[index].state == WordLetterState::SOLVED) return true;

        word[index].state = WordLetterState::SOLVED;
        word[index].style = WordLetterStyle::UNTOUCHABLE;

        int size = (int)word.size();

        std::function<void(int, int)> activate = [&](int i, int direction) {
            if (i < 0 || i >= size) return;

            if (word[i].letter == ' ' || word[i].state == WordLetterState::SOLVED) {
                if (direction != 0) {
                    int nextIdx = (direction > 0) ? (i + 1) % size : (i - 1 + size) % size;
                    if (nextIdx != index)
                        activate(nextIdx, direction);
                }
                return;
            }

            word[i].state = WordLetterState::KNOWN;
            word[i].style = WordLetterStyle::COLLIDEABLE;
        };

        switch (method)
        {
        case WordGameMethod::RIGHT:
            activate((index + 1) % size, 1);
            break;

        case WordGameMethod::LEFT:
            activate((index - 1 + size) % size, -1);
            break;

        case WordGameMethod::BOTH_SIDES:
            activate((index + 1) % size, 1);
            activate((index - 1 + size) % size, -1);
            break;

        case WordGameMethod::RND_SIDE:
        {
            int dir = (GetRandomValue(0, 1) == 0) ? 1 : -1;
            int nextIdx = (dir > 0) ? (index + 1) % size : (index - 1 + size) % size;
            activate(nextIdx, dir);
            break;
        }

        case WordGameMethod::RND:
        {
            std::vector<int> candidates;
            for (int i = 0; i < size; i++) {
                if (word[i].state == WordLetterState::UNKNOWN && word[i].letter != ' ') {
                    candidates.push_back(i);
                }
            }
            if (!candidates.empty()) {
                activate(candidates[GetRandomValue(0, candidates.size() - 1)], 0);
            }
            break;
        }
        default:
            break;
        }

        return true;
    }
private:

};