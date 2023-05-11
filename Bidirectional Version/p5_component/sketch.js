let backgroundImage;
let manualBgImage;
let screenState;

function preload() {
  backgroundImage = loadImage("bgImgVec3.svg");
  coverImage = loadImage("cover_photo.svg");
  manualBgImage = loadImage("bgImgManualVec.svg");
}

let leftRecv = 0; //ack of movement or detection
let rightRecv = 0; //direction taken after collision
let leftSend = 0; //will send the screenState
let rightSend = 0; //will send the direction in case of manual

class AcuteIsoscelesTriangle {
  constructor(x, y, sideLength, angle) {
    // Calculate the coordinates of the three points of the triangle
    this.x = x;
    this.y = y;
    this.sideLength = sideLength;
    this.angle = angle;
    this.x1 = x - sideLength / 2;
    this.y1 = y + (sideLength / 2) * tan(angle);
    this.x2 = x + sideLength / 2;
    this.y2 = y + (sideLength / 2) * tan(angle);
    this.x3 = x;
    this.y3 = y - sideLength / 2 / cos(angle);
    this.trail = [];
    // Store the angle of inclination of the vertex opposite to the non-equal side
    this.angle = angle;
  }

  // Function to draw the triangle
  display() {
    triangle(this.x1, this.y1, this.x2, this.y2, this.x3, this.y3);
  }

  // Function to move the triangle
  move(x, y) {
    // Update the coordinates of the three points of the triangle
    this.x1 += x;
    this.y1 += y;
    this.x2 += x;
    this.y2 += y;
    this.x3 += x;
    this.y3 += y;
  }

  rotate(angle) {
    // Convert angle to radians
    angle = radians(angle);

    // Calculate the new coordinates of the three points of the triangle after rotation
    let x1New =
      (this.x1 - this.x3) * cos(angle) -
      (this.y1 - this.y3) * sin(angle) +
      this.x3;
    let y1New =
      (this.x1 - this.x3) * sin(angle) +
      (this.y1 - this.y3) * cos(angle) +
      this.y3;
    let x2New =
      (this.x2 - this.x3) * cos(angle) -
      (this.y2 - this.y3) * sin(angle) +
      this.x3;
    let y2New =
      (this.x2 - this.x3) * sin(angle) +
      (this.y2 - this.y3) * cos(angle) +
      this.y3;
    let x3New =
      (this.x3 - this.x3) * cos(angle) -
      (this.y3 - this.y3) * sin(angle) +
      this.x3;
    let y3New =
      (this.x3 - this.x3) * sin(angle) +
      (this.y3 - this.y3) * cos(angle) +
      this.y3;

    // Update the coordinates of the three points of the triangle
    this.x1 = x1New;
    this.y1 = y1New;
    this.x2 = x2New;
    this.y2 = y2New;
    this.x3 = x3New;
    this.y3 = y3New;
  }
  respawn() {
    // Reset the position of the triangle to the center of the screen
    // console.log("Respawned");
    this.x1 = this.x - this.sideLength / 2;
    this.y1 = this.y + (this.sideLength / 2) * tan(this.angle);
    this.x2 = this.x + this.sideLength / 2;
    this.x2 = this.x + this.sideLength / 2;
    this.y2 = this.y + (this.sideLength / 2) * tan(this.angle);
    this.x3 = this.x;
    this.y3 = this.y - this.sideLength / 2 / cos(this.angle);
  }

  moveInDirection(distance) {
    let dx =
      (this.x3 - (this.x1 + this.x2) / 2) /
      dist((this.x1 + this.x2) / 2, (this.y1 + this.y2) / 2, this.x3, this.y3);
    let dy =
      (this.y3 - (this.y1 + this.y2) / 2) /
      dist((this.x1 + this.x2) / 2, (this.y1 + this.y2) / 2, this.x3, this.y3);

    // Move the triangle in the direction of its head by the specified distance
    this.move(dx * distance, dy * distance);

    // Check if any point of the triangle goes off the canvas
    if (
      this.x1 < 0 ||
      this.x2 < 0 ||
      this.y1 < 0 ||
      this.y2 < 0 ||
      this.x1 > width ||
      this.x2 > width ||
      this.y1 > height ||
      this.y2 > height
    ) {
      // Condition for respawn
      this.respawn();
    }
  }

  turnRight() {
    this.rotate(5);
  }
  turnLeft() {
    this.rotate(-5);
  }
  // Method to leave a trail of circles at the previous position of the triangle
  leaveTrail(radius) {
    // Create a new circle with the current position of the triangle

    let circle = {
      x: (this.x1 + this.x2 + this.x3) / 3,
      y: (this.y1 + this.y2 + this.y3) / 3,
      r: radius,
    };

    // Add the circle to the list of circles in the trail
    this.trail.push(circle);

    // // If the trail has more than 50 circles, remove the oldest circle
    // if (this.trail.length > 50) {
    //   this.trail.shift();
    // }

    // Draw the circles in the trail
    for (let i = 0; i < this.trail.length; i++) {
      // fill("red");
      noStroke();
      stroke("#cb6ce6");
      ellipse(
        this.trail[i].x,
        this.trail[i].y,
        this.trail[i].r,
        this.trail[i].r
      );
    }
  }
}

let button1X, button1Y, button1Width, button1Height;
let button2X, button2Y, button2Width, button2Height;

