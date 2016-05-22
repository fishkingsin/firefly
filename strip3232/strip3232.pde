import controlP5.*;
ControlP5 cp5;

OPC opc;
PImage im;
float speed = 1;
void setup()
{
  size(800, 400);

  cp5 = new ControlP5(this);

  // add a horizontal sliders, the value of this slider will be linked
  // to variable 'sliderValue' 
  cp5.addSlider("speed")
    .setPosition(100, 50)
    .setRange(0.01, 5)
    ;


  // Load a sample image
  im = loadImage("hello-01.png");

  // Connect to the local instance of fcserver
  opc = new OPC(this, "10.0.1.9", 7890);
  //opc = new OPC(this, "127.0.0.1", 7890);
  opc.setStatusLed(true);
  // Map one 64-LED strip to the center of the window
  opc.ledStrip(0, 32, width/4 - 5, height/2, width/64, 0, false);
  opc.ledStrip(65, 32, width/4 * 3, height/2, width / 64, 0, false);
}

void draw()
{
  // Scale the image so that it matches the width of the window
  int imHeight = im.height * width / im.width;

  // Scroll down slowly, and wrap around
  
  float y = (millis() * -speed) % imHeight;

  // Use two copies of the image, so it seems to repeat infinitely  
  image(im, 0, y, width, imHeight);
  image(im, 0, y + imHeight, width, imHeight);
}

void stop() {

  opc.setStatusLed(false);

}

void keyPressed() {

  if (key == '1') {
    im = loadImage("hello-01.png");
  } else if (key == '2') {
    im = loadImage("arrow-01.png");
  } else if (key == '3') {
    im = loadImage("star-01.png");
  } else if (key == '4') {
    im = loadImage("square-01.png");
  } else if (key == '5') {
    im = loadImage("triangle-01.png");
  } else if (key == '6') {
    im = loadImage("rainbow-01.png");
  }
}