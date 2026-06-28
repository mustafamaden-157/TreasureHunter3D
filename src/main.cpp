#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>
#include <OpenGL/glu.h>

#include <cmath>
#include <iostream>
#include <vector>
#include <string>

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 700;

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

struct Vec3 {
    float x;
    float y;
    float z;
};

struct Treasure {
    Vec3 position;
    bool collected;
};

std::vector<Treasure> treasures = {
    {{-3.0f, 0.35f, -3.0f}, false},
    {{ 2.0f, 0.35f, -4.0f}, false},
    {{ 3.0f, 0.35f,  2.0f}, false},
    {{-2.0f, 0.35f,  3.0f}, false},
    {{ 0.0f, 0.35f,  0.0f}, false}
};

float playerX = -3.5f;
float playerY = 0.65f;
float playerZ = -3.5f;

float yawAngle = -90.0f;
float pitchAngle = 0.0f;

float lastMouseX = SCREEN_WIDTH / 2.0f;
float lastMouseY = SCREEN_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

GLuint wallTexture = 0;
GLuint floorTexture = 0;
GLuint treasureTexture = 0;

int score = 0;


void updateWindowTitle(GLFWwindow* window) {
    std::string title = "TreasureHunter3D - Skor: " + std::to_string(score) + "/" + std::to_string(treasures.size());
    glfwSetWindowTitle(window, title.c_str());
}


float degToRad(float degree) {
    return degree * 3.14159265f / 180.0f;
}

void createCheckerTexture(GLuint& textureId, unsigned char r1, unsigned char g1, unsigned char b1,
                          unsigned char r2, unsigned char g2, unsigned char b2) {
    const int size = 64;
    unsigned char data[size * size * 3];

    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            int checker = ((x / 8) + (y / 8)) % 2;
            int index = (y * size + x) * 3;

            if (checker == 0) {
                data[index + 0] = r1;
                data[index + 1] = g1;
                data[index + 2] = b1;
            } else {
                data[index + 0] = r2;
                data[index + 1] = g2;
                data[index + 2] = b2;
            }
        }
    }

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
}

void setupTextures() {
    createCheckerTexture(wallTexture, 95, 95, 95, 130, 130, 130);
    createCheckerTexture(floorTexture, 70, 110, 70, 95, 140, 95);
    createCheckerTexture(treasureTexture, 230, 170, 30, 255, 220, 80);
}

void setupLighting() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    GLfloat lightPosition[] = {0.0f, 6.0f, 3.0f, 1.0f};
    GLfloat lightAmbient[]  = {0.25f, 0.25f, 0.25f, 1.0f};
    GLfloat lightDiffuse[]  = {0.85f, 0.85f, 0.85f, 1.0f};
    GLfloat lightSpecular[] = {0.60f, 0.60f, 0.60f, 1.0f};

    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
}

void setPerspective(float fovY, float aspect, float nearPlane, float farPlane) {
    float top = tanf(degToRad(fovY) / 2.0f) * nearPlane;
    float bottom = -top;
    float right = top * aspect;
    float left = -right;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(left, right, bottom, top, nearPlane, farPlane);
    glMatrixMode(GL_MODELVIEW);
}

bool isWallAtPosition(float x, float z) {
    int mapX = static_cast<int>(std::floor(x + MAP_W / 2.0f));
    int mapZ = static_cast<int>(std::floor(z + MAP_H / 2.0f));

    if (mapX < 0 || mapX >= MAP_W || mapZ < 0 || mapZ >= MAP_H) {
        return true;
    }

    return levelMap[mapZ][mapX] == 1;
}

bool canMoveTo(float newX, float newZ) {
    float radius = 0.18f;

    if (isWallAtPosition(newX - radius, newZ - radius)) return false;
    if (isWallAtPosition(newX + radius, newZ - radius)) return false;
    if (isWallAtPosition(newX - radius, newZ + radius)) return false;
    if (isWallAtPosition(newX + radius, newZ + radius)) return false;

    return true;
}