let tri;

function setup() {
  createCanvas(windowWidth, windowHeight);
  tri = new AcuteIsoscelesTriangle(windowWidth / 2, windowHeight / 2, 30, 45);
  background(0);
  screenState = 0;

  rectMode(CENTER);

  // set the position and size of the first button
  button1X = (3 * width) / 7;
  button1Y = height - 50;
  button1Width = 100;
  button1Height = 40;

  // set the position and size of the second button
  button2X = (4 * width) / 7;
  button2Y = height - 50;
  button2Width = 100;
  button2Height = 40;
}

function draw() {
  leftSend = screenState;

  if (0 == screenState) {
    background(coverImage);
  } else if (1 == screenState) {
    background(backgroundImage);

    tri.leaveTrail(8);
    fill("#000");
    stroke("#00BCD4");
    strokeWeight(3);
    tri.display();
    strokeWeight(1);
    stroke("#8c52ff");
    displayButtons();
    
    if (1 == leftRecv) {
      tri.moveInDirection(2);
    }
    else if(3 == leftRecv){
      tri.moveInDirection(-2);
      // tri.moveInDirection(-1);
      if(1 == rightRecv){
        //go back turn left
        tri.turnLeft();
        tri.turnLeft();
        tri.turnLeft();
        
      }
      else if(2 == rightRecv){
        //ho back turn right
        tri.turnRight();
        tri.turnRight();
        tri.turnRight();
        
      }
    }
    
    
    
  } else if (2 == screenState) {
    background(manualBgImage);
    tri.leaveTrail(8);
    fill("#000");
    stroke("#00BCD4");
    strokeWeight(3);
    tri.display();
    strokeWeight(1);
    stroke("#8c52ff");
    displayButtons();
    if (2 == leftRecv) {
      if (1 == rightRecv) {
        tri.moveInDirection(5);
      }
      else if (2 == rightRecv) {
        tri.moveInDirection(-5);
      }
      else if (3 == rightRecv) {
        tri.turnRight();
      }
      else if (4 == rightRecv) {
        tri.turnLeft();
      }
    }
  }
}

function mouseClicked() {
  if (screenState == 0 && mouseX < width / 2) {
    tri.respawn();
    screenState = 1;
  }

  if (screenState == 0 && mouseX > width / 2) {
    tri.respawn();
    screenState = 2;
  }

  if (
    (screenState == 1 || screenState == 2) &&
    mouseX > button1X - button1Width / 2 &&
    mouseX < button1X + button1Width / 2 &&
    mouseY > button1Y - button1Height / 2 &&
    mouseY < button1Y + button1Height / 2
  ) {
    // console.log("Button 1 clicked");
    screenState = 0;
    tri.trail = [];
  }

  if (
    (screenState == 1 || screenState == 2) &&
    mouseX > button2X - button2Width / 2 &&
    mouseX < button2X + button2Width / 2 &&
    mouseY > button2Y - button2Height / 2 &&
    mouseY < button2Y + button2Height / 2
  ) {
    // console.log("Button 2 clicked");
    saveCanvas("robot_playground", "png");
  }
}

function displayButtons() {
  // draw the first button
  rect(button1X, button1Y, button1Width, button1Height);
  // draw the second button
  rect(button2X, button2Y, button2Width, button2Height);
  // Draw text in the middle of the button
  textAlign(CENTER, CENTER);
  textSize(25);
  fill("#fff"); // black color
  text("Back", button1X, button1Y);
  text("Save", button2X, button2Y);
  fill("#000");
}

// function keyPressed() {

// }
// This function will be called by the web-serial library
// with each new *line* of data. The serial library reads
// the data until the newline and then gives it to us through
// this callback function
function readSerial(data) {
  ////////////////////////////////////
  //READ FROM ARDUINO HERE
  ////////////////////////////////////

  if (data != null) {
    // make sure there is actually a message
    // split the message
    let fromArduino = split(trim(data), ",");
    // if the right length, then proceed
    if (fromArduino.length == 2) {
      // only store values here
      // do everything with those values in the main draw loop

      // We take the string we get from Arduino and explicitly
      // convert it to a number by using int()
      // e.g. "103" becomes 103
      leftRecv = int(fromArduino[0]);
      rightRecv = int(fromArduino[1]);
      console.log(leftRecv + "," + rightRecv);
    }

    //////////////////////////////////
    //SEND TO ARDUINO HERE (handshake)
    //////////////////////////////////
    manualControls();
    let sendToArduino = leftSend + "," + rightSend + "\n";
    writeSerial(sendToArduino);

    // rightSend = 0;
  }
}

function keyPressed() {
  if (key == " ") {
    // important to have in order to start the serial connection!!
    setUpSerial();
  }
}

function manualControls() {
  if (screenState == 2 && keyIsPressed) {
    if (keyCode === UP_ARROW) {
      rightSend = 1;
      // tri.moveInDirection(10);
    } else if (keyCode === DOWN_ARROW) {
      // tri.moveInDirection(-10);
      rightSend = 2;
    } else if (keyCode === RIGHT_ARROW) {
      // tri.turnRight();
      rightSend = 3;
    } else if (keyCode === LEFT_ARROW) {
      // tri.turnLeft();
      rightSend = 4;
    } else {
      rightSend = 0;
    }
  } else {
    rightSend = 0;
  }
}
