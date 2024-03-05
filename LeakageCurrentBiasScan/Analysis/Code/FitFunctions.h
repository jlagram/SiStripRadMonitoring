#ifndef FitFunctions_h
#define FitFunctions_h

#include <math.h>

Double_t fitfunction(Double_t *x, Double_t *par){
  Double_t value, value_th, value_0;
  if (x[0]<par[2]) {
    value = par[0] + par[1]*sqrt(x[0]) + par[3]*x[0]*sqrt(x[0]); // x^1/2 + x^3/2
  }
  else
  {
    value_th = par[0] + par[1]*sqrt(par[2]) + par[3]*par[2]*sqrt(par[2]);
    value_0 = value_th - par[4]*par[2];
    value = value_0 + par[4]*x[0]; // pol1
  }
  //else { value=0; TF1::RejectPoint();}
  
  return value;
}

Double_t fitfunction2(Double_t *x, Double_t *par){
  Double_t value, value_th, value_0;
  if (x[0]<par[2]) {
    value = par[0]/(1+exp(-1*par[1]*(x[0]+par[3]))); // sigmoid
  }
  else
  {
    value_th = par[0]/(1+exp(-1*par[1]*(par[2]+par[3])));
    value_0 = value_th - par[4]*par[2];
    value = value_0 + par[4]*x[0]; // pol1
  }
  //else { value=0; TF1::RejectPoint();}
  
  return value;
}

Double_t fitfunction3(Double_t *x, Double_t *par){
  Double_t value, value_th, value_0;
  if (x[0]<par[2]) {
    value = par[0] + par[1]*atan(x[0]); // atan
  }
  else
  {
    value_th = par[0] + par[1]*atan(par[2]);
    value_0 = value_th - par[3]*par[2];
    value = value_0 + par[3]*x[0]; // pol1
  }
  //else { value=0; TF1::RejectPoint();}
  
  return value;
}


Double_t fitfunction4(Double_t *x, Double_t *par){ //atan + pol2
  Double_t value, value_th, value_0;
  if (x[0]<par[2]) {
    value = par[0] + par[1]*atan(x[0]); // atan
  }
  else
  {
    value_th = par[0] + par[1]*atan(par[2]);
    value_0 = value_th - par[3]*par[2]-par[4]*par[2]*par[2];
    value = value_0 + par[3]*x[0]+par[4]*x[0]*x[0]; // pol2
  }
  //else { value=0; TF1::RejectPoint();}
  
  return value;
}


Double_t fitfunction6(Double_t *x, Double_t *par){
  Double_t value, value_th, value_0;
  if (x[0]<par[2]) {
    value = par[0]/(1+exp(-1*par[1]*(x[0]+par[3]))); // sigmoid
  }
  else
  {
    value_th = par[0]/(1+exp(-1*par[1]*(par[2]+par[3])));
    value_0 = value_th - par[4]*par[2]-par[5]*par[2]*par[2];
    value = value_0 + par[4]*x[0]+par[5]*x[0]*x[0]; // pol2
  }
  //else { value=0; TF1::RejectPoint();}
  
  return value;
}


Double_t fitfunction7(Double_t *x, Double_t *par){
  Double_t value, value_th, value_0;
  if (x[0]<par[2]) {
    value = par[0]+exp(-1*par[1]*(x[0]+par[3])); 
  }
  else
  {
    value_th = par[0]+exp(-1*par[1]*(par[2]+par[3]));
    value_0 = value_th - par[4]*par[2];
    value = value_0 + par[4]*x[0]+par[5]*x[0]*x[0]; // pol2
  }
  //else { value=0; TF1::RejectPoint();}
  
  return value;
}


Double_t fitfunctionderiv(Double_t *x, Double_t *par){
  Double_t value, value_th, value_0;
  if (x[0]<par[2]) {
    value = par[0] + par[1]*exp(-1*par[3]*x[0]);
  }
  else
  {
    value_th = par[0] + par[1]*exp(-1*par[3]*par[2]);
    value_0 = value_th - par[4]*par[2];
    value = value_0 + par[4]*x[0]; // pol1
  }
  //else { value=0; TF1::RejectPoint();}
  
  return value;
}

