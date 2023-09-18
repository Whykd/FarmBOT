// -*- mode: c++; c-basic-offset: 2; indent-tabs-mode: nil; -*-
// Small example how to use the library.
// For more examples, look at demo-main.cc
//
// This code is public domain
// (but note, that the led-matrix library this depends on is GPL v2)

#include "led-matrix.h"
#include <iostream>
#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <signal.h>

#include <fstream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

using namespace std;

std::ifstream f("test.json");
json data = json::parse(f);


using rgb_matrix::RGBMatrix;
using rgb_matrix::Canvas;

volatile bool interrupt_received = false;
static void InterruptHandler(int signo) {
  interrupt_received = true;
}

static void DrawOnCanvas(Canvas *canvas) {
  /*
   * Let's create a simple animation. We use the canvas to draw
   * pixels. We wait between each step to have a slower animation.
   */
    canvas->Fill(100, 100, 100);
    usleep(1 * 10000);  // wait a little to slow down things.
    for (int y = 0; y < 32; y++){
      for (int x = 0; x < 64; x++){
        if (interrupt_received)
          return;
        int red = (rand()%256);
        int g = (rand()%256);
        int b = (rand()%256);
        canvas->SetPixel(x, y, red, g, b); 
      }
    }
    usleep(1 * 10000); 
}

int main(int argc, char *argv[]) {
  cout << data;
  RGBMatrix::Options defaults;
  defaults.hardware_mapping = "adafruit-hat";  // or e.g. "adafruit-hat"
  defaults.rows = 32;
  defaults.chain_length = 1;
  defaults.parallel = 1;
  defaults.show_refresh_rate = false;
  Canvas *canvas = RGBMatrix::CreateFromFlags(&argc, &argv, &defaults);
  if (canvas == NULL)
    return 1;

  // It is always good to set up a signal handler to cleanly exit when we
  // receive a CTRL-C for instance. The DrawOnCanvas() routine is looking
  // for that.
  signal(SIGTERM, InterruptHandler);
  signal(SIGINT, InterruptHandler);

  DrawOnCanvas(canvas);    // Using the canvas.

  // Animation finished. Shut down the RGB matrix.
  canvas->Clear();
  delete canvas;

  return 0;
}