void checkTreasureCollection(GLFWwindow* window) {
    for (auto& treasure : treasures) {
        if (treasure.collected) {
            continue;
        }

        float dx = playerX - treasure.position.x;
        float dz = playerZ - treasure.position.z;
        float distance = std::sqrt(dx * dx + dz * dz);

        if (distance < 1.25f) {
            treasure.collected = true;
            score++;
            std::cout << "Hazine toplandi! Skor: " << score << "/" << treasures.size() << std::endl;
            updateWindowTitle(window);
        }
    }
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastMouseX = static_cast<float>(xpos);
        lastMouseY = static_cast<float>(ypos);
        firstMouse = false;
    }

    float xoffset = static_cast<float>(xpos) - lastMouseX;
    float yoffset = lastMouseY - static_cast<float>(ypos);

    lastMouseX = static_cast<float>(xpos);
    lastMouseY = static_cast<float>(ypos);

    float sensitivity = 0.10f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yawAngle += xoffset;
    pitchAngle += yoffset;

    if (pitchAngle > 89.0f) pitchAngle = 89.0f;
    if (pitchAngle < -89.0f) pitchAngle = -89.0f;
}

void processInput(GLFWwindow* window) {
    float speed = 2.8f * deltaTime;

    float yawRad = degToRad(yawAngle);

    // Kamera hangi yatay yöne bakıyorsa W o yöne gider.
    float forwardX = std::cos(yawRad);
    float forwardZ = std::sin(yawRad);

    // Sağ yön, ileri yönün sağa çevrilmiş halidir.
    float rightX = -forwardZ;
    float rightZ = forwardX;

    float moveX = 0.0f;
    float moveZ = 0.0f;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        moveX += forwardX * speed;
        moveZ += forwardZ * speed;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        moveX -= forwardX * speed;
        moveZ -= forwardZ * speed;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        moveX -= rightX * speed;
        moveZ -= rightZ * speed;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        moveX += rightX * speed;
        moveZ += rightZ * speed;
    }
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    // X ve Z ayrı kontrol edilir. Böylece duvara değince tamamen kilitlenmez.
    float candidateX = playerX + moveX;
    if (canMoveTo(candidateX, playerZ)) {
        playerX = candidateX;
    }

    float candidateZ = playerZ + moveZ;
    if (canMoveTo(playerX, candidateZ)) {
        playerZ = candidateZ;
    }

    checkTreasureCollection(window);
}

void drawTexturedCube(float x, float y, float z, float sx, float sy, float sz, GLuint textureId) {
    glPushMatrix();

    glTranslatef(x, y, z);
    glScalef(sx, sy, sz);

    glBindTexture(GL_TEXTURE_2D, textureId);

    glBegin(GL_QUADS);

    glNormal3f(0, 0, 1);
    glTexCoord2f(0, 0); glVertex3f(-0.5f, -0.5f,  0.5f);
    glTexCoord2f(1, 0); glVertex3f( 0.5f, -0.5f,  0.5f);
    glTexCoord2f(1, 1); glVertex3f( 0.5f,  0.5f,  0.5f);
    glTexCoord2f(0, 1); glVertex3f(-0.5f,  0.5f,  0.5f);

    glNormal3f(0, 0, -1);
    glTexCoord2f(1, 0); glVertex3f(-0.5f, -0.5f, -0.5f);
    glTexCoord2f(1, 1); glVertex3f(-0.5f,  0.5f, -0.5f);
    glTexCoord2f(0, 1); glVertex3f( 0.5f,  0.5f, -0.5f);
    glTexCoord2f(0, 0); glVertex3f( 0.5f, -0.5f, -0.5f);

    glNormal3f(0, 1, 0);
    glTexCoord2f(0, 1); glVertex3f(-0.5f,  0.5f, -0.5f);
    glTexCoord2f(0, 0); glVertex3f(-0.5f,  0.5f,  0.5f);
    glTexCoord2f(1, 0); glVertex3f( 0.5f,  0.5f,  0.5f);
    glTexCoord2f(1, 1); glVertex3f( 0.5f,  0.5f, -0.5f);

    glNormal3f(0, -1, 0);
    glTexCoord2f(1, 1); glVertex3f(-0.5f, -0.5f, -0.5f);
    glTexCoord2f(0, 1); glVertex3f( 0.5f, -0.5f, -0.5f);
    glTexCoord2f(0, 0); glVertex3f( 0.5f, -0.5f,  0.5f);
    glTexCoord2f(1, 0); glVertex3f(-0.5f, -0.5f,  0.5f);

    glNormal3f(1, 0, 0);
    glTexCoord2f(1, 0); glVertex3f(0.5f, -0.5f, -0.5f);
    glTexCoord2f(1, 1); glVertex3f(0.5f,  0.5f, -0.5f);
    glTexCoord2f(0, 1); glVertex3f(0.5f,  0.5f,  0.5f);
    glTexCoord2f(0, 0); glVertex3f(0.5f, -0.5f,  0.5f);

    glNormal3f(-1, 0, 0);
    glTexCoord2f(0, 0); glVertex3f(-0.5f, -0.5f, -0.5f);
    glTexCoord2f(1, 0); glVertex3f(-0.5f, -0.5f,  0.5f);
    glTexCoord2f(1, 1); glVertex3f(-0.5f,  0.5f,  0.5f);
    glTexCoord2f(0, 1); glVertex3f(-0.5f,  0.5f, -0.5f);

    glEnd();

    glPopMatrix();
}

