__kernel void clkernel (__global const int* gol_h, __global int* res, const int num)
{
   /* get_global_id(0) returns the ID of the thread in execution.
   As many threads are launched at the same time, executing the same kernel,
   each one will receive a different ID, and consequently perform a different computation.*/
   const int idx = get_global_id(0);

   /* Now each work-item asks itself: "is my ID inside the vector's range?"
   If the answer is YES, the work-item performs the corresponding computation*/
   if (idx < num*num)
   {
      int n = 0;
      int x = idx%num;
      int y = idx/num;
      // not left edge
      if (x!=0){
        n += gol_h[idx-1]; 
        //not top
        if (y>0) {n += gol_h[idx-num-1];}    // oben links
        //not bottom
        if (y<num-1) {n += gol_h[idx+num-1];}  // unten links
      }
      // not right edge
      if (x!=num-1){
        n += gol_h[idx+1];        
        //not top
        if (y>0) {n += gol_h[idx-num+1];}    // oben rechts
        //not bottom
        if (y<num-1) {n += gol_h[idx+num+1];}  // unten rechts
      }
      if (y>0) {n += gol_h[idx-num];}  
      if (y<num-1) {n += gol_h[idx+num]; } 

      res[idx] = gol_h[idx]; 
      if ((gol_h[idx]==0 && n==3)||(gol_h[idx]==1 && (n<2||n>3)))  { res[idx]=1-res[idx];}
   }
}