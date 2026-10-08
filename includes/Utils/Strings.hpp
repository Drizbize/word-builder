#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <string_view>

// Our existing 2D logic
class String2D
{
public:
    void add(const std::string& s)
    {
        offsets.push_back(data.size());
        data += s;
    }

    std::string_view get(size_t index) const
    {
        if (index >= offsets.size())
            return "";

        size_t start = offsets[index];
        size_t length = (index == offsets.size() - 1) 
                        ? data.size() - start 
                        : offsets[index + 1] - start;
        return std::string_view(data.data() + start, length);
    }

    size_t size() const
    {
        return offsets.size();
    }

    size_t getTotalSize() const
    {
        return data.size();
    }

    std::string& getData()
    {
        return data;
    }
private:
    std::string data;
    std::vector<size_t> offsets;
};

// The 3D Wrapper
class String3D
{
public:
    size_t addLayer()
    {
        layers.emplace_back();
        return layers.size() - 1;
    }

    void pushToLayer(size_t layer_idx, const std::string& s)
    {
        if (layer_idx < layers.size())
            layers[layer_idx].add(s);
    }

    void pushToLayer(size_t layer_idx, std::initializer_list<std::string> list) {
        if (layer_idx < layers.size()) {
            for (const auto& s : list) {
                layers[layer_idx].add(s);

                std::cout << s << '\n';
            }
        }
    }

    String2D& getLayer(size_t layer_idx)
    {
        if (layer_idx >= layers.size())
            throw std::out_of_range("Invalid layer index");
        return layers[layer_idx];
    }

    std::string_view get(size_t layerIdx, size_t strIdx) const
    {
        if (layerIdx >= layers.size())
            return "INVALID_LAYER";
        return layers[layerIdx].get(strIdx);
    }

    size_t size() const
    {
        return layers.size();
    }

    size_t getTotalSize()
    {
        size_t s = 0;
        for (auto& layer : layers)
        {
            s += layer.size();
        }

        return s;
    }
private:
    std::vector<String2D> layers;
};