#include"qtrx.h" // calibration(),sensorRead(),sensVal,linePos,lastPos
#include"motor.h" // motorL(),motorR(), all_stop()
//  that classifies the node
//  node 

//bool invalid = 0; // set 1 when the path leads to a dead end and clear after detecting next node (which is not added).. 
//clears the left most entry from the node list 
 // nodesens (type of node) comes from the LF/sensor data interpretation logic
 // A node is any point with 2 or 3 branches
 // at every node turn left or 180 incase of a dead end


#define Kp 2 // PID params 
#define Kd 1
#define samprate 100 // approx val
#define fwdSpeed 100
//int sensRaw[8]; 
//int sensMax[] = {0,0,0,0,0,0,0,0}; // init values
//int sensMin[] = {255,255,255,255,255,255,255,255};
bool mapped = 0; // 1 if arena is mapped
bool node[][3] = {{1,1,1},{1,1,0}}; //**dummy data** .. node info type graph
int path[] = {}; // final calculated path
unsigned short int pathNode = 0;
unsigned short int node_num = 0;
bool invalid =0;
//bool node = 1;
float error;
float last_error=0;
float adjustment = 0;


void node_arrange(bool n1=0, bool n2=0, bool n3=0) // organize the newly identified node
{
  if(!invalid){ // the left most entry from the last node lead to a valid node
    node[node_num][0] = n1; // add the node
    node[node_num][1] = n2;
    node[node_num++][2] = n3;
  }
  else{
    // handle the invalidity of the current node's left most path
    // invalid indicates that the bot has returned to the same node after making a 180 turn so no new node needs to be added
    // so delete the left most entry from the current node
    invalid = 0; // assumption for cases 1 and 2 
    if(node[node_num][0] == 1) // if left is 1 
      node[node_num][0]=0; // the current node is valid but the left path is invalid
    else if (node[node_num][1] == 1) // if centre is 1
      node[node_num][0]=0;// the right path is invalid
    else{// the current node is also invalid.. delete the current node
      node_num--;
      invalid = 1; // delete the left most of the last node .. sets up invalidity on reaching the next node 
    }
  }
}
int valSum = 0;
void lfr_logic() // mapping/unmapped logic .. node classification is the main goal!
{
readSensor();
  valSum = sensVal[0]+sensVal[1]+sensVal[2]+sensVal[3]+sensVal[4]+sensVal[5]+sensVal[6]+sensVal[7];
  if(valSum<1000 && valSum>300){ 
    // no node/dead end detected; LF case PID mode // add case incase the sensors detect all black (turn hard opposite the direction  where line was last detected )
  error= linePos;

  adjustment = Kp*error + Kd*((error-last_error)*samprate);

  last_error= error;

  motorL(fwdSpeed+adjustment,1);
  motorR(fwdSpeed-adjustment,1);
  }

  
  else{
    // node / 90 degree turn detect
    // reduce speed and classify the node then stop 
  // based on the classification take action
  // node arrange here based on node
  all_stop();
  
  int nodeType = 0;
  if(valSum > 1450 ) // fully covered line case LR both available
    nodeType = 2;
  else if(valSum < 300 ) // dead end  
    nodeType = 11;
  else if(linePos < -0.5) // left
    nodeType = 4;
  else if (linePos > 0.5) // right
    nodeType = 6;
  else // try to backup and check again 
    nodeType = 0;   // temporary 

  motorL(100,1); // move forward ~4-5 cm fwd path av
  motorR(100,1);

  readSensor(); // get new readings
  valSum = sensVal[0]+sensVal[1]+sensVal[2]+sensVal[3]+sensVal[4]+sensVal[5]+sensVal[6]+sensVal[7]; // calc new valsum
  if(valSum > 300)
    nodeType++; 
  if(valSum>1450 && nodeType == 2 ) // end box  
    nodeType = 10; // end
    
  if (nodeType == 4); // 90 deg left
  else if(nodeType == 6);// 90 deg right
  // ******** combine into 1
  else if(nodeType == 3){ // left straight right 111 detected
      // turn left
      node_arrange(1,1,1);
  }
  else if(nodeType == 5){ // left straight 110 detected
    // turn left
    node_arrange(1,1,0);
  }
  else if(nodeType == 7){// straight right 011
    // turn left
    node_arrange(0,1,1);
  }
  else if(nodeType == 2){ // left right 101
    // turn left
    node_arrange(1,0,1);
  }
  // **** combine into 1
  else if(nodeType == 11){ // dead end .. 180 moment
    // 180 turn
    invalid = 1; // invalidates the the current left most path on reaching the next node (same node from the opposite dirn hence not stored)
     
  }
  else if(nodeType == 10){ // end
    mapped =1;
    // generate path
    for(int i=0;i<node_num;i++){
      if(node[i][0] == 1)      // assign path in the order of left to right depending on availability 
        path[i] = 0; // assign left
      else if(node[i][1] == 1)
        path[i]= 1; // assign straight
      else if(node[i][2] == 1)
        path[i] = 2; // assign right
     //while(! button_pressed() ) // analogue
     // glow led // buzz 
    } // for end
   } // end end
  } // else end
} // fn end







void path_follow() //just follow the generated path and be fast as fuck !
{
  // node classification is not required .. just have to detect them now ... yay!
  if (1); // if node is not detected use LF logic optimized for path following
  
  else if (1){ // if any node is detected and pathNode < Nodenum
    if(pathNode<node_num){ // in path node list
      if(path[pathNode] == 0); // turn left
      else if(path[pathNode] == 1); // go straight
      else; // go right
      pathNode++; // move to the next path node turn info .. to be read on the next node
    }
    else{ // path node list is finished so this node has to be the end node
      // glow leds 
      while(1); // wait forever
    }
   
  }
  else;
    // dekhlena
 
}

// if mapped =1 then number of nodes in path is node_Num
void setup() {
  // put your setup code here, to run once:
TCCR2A = 0x0; // disable port2 for PWM and CTC
calibration(); // run calib routine
motorL(100,1);
motorR(100,1);
delay(1000);
all_stop();
}

void loop() {
  // put your main code here, to run repeatedly:

}
