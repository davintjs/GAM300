using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

public class PlayerAudioManager : Script
{
    public static PlayerAudioManager instance;

    //ADD A GAMEOBJECT FOR EACH SOUND
    public AudioSource PlayerSlashAttack;
    public AudioSource Spark;
    public AudioSource JumpVoice;
    public AudioSource PlayerInjured;
    public AudioSource PlayerFootstep;
    public AudioSource PlayerAttack;
    public AudioSource UseItem;
    public AudioSource PlayerOverdrive;
    public AudioSource JumpVoice2;
    public AudioSource JumpVoice3;
    public AudioSource Thump1;
    public AudioSource Thump2;
    public AudioSource DodgeRoll1;
    public AudioSource DodgeRoll2;
    public AudioSource DashAttack;
    public AudioSource PowerUp;
    public AudioSource OverdriveVFXSound;
    public AudioSource JumpOffGroundSound;
    public AudioSource HitGroundSound;
    public AudioSource LowHealthSound;
    public AudioSource LowHealthHeartbeatSound;

    public AudioSource DoorOpen;
    public AudioSource Elevator;

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