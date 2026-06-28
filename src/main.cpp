#include <iostream>
#include <vector>
#include <cmath>
#include <string>

/*
    TreasureHunter3D
    Bilgisayar Grafikleri dersi için 3B hazine toplama oyunu iskeleti.

    Bu dosyada:
    - Oyuncu yapısı
    - Duvar ve hazine nesneleri
    - Labirent haritası
    - AABB çarpışma kontrolü
    - Hazine toplama ve skor mantığı
    bulunmaktadır.
*/

struct Vec3 {
    float x, y, z;

    Vec3(float xVal = 0, float yVal = 0, float zVal = 0)
        : x(xVal), y(yVal), z(zVal) {}

    Vec3 operator+(const Vec3& other) const {
        return Vec3(x + other.x, y + other.y, z + other.z);
    }

    Vec3 operator-(const Vec3& other) const {
        return Vec3(x - other.x, y - other.y, z - other.z);
    }

    Vec3 operator*(float scalar) const {
        return Vec3(x * scalar, y * scalar, z * scalar);
    }
};

struct AABB {
    Vec3 min;
    Vec3 max;
};

bool intersectsAABB(const AABB& a, const AABB& b) {
    return (
        a.min.x <= b.max.x && a.max.x >= b.min.x &&
        a.min.y <= b.max.y && a.max.y >= b.min.y &&
        a.min.z <= b.max.z && a.max.z >= b.min.z
    );
}

struct GameObject {
    std::string name;
    Vec3 position;
    Vec3 scale;
    bool active = true;

    AABB getBounds() const {
        Vec3 half = scale * 0.5f;

        return AABB{
            Vec3(position.x - half.x, position.y - half.y, position.z - half.z),
            Vec3(position.x + half.x, position.y + half.y, position.z + half.z)
        };
    }
};

struct Player {
    Vec3 position = Vec3(0.0f, 0.5f, 3.0f);
    Vec3 scale = Vec3(0.5f, 1.0f, 0.5f);
    float speed = 3.0f;

    AABB getBounds() const {
        Vec3 half = scale * 0.5f;

        return AABB{
            Vec3(position.x - half.x, position.y - half.y, position.z - half.z),
            Vec3(position.x + half.x, position.y + half.y, position.z + half.z)
        };
    }
};

// 1 = duvar, 0 = boş alan
const int MAP_W = 10;
const int MAP_H = 10;

int levelMap[MAP_H][MAP_W] = {
    {1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,1,0,0,0,0,1},
    {1,0,1,0,1,0,1,1,0,1},
    {1,0,1,0,0,0,0,1,0,1},
    {1,0,1,1,1,1,0,1,0,1},
    {1,0,0,0,0,1,0,0,0,1},
    {1,1,1,1,0,1,1,1,0,1},
    {1,0,0,1,0,0,0,1,0,1},
    {1,0,0,0,0,1,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1}
};

std::vector<GameObject> createWallsFromMap() {
    std::vector<GameObject> walls;

    for (int z = 0; z < MAP_H; z++) {
        for (int x = 0; x < MAP_W; x++) {
            if (levelMap[z][x] == 1) {
                GameObject wall;
                wall.name = "Wall";
                wall.position = Vec3(
                    static_cast<float>(x) - MAP_W / 2.0f,
                    0.5f,
                    static_cast<float>(z) - MAP_H / 2.0f
                );
                wall.scale = Vec3(1.0f, 1.0f, 1.0f);
                walls.push_back(wall);
            }
        }
    }

    return walls;
}

std::vector<GameObject> createTreasures() {
    std::vector<GameObject> treasures;

    std::vector<Vec3> positions = {
        Vec3(-3.0f, 0.35f, -3.0f),
        Vec3( 2.0f, 0.35f, -4.0f),
        Vec3( 3.0f, 0.35f,  2.0f),
        Vec3(-2.0f, 0.35f,  3.0f),
        Vec3( 0.0f, 0.35f,  0.0f)
    };

    for (int i = 0; i < static_cast<int>(positions.size()); i++) {
        GameObject treasure;
        treasure.name = "Treasure_" + std::to_string(i + 1);
        treasure.position = positions[i];
        treasure.scale = Vec3(0.35f, 0.35f, 0.35f);
        treasures.push_back(treasure);
    }

    return treasures;
}

bool canMoveTo(const Player& player, const Vec3& newPosition, const std::vector<GameObject>& walls) {
    Player temp = player;
    temp.position = newPosition;

    AABB playerBox = temp.getBounds();

    for (const auto& wall : walls) {
        if (intersectsAABB(playerBox, wall.getBounds())) {
            return false;
        }
    }

    return true;
}

void collectTreasures(Player& player, std::vector<GameObject>& treasures, int& score) {
    AABB playerBox = player.getBounds();

    for (auto& treasure : treasures) {
        if (!treasure.active) {
            continue;
        }

        if (intersectsAABB(playerBox, treasure.getBounds())) {
            treasure.active = false;
            score++;
            std::cout << "Hazine toplandi! Skor: " << score << std::endl;
        }
    }
}

bool allTreasuresCollected(const std::vector<GameObject>& treasures) {
    for (const auto& treasure : treasures) {
        if (treasure.active) {
            return false;
        }
    }

    return true;
}

void printProjectInfo() {
    std::cout << "TreasureHunter3D baslatildi." << std::endl;
    std::cout << "Bu dosya oyun mantigi, harita, carpışma ve skor sistemini icerir." << std::endl;
    std::cout << "Gercek OpenGL surumunde bu mantik render dongusune baglanacaktir." << std::endl;
}

int main() {
    printProjectInfo();

    Player player;
    std::vector<GameObject> walls = createWallsFromMap();
    std::vector<GameObject> treasures = createTreasures();

    int score = 0;

    std::cout << "Duvar sayisi: " << walls.size() << std::endl;
    std::cout << "Hazine sayisi: " << treasures.size() << std::endl;

    // Bu hareketler test amacli yazildi.
    // Gercek OpenGL surumunde hareketler klavyeden alinacaktir.
    std::vector<Vec3> testMoves = {
        Vec3(-0.5f, 0.0f, 0.0f),
        Vec3(-0.5f, 0.0f, 0.0f),
        Vec3( 0.0f, 0.0f,-0.5f),
        Vec3( 0.0f, 0.0f,-0.5f),
        Vec3( 0.5f, 0.0f, 0.0f)
    };

    for (const auto& move : testMoves) {
        Vec3 newPosition = player.position + move;

        if (canMoveTo(player, newPosition, walls)) {
            player.position = newPosition;

            std::cout << "Oyuncu hareket etti. Yeni konum: "
                      << player.position.x << ", "
                      << player.position.y << ", "
                      << player.position.z << std::endl;
        } else {
            std::cout << "Duvara carpildi. Hareket engellendi." << std::endl;
        }

        collectTreasures(player, treasures, score);
    }

    if (allTreasuresCollected(treasures)) {
        std::cout << "Tebrikler! Tum hazineler toplandi." << std::endl;
    } else {
        std::cout << "Kalan hazineler var. Oyun devam ediyor." << std::endl;
    }

    return 0;
}
