#include <functional>
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>

struct Connection {
    size_t id = -1;
    std::function<void()> disconnectFunc;
    void disconnect()
    {
        if (id != -1)
        {
            disconnectFunc();
            id = -1;
        }
    }
};

template<typename... Args>
class Signal {
private:
    struct Slot {
        size_t id;
        std::function<void(Args...)> callback;
    };
    
    std::vector<Slot> m_slots;
    size_t m_nextId = 0;
public:
    Connection connect(std::function<void(Args...)> callback) {
        size_t id = m_nextId++;
        m_slots.push_back({ id, callback });

        return Connection{
            id,
            [this, id]() { this->disconnect(id); std::cout << "Disconnected\n"; }
        };
    }

    void disconnect(size_t id) {
        m_slots.erase(std::remove_if(m_slots.begin(), m_slots.end(),
            [id](const Slot& s) { return s.id == id; }), m_slots.end());
    }

    void fire(Args... args) {
        // Use a copy or a safe iterator in case a callback disconnects itself during execution
        auto temp_slots = m_slots; 
        for (auto& slot : temp_slots) {
            slot.callback(args...);
        }
    }
};