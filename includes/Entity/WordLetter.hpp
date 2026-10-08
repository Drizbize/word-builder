#pragma once

#include <vector>

enum class WordLetterState
{
    UNKNOWN = 1,
    KNOWN,
    SOLVED
};

enum class WordLetterStyle
{
    INVISIBLE,
    COLLIDEABLE,
    UNTOUCHABLE
};

struct WordLetter
{
    char letter;
    WordLetterStyle style = WordLetterStyle::COLLIDEABLE;
    WordLetterState state = WordLetterState::KNOWN;

    WordLetter(char l)
        : letter(l)
    {
    }
};

typedef std::vector<WordLetter> WordArray;