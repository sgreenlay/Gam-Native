extern "C" void logInt(int n);

extern "C" int add(int a, int b)
{
    logInt(a + b);
    return (a + b);
}