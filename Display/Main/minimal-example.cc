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
    canvas->Fill(0, 0, 0);
    usleep(1 * 10000);  // wait a little to slow down things.
    for (int rows = 0; rows < 32; rows++){
      	canvas->SetPixel(0, rows, 0, 0, 0);
      	canvas->SetPixel(1, rows, 0, 0, 0);
      	canvas->SetPixel(62, rows, 0, 0, 0);
      	canvas->SetPixel(63, rows, 0, 0, 0);
    }
    for (int vals = 0; vals < 10; vals++){
        if(interrupt_received) return;
		int value = data[to_string(vals)]["Sensors"]["avg"];
		value = ((value * 32)/1024) + 1;
		for (int row = 1; row <= value; row++){
			canvas->SetPixel(vals + 2, 31 - (row-1), 100, 100, 100);
			if(interrupt_received) return;
		}
    }
    usleep(1 * 10000); //delay to be removed later maybe
}

int main(int argc, char *argv[]) {
  	RGBMatrix::Options defaults;
  	defaults.hardware_mapping = "adafruit-hat";  // or e.g. "adafruit-hat"
  	defaults.rows = 32;
  	defaults.cols = 64;
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
