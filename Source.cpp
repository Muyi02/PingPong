#include <iostream>
#include <deque>
#include <raylib.h>

using namespace std;

Color Green = Color{ 38, 185, 154, 255 };
Color Dark_Green = Color{ 20, 160, 133, 255 };
Color Lime = Color{ 129, 204, 184, 255 };
Color Yellow = Color{ 243, 213, 91, 255 };
Color BallColor = Yellow;

int p_score = 0;
int cpu_score = 0;

// Ball trail effect history
deque<Vector2> ballTrail;
const int trailLength = 10;  // Number of trail positions

class Ball {
public:
    float x, y;
    int speed_x, speed_y;
    int radius;

    void Draw() {
        // Draw ball trail
        for (int i = 0; i < ballTrail.size(); i++) {
            float alpha = (i + 1.0f) / ballTrail.size();
            DrawCircleV(ballTrail[i], radius, Fade(BallColor, alpha * 0.5f));
        }

        // Draw the actual ball
        DrawCircle(x, y, radius, BallColor);
    }

    void Refresh() {
        // Store current position in trail
        ballTrail.push_front(Vector2{ x, y });
        if (ballTrail.size() > trailLength) {
            ballTrail.pop_back();
        }

        x += speed_x;
        y += speed_y;

        if (y + radius >= GetScreenHeight() || y - radius <= 0) {
            speed_y *= -1;
        }

        if (x + radius >= GetScreenWidth()) {
            cpu_score++;
            ResetBall();
        }

        if (x - radius <= 0) {
            p_score++;
            ResetBall();
        }
    }

    void ResetBall() {
        x = GetScreenWidth() / 2;
        y = GetScreenHeight() / 2;

        int speed_choice[2] = { -1, 1 };
        speed_x *= speed_choice[GetRandomValue(0, 1)];
        speed_y *= speed_choice[GetRandomValue(0, 1)];

        BallColor = Yellow;  // Reset color on score
    }

    void ChangeColorOnCollision() {
        BallColor = RED;  // Change to red on collision
    }
};

class Paddle {
protected:
    void Limit() {
        if (y <= 0) {
            y = 0;
        }
        if (y + height >= GetScreenHeight()) {
            y = GetScreenHeight() - height;
        }
    }

public:
    float x, y;
    float width, height;
    int speed;

    void Draw() {
        DrawRectangleRounded(Rectangle{ x, y, width, height }, 0.8, 0, WHITE);
    }

    void Refresh() {
        if (IsKeyDown(KEY_UP)) {
            y = y - speed;
        }
        if (IsKeyDown(KEY_DOWN)) {
            y = y + speed;
        }
        Limit();
    }
};

class CPUPlayer : public Paddle {
public:
    void Refresh(int ball_y) {
        if (y + height / 2 > ball_y) {
            y = y - speed;
        }
        if (y + height / 2 <= ball_y) {
            y = y + speed;
        }
        Limit();
    }
};

Ball ball;
Paddle player;
CPUPlayer cpu;

int main() {
    cout << "Starting the game" << endl;
    const int canvas_width = 1280;
    const int canvas_height = 800;
    InitWindow(canvas_width, canvas_height, "Ping Muyi Pong!");
    SetTargetFPS(60);

    ball.radius = 20;
    ball.x = canvas_width / 2;
    ball.y = canvas_height / 2;
    ball.speed_x = 7;
    ball.speed_y = 7;

    player.width = 25;
    player.height = 120;
    player.x = canvas_width - player.width - 10;
    player.y = canvas_height / 2 - player.height / 2;
    player.speed = 6;

    cpu.height = 120;
    cpu.width = 25;
    cpu.x = 10;
    cpu.y = canvas_height / 2 - cpu.height / 2;
    cpu.speed = 6;

    while (!WindowShouldClose()) {
        BeginDrawing();

        ball.Refresh();
        player.Refresh();
        cpu.Refresh(ball.y);

        if (CheckCollisionCircleRec(Vector2{ ball.x, ball.y }, ball.radius,
            Rectangle{ player.x, player.y, player.width, player.height })) {
            ball.speed_x *= -1;
            ball.ChangeColorOnCollision();  // Change color on collision
        }

        if (CheckCollisionCircleRec(Vector2{ ball.x, ball.y }, ball.radius,
            Rectangle{ cpu.x, cpu.y, cpu.width, cpu.height })) {
            ball.speed_x *= -1;
            ball.ChangeColorOnCollision();  // Change color on collision
        }

        ClearBackground(Dark_Green);
        DrawRectangle(canvas_width / 2, 0, canvas_width / 2, canvas_height, Green);
        DrawCircle(canvas_width / 2, canvas_height / 2, 150, Lime);
        DrawLine(canvas_width / 2, 0, canvas_width / 2, canvas_height, WHITE);
        ball.Draw();
        cpu.Draw();
        player.Draw();

        DrawText(TextFormat("%i", cpu_score), canvas_width / 4 - 20, 20, 80, WHITE);
        DrawText(TextFormat("%i", p_score), 3 * canvas_width / 4 - 20, 20, 80, WHITE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}