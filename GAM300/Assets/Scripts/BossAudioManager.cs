using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

public class BossAudioManager : Script
{
    public static BossAudioManager instance;

    //ADD A GAMEOBJECT FOR EACH SOUND

    //Other SFX

    //BOSS DIALOGUE
    public AudioSource deathVoiceOver;
    public AudioSource openingVoiceOver;
    public AudioSource phase2VoiceOver;

    //BOSS VOICE
    public AudioSource attack1SFX;
    public AudioSource attack2SFX;
    public AudioSource attack3SFX;
    public AudioSource attack4SFX;
    public AudioSource jumpAttackSFX;
    public AudioSource stunSFX;
    public AudioSource deathSFX;
    public AudioSource laughingSFX;
    public AudioSource screamSFX;
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

    }
}