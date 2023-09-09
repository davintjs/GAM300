using BeanFactory;
using System;
using System.Collections.Generic;

public class Player : BeanScript
{
    void Start()
    {
        Console.WriteLine("Start from Player");
    }

    void Update()
    {
        Console.WriteLine("Update from Player");
    }

    void Exit()
    {
        Console.WriteLine("Exit from Player");
    }
}