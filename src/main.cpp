#include <Arduino.h>
#include <TFT_eSPI.h>
#include "bird_image.h"
#include "splash_image.h"
#define acceleration 20
#define bird_spr_w 20
#define scrn_width 135
#define scrn_height 240
#define A 25
#define B 20
#define C 35
#define D 80

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite bird = TFT_eSprite(&tft);
TFT_eSprite bird_rotated = TFT_eSprite(&tft);
TFT_eSprite screen = TFT_eSprite(&tft);
TFT_eSprite cloud = TFT_eSprite(&tft);
TFT_eSprite pipes = TFT_eSprite(&tft);

unsigned long old_time = 0.0;
float Delta_time = 0.0;
double y_bird = 0;
double old_y_bird = 0;
double vel_bird = 0;
double x_pipes = 135;
double pipes_vel = 25;
double level = 1;
int pipe_edge = random(20, 141);

int points = 0;
int old_points = -1;
int caso = 0;
bool btwn_tube = true;

struct cloud_data
{
  double x_pos;
  double y_pos;
  double x_vel;
};

cloud_data cloud_1 = {(double)random(0, 136), (double)random(0, 241), (double)random(3, 12)};
cloud_data cloud_2 = {(double)random(0, 136), (double)random(0, 241), (double)random(3, 12)};
cloud_data cloud_3 = {(double)random(0, 136), (double)random(0, 241), (double)random(3, 12)};

void reset_cloud(cloud_data &cloud_ad)
{
  cloud_ad.x_pos = 135;
  cloud_ad.y_pos = (double)random(0, 241);
  cloud_ad.x_vel = (double)random(3, 12);
}

void make_cloud()
{
  cloud.createSprite(50, 25);
  cloud.fillSprite(TFT_TRANSPARENT);
  cloud.fillCircle(9, 18, 5, TFT_WHITE);
  cloud.fillCircle(20, 11, 10, TFT_WHITE);
  cloud.fillCircle(27, 16, 7, TFT_WHITE);
  cloud.fillCircle(31, 9, 6, TFT_WHITE);
  cloud.fillCircle(40, 16, 8, TFT_WHITE);
}

void make_pipes(int rand_place)
{
  pipes.createSprite(C, 240);
  pipes.fillSprite(TFT_TRANSPARENT);
  pipes.fillRect((C - A) / 2, 0, A, rand_place - B, TFT_GREEN);
  pipes.drawRect((C - A) / 2, 0, A, rand_place - B, TFT_BLACK);
  pipes.fillRect(0, rand_place - B, C, B, TFT_GREEN);
  pipes.drawRect(0, rand_place - B, C, B, TFT_BLACK);
  pipes.fillRect(0, rand_place + D, C, B, TFT_GREEN);
  pipes.drawRect(0, rand_place + D, C, B, TFT_BLACK);
  pipes.fillRect((C - A) / 2, rand_place + D + B, A, scrn_height - rand_place - D - B, TFT_GREEN);
  pipes.drawRect((C - A) / 2, rand_place + D + B, A, scrn_height - rand_place - D - B, TFT_BLACK);
}

void make_bird()
{
  bird.createSprite(bird_spr_w, bird_spr_w);
  bird.setSwapBytes(true);
  bird.fillSprite(TFT_TRANSPARENT);
  bird.pushImage(0, 0, 20, 20, bird_img);
  bird.setPivot(10, 10);
  
  // BASIC SHAPE VERSION
  // bird.fillCircle(15, 15, 15, TFT_YELLOW);
  // bird.fillCircle(24, 9, 7, TFT_WHITE);
  // bird.fillCircle(26, 9, 3, TFT_BLACK);
  // bird.fillRect(11, 19, 23, 6, TFT_ORANGE);
  // bird.drawFastHLine(18, 22, 16, TFT_BLACK);
}

void make_bird_rotated()
{
  bird_rotated.createSprite(bird_spr_w, bird_spr_w);
  bird_rotated.fillSprite(TFT_TRANSPARENT);
  bird_rotated.setPivot(10, 10);
}

void make_screen()
{
  screen.createSprite(135, 240);
  screen.fillSprite(TFT_SKYBLUE);
}

void setup()
{
  Serial.begin(9600);
  tft.init();
  tft.setSwapBytes(true);
  tft.fillScreen(TFT_BLACK);
  make_bird();
  make_bird_rotated();
  make_screen();
  make_cloud();
  make_pipes(pipe_edge);
  pinMode(0, INPUT_PULLUP);
  pinMode(35, INPUT_PULLUP);
}

