 __kernel void MatMul(int m, int n, int p, __global float* a, __global float* b, __global float* c)
 {
 float value = 0;
 // Zwischenspeicher zur Berechnung der Summe
 int i = get_global_id(0); // Zeilenindex von C
 int j = get_global_id(1); // Spaltenindex von C
 int k;
 // Laufvariable zur Summation
 for(k = 0; k < n; k++)
 {
 // Addiere aik bkj zur Summe.
 value += a[i * n + k] * b[k * p + j];
 }
 c[i * p + j] = value; // Schreibe Ergebnis in cij .
 }