Double_t fitfunctionderiv2(Double_t *x, Double_t *par){
  Double_t value, value_th, value_0;
  if (x[0]<par[2]) {
    value = par[0] + par[1]*x[0];
  }
  else
  {
    value_th = par[0] + par[1]*par[2];
    value_0 = value_th - par[3]*par[2];
    value = value_0 + par[3]*x[0]; // pol1
  }
  
  return value;
}


Double_t fitfunctionderiv3(Double_t *x, Double_t *par){ //gauss for run 3 
  Double_t value, value_th, value_0;
  
    value_0 = par[0];
    value = value_0 + par[1]*exp(-1*((par[2]-x[0])/par[3])*((par[2]-x[0])/par[3])); // gaus
   
  return value;

}

Double_t fitfunctionderiv4(Double_t *x, Double_t *par){ //tril for run 3 
  Double_t value, value_th, value_0;
  
  if (x[0]<par[2]) {
    value = par[0] + par[1]*x[0];
  }
  else if (par[2]<x[0] && x[0]<par[3])
  {
    value_th = par[0] + par[1]*par[2];
    value_0 = value_th - par[4]*par[2];
    value = value_0 + par[4]*x[0]; // pol1
  }
  else
    {
      value_th = par[0] + par[1]*par[2] - par[4]*par[2] + par[4]*par[3];
      value_0 = value_th - par[5]*par[3];
      value = value_0 + par[5]*x[0]; // pol1
    }
  return value;
  
}

Double_t fitfunctionderiv5(Double_t *x, Double_t *par){
  Double_t value;
  // if (x[0]<par[2]) {
  //   value = par[0] + par[1]*x[0];
  // }
  // else
  // {
  //   value_th = par[0] + par[1]*par[2];
  //   value_0 = value_th - par[3]*par[2];
  //   value = value_0 + par[3]*x[0]; // pol1
  // }
  value = par[0]-par[1]*exp(-par[2]*x[0]);
  // value = par[0]+par[1]*log(par[2]*x[0]);
  return value;
}

Double_t fitfunctionderiv6(Double_t *x, Double_t *par){
  Double_t value;
  // if (x[0]<par[2]) {
  //   value = par[0] + par[1]*x[0];
  // }
  // else
  // {
  //   value_th = par[0] + par[1]*par[2];
  //   value_0 = value_th - par[3]*par[2];
  //   value = value_0 + par[3]*x[0]; // pol1
  // }
  // value = par[0]+par[1]*exp(-par[2]*x[0]);
  value = par[0]-par[1]*log(par[2]*x[0]);
  return value;
}

Double_t fitfunctioncurv(Double_t *x, Double_t *par){
  Double_t value, value_0;

    value_0 = par[0];
    value = value_0 + par[1]*exp(-1*((par[2]-x[0])/par[3])*((par[2]-x[0])/par[3])); // gaus
   
  return value;
}

Double_t fitfunctionGeneral(Double_t *x, Double_t *par){
  Double_t value;

  value = -par[0]*atan(par[2]*x[0])+exp(par[3]*x[0]-par[4])+par[5]*x[0]*log(1+par[1]*x[0]*x[0]);
  return value;
}


Double_t fitfunctionderivGeneral(Double_t *x, Double_t *par){
  Double_t value;

  value = -par[0]/sqrt(par[1])*atan(sqrt(par[1])*(x[0]-par[2]))+exp(par[3]*x[0]-par[4])+(par[0]/sqrt(par[1]))*3.14/2;//the exponential can be divided by c
  return value;
}

Double_t fitfunctioncurvGeneral(Double_t *x, Double_t *par){
  Double_t value;
    value = -par[0]/(1+par[1]*(x[0]-par[2])*(x[0]-par[2])) +exp(par[3]*x[0]-par[4]); // gaus
  //  \frac{-a}{1+b\left(x-u\right)^{2}}+\exp\left(cx-d\right)
  return value;
}

#endif