void loop()
{
  switch (caso)
  {
  case 0:
    // tft.drawString("Press a key", 10, 40, 4);
    // tft.drawString("to start...", 10, 80, 4);
    tft.pushImage(0,0,135,240,splash_img);
    if (!digitalRead(0) || !digitalRead(35))
    {
      caso = 1;
      reset_cloud(cloud_1);
      reset_cloud(cloud_2);
      reset_cloud(cloud_3);
      level = 1;
      points = 0;
      old_points = -1;
      y_bird = 0;
      old_y_bird = 0;
      vel_bird = 0;
      x_pipes = 135;
      pipe_edge = random(20, 141);
      make_pipes(pipe_edge);
      btwn_tube = true;
    }
    break;
  case 1:
    old_time = micros();
    // CLOUD DRAWING //
    level = 1 + points / 10;
    cloud.pushToSprite(&screen, cloud_1.x_pos, cloud_1.y_pos, TFT_TRANSPARENT);
    cloud.pushToSprite(&screen, cloud_2.x_pos, cloud_2.y_pos, TFT_TRANSPARENT);
    cloud.pushToSprite(&screen, cloud_3.x_pos, cloud_3.y_pos, TFT_TRANSPARENT);
    cloud_1.x_pos -= cloud_1.x_vel * Delta_time;
    cloud_2.x_pos -= cloud_2.x_vel * Delta_time;
    cloud_3.x_pos -= cloud_3.x_vel * Delta_time;
    if (cloud_1.x_pos < -50)
    {
      reset_cloud(cloud_1);
    }
    if (cloud_2.x_pos < -50)
    {
      reset_cloud(cloud_2);
    }
    if (cloud_3.x_pos < -50)
    {
      reset_cloud(cloud_3);
    }
    // CLOUD DRAWING END//

    // PIPES DRAWING SECTION //
    x_pipes -= pipes_vel * Delta_time * level;
    pipes.pushToSprite(&screen, x_pipes, 0, TFT_TRANSPARENT);
    // END OF PIPE DRAWING SECTION //

    // BIRD DRAWING SECTION //
    if (y_bird > old_y_bird + 3)
    {
      bird.pushRotated(&bird_rotated, (y_bird - old_y_bird) * 2, TFT_BLACK);
      old_y_bird = y_bird;
    }
    else if (y_bird < old_y_bird - 3)
    {
      bird.pushRotated(&bird_rotated, (y_bird - old_y_bird) * 2, TFT_BLACK);
      old_y_bird = y_bird;
    }
    else
    {
      bird.pushRotated(&bird_rotated, 0, TFT_BLACK);
      old_y_bird = y_bird;
    }
    vel_bird += acceleration;
    if (!digitalRead(0) || !digitalRead(35))
    {
      vel_bird = -150 * level;
    }
    y_bird += vel_bird * Delta_time;
    bird_rotated.pushToSprite(&screen, 20, y_bird, TFT_TRANSPARENT);
    bird_rotated.fillSprite(TFT_TRANSPARENT);
    // END OF BIRD DRAWING SECTION on the screen sprite //

    // SCREEN DRAWING //
    screen.drawString("score:", 2, 2, 2);
    screen.drawString(String(points), 40, 2, 2);
    screen.pushSprite(0, 0, TFT_TRANSPARENT);
    screen.fillSprite(TFT_SKYBLUE); // REFRESH
    // SCREEN DRAWING END //

    // COLLISION DETECTION //
    if (x_pipes <= 20 + bird_spr_w - 2 && btwn_tube == 1) // collision zone entering i flag that i'm between the tube
    {
      if (y_bird > pipe_edge && y_bird < pipe_edge + 80 - bird_spr_w)
      {
        btwn_tube = 1;
        if (x_pipes < -15)
        {
          points++;
          btwn_tube = 0;
        }
      }
      else
      {
        btwn_tube = 0;
        caso = 2;
        tft.fillScreen(TFT_BLACK);
      }
    }

    if (y_bird <= 0)
    {
      y_bird = 0;
    }

    if (y_bird > 225)
    {
      caso = 2;
      tft.fillScreen(TFT_BLACK);
    }

    //

    // PIPE RESET
    if (points != old_points)
    {
      if (x_pipes < -35)
      {
        x_pipes = 135;
        pipe_edge = random(20, 141);
        make_pipes(pipe_edge);
        old_points = points;
        btwn_tube = true;
      }
    }

    // Serial.println(btwn_tube);
    Delta_time = (float)(micros() - old_time) / 1000000.0;
    break;
  case 2:
    delay(100);
    tft.drawString("GAME", 30, 40, 4);
    tft.drawString("OVER", 30, 80, 4);
    tft.drawString("SCORE: ", 5, 150, 4);
    tft.drawString(String(points), 100, 150, 4);
    if (!digitalRead(0) || !digitalRead(35))
    {
      caso = 0;
      tft.fillScreen(TFT_BLACK);
    }
    break;
  }
}
