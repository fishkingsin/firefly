import controlP5.*;
import oscP5.*;
import netP5.*;

OscP5 oscP5;
NetAddress myRemoteLocation;

ControlP5 cp5;
String textValue = "";
PImage image;
File file;
void setup() {
  size(800, 400);
  frameRate(25);
  oscP5 = new OscP5(this, 3333);
  myRemoteLocation = new NetAddress("127.0.0.1", 7110);


  PFont font = createFont("arial", 20);

  cp5 = new ControlP5(this);
  int x = 20, y = 20;

  cp5.addTextfield("ipAddress")
    .setPosition(x, y)
    .setSize(200, 40)
    .setFont(createFont("arial", 20))
    .setAutoClear(false);
  cp5.addBang("OK")
    .setPosition(x+220, y)
    .setSize(80, 40)
    .getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER)
    ;  
  cp5.addBang("clear")
    .setPosition(x+330, y)
    .setSize(80, 40)
    .getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER)
    ;  
  cp5.addBang("off")
    .setPosition(x+220, y+50)
    .setSize(80, 40)
    .getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER)
    ;  
  cp5.get(Textfield.class, "ipAddress").setText("0.0.0.0");

  cp5.addScrollableList("dropdown")
    .setPosition(x, y+100)
    .setSize(200, 200)
    .setBarHeight(20)
    .setItemHeight(20)
    // .setType(ScrollableList.LIST) // currently supported DROPDOWN and LIST
    ;
  selectFolder("choose a folder containing images", "selectedFile");
}
void selectedFile(File selection) {

  if (selection!=null) {
    file = selection;
    String[] list = selection.list();
    if (list == null) {

      println("Folder does not exist or cannot be accessed.");
    } else {
      //println(list);

      ScrollableList l = cp5.get(ScrollableList.class, "dropdown");
      for (String item : list ) {
        if (item.contains(".png")) {
          l.addItem(item, item);
          l.getItem(item).put("color", new CColor().setBackground(0xffff0000).setBackground(0xffff8800));
        }
      }
    }
  } //<>//
}
void dropdown(int n) {
  /* request the selected item based on index n */
  println(n, cp5.get(ScrollableList.class, "dropdown").getItem(n));

  /* here an item is stored as a Map  with the following key-value pairs:
   * name, the given name of the item
   * text, the given text of the item by default the same as name
   * value, the given value of the item, can be changed by using .getItem(n).put("value", "abc"); a value here is of type Object therefore can be anything
   * color, the given color of the item, how to change, see below
   * view, a customizable view, is of type CDrawable 
   */

  CColor c = new CColor();
  c.setBackground(color(255, 0, 0));
  cp5.get(ScrollableList.class, "dropdown").getItem(n).put("color", c);

  OscMessage myMessage = new OscMessage("/change_image");
  String value = cp5.get(ScrollableList.class, "dropdown").getItem(n).get("value").toString();
  myMessage.add(value); 
  /* send the message */
  oscP5.send(myMessage, myRemoteLocation);
  image = loadImage(file+"/"+value);
}

void draw() {
  background(0);
  //fill(255);
  //text(cp5.get(Textfield.class, "ipAddress").getText(), 360, 130);
  //text(textValue, 360, 180);
  if (image!=null) {
    image(image, 20, height-image.height);
  }
}
public void clear() {
  cp5.get(Textfield.class, "ipAddress").clear();
}
public void off() {
  OscMessage myMessage = new OscMessage("/off");
 
  myMessage.add("off"); 
  /* send the message */
  oscP5.send(myMessage, myRemoteLocation);
}
public void OK() {
  myRemoteLocation = new NetAddress(cp5.get(Textfield.class, "ipAddress").getText(), 7110);
}

/* incoming osc message are forwarded to the oscEvent method. */
void oscEvent(OscMessage theOscMessage) {
  /* print the address pattern and the typetag of the received OscMessage */
  print("### received an osc message.");
  print(" addrpattern: "+theOscMessage.addrPattern());
  println(" typetag: "+theOscMessage.typetag());
}