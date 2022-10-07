#define r1 5
#define r2 6
#define l1 7
#define l2 8

void motorR(speed,direction){

   //assigning as output pin
  if(direction==1) {
    digitalWrite(r2,0);
    analogWrite(r1,speed);
    
} else {
  digitalWrite(r1,0);
   analogWrite(r2,speed);
   
}
}
void motorL(speed,direction){

   //assigning as output pin
  if(direction==1) {
    digitalWrite(l2,0);
    analogWrite(l1,speed);
    
} else {
   digitalWrite(l1,0);
   analogWrite(l2,speed);
  
}
}
