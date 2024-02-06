using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

public class AudioManager : Script
{
    public static AudioManager instance;

    //ADD A GAMEOBJECT FOR EACH SOUND
    public AudioSource playerSlashAttack;
    public AudioSource playerInjured;
    public AudioSource jumpVoice;
    public AudioSource jumpVoice2;
    public AudioSource jumpVoice3;
    public AudioSource spark;
    public AudioSource itemCollected;
    public AudioSource rangeEnemyFiring;
    public AudioSource rangeEnemyDead;
    public AudioSource doorOpen;
    public AudioSource elevator;
    public AudioSource playerFootstep;
    public AudioSource playerAttack;
    public AudioSource enemyHit;
    public AudioSource keyCollected;
    public AudioSource uiSound;
    public AudioSource useItem;
    public AudioSource meleeEnemyDie;
    public AudioSource meleeEnemyAttack;
    public AudioSource meleeEnemyInjured;
    public AudioSource playerOverdrive;
    public AudioSource overdriveVFXSound;

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

        if(playerSlashAttack == null)
        {
            Console.WriteLine("PlayerAttack Sound missing reference");
        }
        if(playerInjured == null)
        {
            Console.WriteLine("PlayerInjured Sound missing reference");
        }
        if (jumpVoice == null)
        {
            Console.WriteLine("JumpVoice Sound missing reference");
        }
        if (spark == null)
        {
            Console.WriteLine("Spark Sound missing reference");
        }
        if (itemCollected == null)
        {
            Console.WriteLine("ItemCollected Sound missing reference");
        }
        if (rangeEnemyFiring == null)
        {
            Console.WriteLine("RangeEnemyFiring Sound missing reference");
        }
        if (rangeEnemyDead == null)
        {
            Console.WriteLine("RangeEnemyDead Sound missing reference");
        }
        if (doorOpen == null)
        {
            Console.WriteLine("DoorOpen Sound missing reference");
        }
        if (elevator == null)
        {
            Console.WriteLine("Elevator Sound missing reference");
        }
        if (playerFootstep == null)
        {
            Console.WriteLine("PlayerFootstep Sound missing reference");
        }
        if (playerAttack == null)
        {
            Console.WriteLine("PlayerAttack Sound missing reference");
        }
        if (enemyHit == null)
        {
            Console.WriteLine("EnemyHit Sound missing reference");
        }
        if (keyCollected == null)
        {
            Console.WriteLine("keyCollected Sound missing reference");
        }
        if (uiSound == null)
        {
            Console.WriteLine("UI Sound missing reference");
        }
        if (useItem == null)
        {
            Console.WriteLine("UseItem Sound missing reference");
        }
        if (meleeEnemyDie == null)
        {
            Console.WriteLine("MeleeEnemyDie Sound missing reference");
        }
        if (meleeEnemyAttack == null)
        {
            Console.WriteLine("MeleeEnemyAttack Sound missing reference");
        }
        if (meleeEnemyInjured == null)
        {
            Console.WriteLine("MeleeEnemyInjured Sound missing reference");
        }
        if (playerOverdrive == null)
        {
            Console.WriteLine("PlayerOverdrive Sound missing reference");
        }
        if (overdriveVFXSound == null)
        {
            Console.WriteLine("OverdriveVFX Sound missing reference");
        }

    }

    void Start()
    {

    }
}