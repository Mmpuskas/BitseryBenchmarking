#include <iostream>
#include "bitsery/bitsery.h"
#include "bitsery/adapter/buffer.h"
#include "bitsery/traits/vector.h"

// Define our test struct.
enum class Input : uint16_t
{
    Left,
    Up,
    Right,
    Down
};
struct Entity {
    uint32_t id;
    Input input;
    std::vector<float> fs;
};

// Define how the struct should be serialized/deserialized.
// Seems to be a specialization that then gets used for this type?
template <typename S>
void serialize(S& s, Entity& entity) {
    // "value" is a fundamental type (int, float, enum)
    // 4b is the size (2b, 4b, etc)
    s.value4b(entity.id);
    s.value2b(entity.input);

    // Resizable contains needs a maxSize to prevent buffer overflow attacks.
    s.container4b(entity.fs, 10);
}

using OutputAdapter = bitsery::OutputBufferAdapter<std::vector<uint8_t>>;
using InputAdapter = bitsery::InputBufferAdapter<std::vector<uint8_t>>;

int main() {
    // Set up our data.
    Entity entity{42, Input::Right, {12.2, 13.3, 14.4}};

    // Serialize the data.
    std::vector<uint8_t> sendBuffer;
    std::size_t writtenSize = bitsery::quickSerialization<OutputAdapter>(sendBuffer, entity);

    // Deserialize the data.
    Entity receivedEntity{};
    auto result = bitsery::quickDeserialization<InputAdapter>( { sendBuffer.begin(),
            writtenSize }, receivedEntity);

    // First holds the error, second holds whether the buffer was successfully read from beginning to end.
    if (result.second) {
        std::cout << "Entity id: " << receivedEntity.id << ", input: "
        << static_cast<unsigned int>(receivedEntity.input) << ", fs: ";
        for (float value : receivedEntity.fs) {
            std::cout << value << " ";
        }
        std::cout << std::endl;
    }
    else {
        std::cout << "Error reading: " << static_cast<unsigned int>(result.first) << std::endl;
    }

    return 0;
}

