using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

public class AmbientAudioManager : Script
{
    public static AmbientAudioManager instance;

    //ADD A GAMEOBJECT FOR EACH SOUND
    public AudioSource ComputerAmbient;
    public AudioSource SparkAmbient;

    void Awake()
    {
        if (instance != null)
        {
            //Debug.LogError("More than one AudioManager in the scene.");
        }
        else
        {
            instance = this;
        }
    }

    void Start()
    {
        if (ComputerAmbient != null)
        {
            ComputerAmbient.Play();
        }

        if (SparkAmbient != null)
        {
            SparkAmbient.Play();
        }
    }

    void Update()
    {
        
        
    }
}