void drawScene() {
    drawTexturedCube(0.0f, -0.05f, 0.0f, 12.0f, 0.1f, 12.0f, floorTexture);

    for (int z = 0; z < MAP_H; z++) {
        for (int x = 0; x < MAP_W; x++) {
            if (levelMap[z][x] == 1) {
                float worldX = static_cast<float>(x) - MAP_W / 2.0f + 0.5f;
                float worldZ = static_cast<float>(z) - MAP_H / 2.0f + 0.5f;
                drawTexturedCube(worldX, 0.5f, worldZ, 1.0f, 1.0f, 1.0f, wallTexture);
            }
        }
    }

    float time = static_cast<float>(glfwGetTime());

    for (const auto& treasure : treasures) {
        if (treasure.collected) {
            continue;
        }

        glPushMatrix();
        glTranslatef(treasure.position.x, treasure.position.y, treasure.position.z);
        glRotatef(time * 80.0f, 0.0f, 1.0f, 0.0f);
        drawTexturedCube(0.0f, 0.0f, 0.0f, 0.35f, 0.35f, 0.35f, treasureTexture);
        glPopMatrix();
    }
}

void applyCamera() {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    float yawRad = degToRad(yawAngle);
    float pitchRad = degToRad(pitchAngle);

    float dirX = std::cos(yawRad) * std::cos(pitchRad);
    float dirY = std::sin(pitchRad);
    float dirZ = std::sin(yawRad) * std::cos(pitchRad);

    gluLookAt(
        playerX, playerY, playerZ,
        playerX + dirX, playerY + dirY, playerZ + dirZ,
        0.0f, 1.0f, 0.0f
    );
}

int main() {
    if (!glfwInit()) {
        std::cerr << "GLFW baslatilamadi." << std::endl;
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "TreasureHunter3D - OpenGL Hazine Toplama Oyunu", NULL, NULL);

    if (!window) {
        std::cerr << "Pencere olusturulamadi." << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);

    setupTextures();
    setupLighting();

    std::cout << "TreasureHunter3D calisiyor." << std::endl;
    std::cout << "WASD ile hareket et, mouse ile kamerayi cevir, ESC ile cik." << std::endl;
    updateWindowTitle(window);

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        glClearColor(0.10f, 0.14f, 0.18f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        setPerspective(70.0f, static_cast<float>(SCREEN_WIDTH) / SCREEN_HEIGHT, 0.1f, 100.0f);
        applyCamera();

        setupLighting();
        drawScene();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
