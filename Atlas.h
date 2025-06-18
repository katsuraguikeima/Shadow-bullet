#ifndef ATLAS_H
#define ATLAS_H

#include <string>
#include <map>
#include <glm/glm.hpp>
#include "Libraries/LIBS/nlohmann/json.hpp" // <<< incluí esto

using json = nlohmann::json;      // <<< y esta alias



class Atlas {
public:
    Atlas(const std::string& jsonPath);
    glm::vec4 getUV(const std::string& name) const;

private:
    std::map<std::string, glm::vec4> uvMap;
    void loadJSON(const std::string& path);
};

#endif 