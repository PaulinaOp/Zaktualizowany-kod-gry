#include <raylib.h>
#include <iostream>

Color Dark_Blue = Color{0, 82, 172, 255};
Color Gold = Color{255, 203, 0, 255};
Color White = Color{255, 255, 255, 255};
Color Black = Color{0, 0, 0, 255};
Color Light_Grey = Color{200, 200, 200, 255};

int player_score = 0;
int cpu_score = 0;

class Ball {
public:
    float x, y;
    int speed_x, speed_y;
    int radius;

    void Draw() {
        DrawCircle(x, y, radius, Gold);
    }

    void Update() {
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
            player_score++;
            ResetBall();
        }
    }

    void ResetBall() {
        x = GetScreenWidth() / 2;
        y = GetScreenHeight() / 2;

        int speed_choices[2] = {-1, 1};
        speed_x *= speed_choices[GetRandomValue(0, 1)];
        speed_y *= speed_choices[GetRandomValue(0, 1)];
    }
};

class Paddle {
protected:
    void LimitMovement() {
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
        DrawRectangleRounded(Rectangle{x, y, width, height}, 0.8, 0, WHITE);
    }

    void Update() {
        if (IsKeyDown(KEY_UP)) {
            y = y - speed;
        }
        if (IsKeyDown(KEY_DOWN)) {
            y = y + speed;
        }
        LimitMovement();
    }
};

class CpuPaddle : public Paddle {
public:
    void Update(int ball_y){
        int randomOffset = GetRandomValue(-height / 5, height / 5);

        if (y + height / 2 + randomOffset > ball_y) {
            y = y - speed;
        }
        if (y + height / 2 + randomOffset <= ball_y) {
            y = y + speed;
        }
        LimitMovement();
    }
};

Ball ball;
Paddle player;
CpuPaddle cpu;

int lineWidth = 5;
bool game_started = false;

int main() {
    const int screen_width = 1280;
    const int screen_height = 680;
    InitWindow(screen_width, screen_height, "Ping Pong");
    InitAudioDevice();
    Sound paddle_sound = LoadSound("paddle_hit.wav");
    Sound score_sound = LoadSound("score.wav");
    SetTargetFPS(60);
    ball.radius = 20;
    ball.x = screen_width / 2;
    ball.y = screen_height / 2;
    ball.speed_x = 7;
    ball.speed_y = 7;

    player.width = 25;
    player.height = 120;
    player.x = screen_width - player.width - 10;
    player.y = screen_height / 2 - player.height / 2;
    player.speed = 6;

    cpu.height = 120;
    cpu.width = 25;
    cpu.x = 10;
    cpu.y = screen_height / 2 - cpu.height / 2;
    cpu.speed = 6;

    while (!WindowShouldClose()) {
        if (!game_started) {
            BeginDrawing();
            ClearBackground(Dark_Blue);
            DrawText("Press ENTER to start", screen_width / 2 - 200, screen_height / 2 - 20, 40, White);
            EndDrawing();
            if (IsKeyPressed(KEY_ENTER)) {
                game_started = true;
            }
            continue;
        }

        BeginDrawing();

        ball.Update();
        player.Update();
        cpu.Update(ball.y);

        if (CheckCollisionCircleRec({ball.x, ball.y}, ball.radius, {player.x, player.y, player.width, player.height})) {
            ball.speed_x *= -1;
            PlaySound(paddle_sound);
        }

        if (CheckCollisionCircleRec({ball.x, ball.y}, ball.radius, {cpu.x, cpu.y, cpu.width, cpu.height})) {
            ball.speed_x *= -1;
            PlaySound(paddle_sound);
        }

        if (player_score > 0 || cpu_score > 0) {
            ClearBackground(Light_Grey);
        } else {
            ClearBackground(Dark_Blue);
        }
        DrawRectangle(screen_width / 2, 0, screen_width / 2, screen_height, Dark_Blue);
        DrawLineEx(Vector2{screen_width / 2, 0}, Vector2{screen_width / 2, screen_height}, lineWidth, WHITE);
        DrawLine( 0, screen_height / 2, screen_width, screen_height / 2, WHITE);
        ball.Draw();
        cpu.Draw();
        player.Draw();
        DrawText(TextFormat("%i", cpu_score), screen_width / 4 - 20, 20, 80, WHITE);
        DrawText(TextFormat("%i", player_score), 3 * screen_width / 4 - 20, 20, 80, WHITE);

        if (player_score >= 15) {
            DrawText(" YOU WON!!!", screen_width / 2 - 150, screen_height / 2 - 10, 40, GetFrameTime() * 10 > 0.5f ? White : Black);
            ball.speed_x = 0;
            ball.speed_y = 0;

            if (IsKeyPressed(KEY_ENTER)) {
                break;
            }
        }
        if (cpu_score >= 15) {
            DrawText(" Game Over...", screen_width / 2 - 150, screen_height / 2 - 10, 40, GetFrameTime() * 10 > 0.5f ? White : Black);
            ball.speed_x = 0;
            ball.speed_y = 0;

            if (IsKeyPressed(KEY_ENTER)) {
                break;
            }
        }

        EndDrawing();
    }
    
    CloseWindow();
    UnloadSound(paddle_sound);
    UnloadSound(score_sound);
    CloseAudioDevice();
    return 0;
}
