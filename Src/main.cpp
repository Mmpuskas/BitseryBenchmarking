#include "bitsery/bitsery.h"
#include "bitsery/adapter/buffer.h"
#include "bitsery/traits/vector.h"
#include "bitsery/traits/array.h"
#include <iostream>
#include <chrono>

// Define our test struct.
enum class InputState : uint8_t
{
    Invalid,
    Pressed,
    Released
};
struct Entity {
    uint32_t id;
    float x;
    float y;
    float z;
    float velX;
    float velY;
    float velZ;
    std::array<InputState, 6> inputStates;
};

struct EntityUpdate {
    std::vector<Entity> entities;
};

// Define how the struct should be serialized/deserialized.
// Seems to be a specialization that then gets used for this type?
template <typename S>
void serialize(S& s, Entity& entity) {
    // "value" is a fundamental type (int, float, enum)
    // 4b is the size (2b, 4b, etc)
    s.value4b(entity.id);
    s.value4b(entity.x);
    s.value4b(entity.y);
    s.value4b(entity.z);
    s.value4b(entity.velX);
    s.value4b(entity.velY);
    s.value4b(entity.velZ);
    s.container1b(entity.inputStates);
}

template <typename S>
void serialize(S& s, EntityUpdate& entityUpdate) {
    // Resizable contains needs a maxSize to prevent buffer overflow attacks.
    s.container(entityUpdate.entities, 1000);
}

using OutputAdapter = bitsery::OutputBufferAdapter<std::vector<uint8_t>>;
using InputAdapter = bitsery::InputBufferAdapter<std::vector<uint8_t>>;

int main() {
    auto totalStart = std::chrono::high_resolution_clock::now();
    // Set up our data.
    Entity entity;
    entity.id = 42;
    entity.x = 9234.2394;
    entity.y = 293423.1239;
    entity.z = 934.2348;
    entity.velX = 9234.2394;
    entity.velY = 293423.1239;
    entity.velZ = 934.2348;
    for (unsigned int i = 0; i < 6; ++i) {
        entity.inputStates[i] = InputState::Pressed;
    }

    EntityUpdate entityUpdate;
    const unsigned int numEntities = 1000;
    for (unsigned int i = 0; i < numEntities; ++i) {
        entityUpdate.entities.push_back(entity);
    }

    // Serialize the data.
    std::vector<uint8_t> sendBuffer;
    sendBuffer.reserve(35000);
    auto serializeStart = std::chrono::high_resolution_clock::now();

    std::size_t writtenSize = bitsery::quickSerialization<OutputAdapter>(sendBuffer, entityUpdate);

    auto serializeEnd = std::chrono::high_resolution_clock::now();
    auto serializeDuration = std::chrono::duration_cast<std::chrono::microseconds>(serializeEnd - serializeStart).count();

    // Deserialize the data.
    EntityUpdate receivedUpdate{};
    auto deserializeStart = std::chrono::high_resolution_clock::now();

    auto result = bitsery::quickDeserialization<InputAdapter>({ sendBuffer.begin(),
            writtenSize}, receivedUpdate);

    auto deserializeEnd = std::chrono::high_resolution_clock::now();
    auto deserializeDuration = std::chrono::duration_cast<std::chrono::microseconds>(deserializeEnd - deserializeStart).count();

    auto totalEnd = std::chrono::high_resolution_clock::now();
    auto totalDuration = std::chrono::duration_cast<std::chrono::microseconds>(totalEnd - totalStart).count();
    // First holds the error, second holds whether the buffer was successfully read from beginning to end.
    if (result.second) {
        std::cout << "Entities: " << receivedUpdate.entities.size() << std::endl;
        std::cout << "Message size: " << writtenSize << std::endl;
        std::cout << "Serialization time: " << serializeDuration << "us" << std::endl;
        std::cout << "Deserialization time: " << deserializeDuration << "us" << std::endl;
        std::cout << "Total time: " << totalDuration << "us" << std::endl;
    }
    else {
        std::cout << "Error reading: " << static_cast<unsigned int>(result.first) << std::endl;
    }

    return 0;
}

