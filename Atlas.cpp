#include "Atlas.h"
#include <fstream>
#include <iostream>

Atlas::Atlas(const std::string& jsonPath) {
    loadJSON(jsonPath);
}

void Atlas::loadJSON(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "No se pudo abrir atlas JSON: " << path << "\n";
        return;
    }

    json j;
    file >> j;  // ahora 'json' está definido

    // Asumimos que tu JSON es un objeto con keys -> array[4]
    for (auto& [key, value] : j.items()) {
        if (value.is_array() && value.size() == 4) {
            // value = [ u1, v1, u2, v2 ]
            uvMap[key] = glm::vec4(
                value[0].get<float>(),
                value[1].get<float>(),
                value[2].get<float>(),
                value[3].get<float>()
            );
        }
    }
}

glm::vec4 Atlas::getUV(const std::string& name) const {
    auto it = uvMap.find(name);
    if (it != uvMap.end()) return it->second;
    // fallback: toda la textura
    return glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
}
