using System;

public class CSharpTest
{
    public float floatVar = 5.0f;

    public void PrintFloatVar()
    {
        Console.WriteLine("FloatVar = {0:f}", floatVar);
    }

    private void IncrementFloatVar(float val)
    {
        floatVar += val;
    }
}