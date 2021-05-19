
unsigned long long int MultModulo(unsigned long long int a, unsigned long long int b, unsigned long long int mod) {
  unsigned long long int result = 0;
  a = a % mod;
  while (b > 0) {
    if (b % 2 == 1)
      result = (result + a) % mod;
    a = (a * 2) % mod;
    b /= 2;
  }

  return result % mod;
}