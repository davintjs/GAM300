using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

public class GameManager : Script
{
    static public GameManager instance;
    public AudioSource BGM_Source;

    void Awake()
    {
        instance = this;
        if (BGM_Source != null)
            BGM_Source.Play();
    }

}